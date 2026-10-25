#!/usr/bin/python3
"""Resident firmware alert and update notifier for Pixel Blue (Issue #69).

Started as a user service at login (pixel-blue-firmware-alert.service).  It
watches the root-owned state directory and does two jobs:

  * failed flash (flash-failed marker, written by pixel-blue-firmware-daemon,
    class=A|B, state=inprogress|failed, attempts=N): a graded notification
    repeats every INTERVAL until the marker disappears.  Class B (the SPI
    write started) additionally holds a shutdown:sleep inhibitor, so a normal
    reboot from GNOME hits the "an app is blocking shutdown" dialog.  Class A
    (flash untouched) never blocks shutdown: there a reboot is harmless, and
    blocking anyway would only spend the user's trust.  A daemon crash
    mid-flash runs no error path at all; an inprogress marker whose mtime is
    older than GRACE is treated as the worst case.
  * available update (check.json, written by firmware-update-check.timer): one
    notification per available_date, tracked in
    $XDG_STATE_HOME/pixel-blue/notified-<date>.

This is a resident GLib main loop rather than a one-shot `gdbus call`
notifier: the notification carries an "open updater" action whose
ActionInvoked signal requires a live session-bus connection, and the
class-B inhibitor must live as long as the danger does.  Gio.FileMonitor
gives immediate reaction to marker/check updates; the periodic tick is a
backstop for filesystems where directory monitoring is unreliable.

Deliberately repetitive: failure notifications repeat until the marker
disappears (a successful retry removes it; the documented manual stop-loss
is `sudo rm` of the marker).
"""

import json
import os
import re
import sys
import time
import traceback

from gi.repository import Gio, GLib

STATE_DIR = os.environ.get("MRCHROMEBOX_STATE_DIR", "/var/lib/pixel-blue/firmware")
MARKER = os.path.join(STATE_DIR, "flash-failed")
CHECK_JSON = os.path.join(STATE_DIR, "check.json")
NEEDS_REBOOT = os.path.join(STATE_DIR, "needs-reboot")
INTERVAL = float(os.environ.get("MRCHROMEBOX_ALERT_INTERVAL", "300"))
# inprogress marker considered stale (and therefore a crashed writer) only
# after this much time without any rewrite; a healthy attempt rewrites the
# sentinel when the write starts and removes it on completion, both well
# inside GRACE.
GRACE = float(os.environ.get("MRCHROMEBOX_ALERT_GRACE", "600"))
# testability overrides; the shipped service uses the defaults
POLL_INTERVAL = float(os.environ.get("MRCHROMEBOX_ALERT_POLL", "15"))  # seconds between marker re-reads

APP_NAME = "pixel-blue-flash-alert"
ACTION_OPEN_UPDATER = "open-updater"
NOTIFY_DEST = "org.freedesktop.Notifications"
NOTIFY_PATH = "/org/freedesktop/Notifications"
UPDATER_DEST = "tech.pixelblue.Updater"
UPDATER_PATH = "/tech/pixelblue/Updater"
UPDATER_BIN = "pixel-blue-updater"
INHIBIT_BIN = os.environ.get("MRCHROMEBOX_INHIBIT_BIN", "/usr/bin/systemd-inhibit")
SLEEP_BIN = "/usr/bin/sleep"


def read_marker():
    """Return (fields, age_seconds) or None when the marker is gone."""
    try:
        with open(MARKER, encoding="utf-8") as handle:
            text = handle.read()
        mtime = os.path.getmtime(MARKER)
    except OSError:
        return None
    fields = {}
    for line in text.splitlines():
        key, sep, value = line.partition("=")
        if sep and key in ("class", "state", "attempts"):
            fields[key] = value.strip()
    # Legacy markers (pre-state builds) were only ever written on failure.
    fields.setdefault("class", "A")
    fields.setdefault("state", "failed")
    return fields, max(0.0, time.time() - mtime)


def read_check():
    try:
        with open(CHECK_JSON, encoding="utf-8") as handle:
            value = json.load(handle)
    except (OSError, ValueError):
        return {}
    return value if isinstance(value, dict) else {}


def should_notify(fields, age):
    """A failed marker alerts immediately; an inprogress one only once stale."""
    return fields["state"] == "failed" or age >= GRACE


def is_dangerous(fields, age):
    """Class B plus a finished failure, or a class B sentinel gone stale."""
    if fields["class"] != "B":
        return False
    return fields["state"] == "failed" or age >= GRACE


def message(fields, age):
    """(title, body) graded by class and state."""
    attempts = fields.get("attempts", "1")
    dangerous = is_dangerous(fields, age)
    if fields["state"] == "failed":
        if dangerous:
            title = "\u26a0\ufe0f 固件更新失败——千万不要重启电脑"
            body = (
                f"固件可能不完整，此时重启将无法开机。回到更新器再点一次「安装」重试"
                f"（已失败 {attempts} 次）。在重试成功前，关机与挂起已被临时禁止。"
            )
        else:
            title = "\u26a0\ufe0f 固件更新失败（当前固件完好，重启无害）"
            body = (
                "写入尚未开始，固件未被修改，可以正常重启或关机。回到更新器再点一次"
                f"「安装」重试（已失败 {attempts} 次）。"
            )
    else:
        # stale in-progress sentinel: the daemon died mid-attempt without
        # running any error path; class B means the write may have started.
        if dangerous:
            title = "\u26a0\ufe0f 固件更新疑似异常中止——千万不要重启电脑"
            body = (
                "更新进程在写入过程中消失，固件可能不完整，此时重启可能无法开机。"
                "回到更新器再点一次「安装」重试；关机与挂起已被临时禁止。"
            )
        else:
            title = "\u26a0\ufe0f 固件更新疑似异常中止（当前固件完好，重启无害）"
            body = "更新进程在写入开始前消失，固件未被修改，重启无害。回到更新器重试即可。"
    if attempts.isdigit() and int(attempts) >= 3:
        if dangerous:
            body += " 连续失败请保持开机，查看 README 的手动排查步骤——仍然不要重启。"
        else:
            body += " 连续失败请另开终端查看 README 的手动排查步骤。"
    return title, body


def state_home():
    base = os.environ.get("XDG_STATE_HOME") or os.path.join(os.path.expanduser("~"), ".local", "state")
    directory = os.path.join(base, "pixel-blue")
    os.makedirs(directory, mode=0o700, exist_ok=True)
    return directory


def notified_path(available_date):
    return os.path.join(state_home(), f"notified-{available_date}")


class FlashAlert:
    def __init__(self, loop):
        self.loop = loop
        self.failure_id = 0
        self.update_id = 0
        # monotonic clock starts at boot, so seed the gate to fire the first
        # notification immediately even on a freshly started machine
        self.last_failure_notify = -INTERVAL
        self.inhibit = None
        self.notified_date = ""
        self.tick_pending = False
        self.bus = Gio.bus_get_sync(Gio.BusType.SESSION, None)
        self.bus.signal_subscribe(
            None,
            NOTIFY_DEST,
            "ActionInvoked",
            NOTIFY_PATH,
            None,
            Gio.DBusSignalFlags.NONE,
            self._on_action,
        )
        self.monitor = None
        self._arm_monitor()

    def _arm_monitor(self):
        """Watch STATE_DIR, arming again once it exists.

        On a fresh login the root-owned state directory may not exist yet
        (systemd creates it when the daemon starts, the timer at the first
        check).  monitor_directory() on a missing path raises, and the
        service must survive that: the periodic tick still reads the files
        and calls back here to arm the monitor once the directory appears.
        """
        if self.monitor is not None or not os.path.isdir(STATE_DIR):
            return
        try:
            monitor = Gio.File.new_for_path(STATE_DIR).monitor_directory(Gio.FileMonitorFlags.NONE, None)
        except GLib.Error as exc:
            print(f"{APP_NAME}: cannot monitor {STATE_DIR} yet: {exc}", file=sys.stderr)
            return
        monitor.connect("changed", self._on_file_changed)
        self.monitor = monitor

    # --- notifications ------------------------------------------------------

    def _notify(self, title, body, icon, urgency, replace_id):
        params = GLib.Variant(
            "(susssasa{sv}i)",
            (
                APP_NAME,
                replace_id,
                icon,
                title,
                body,
                [ACTION_OPEN_UPDATER, "打开更新器"],
                {"urgency": GLib.Variant("i", urgency)},
                0,
            ),
        )
        try:
            reply = self.bus.call_sync(
                NOTIFY_DEST,
                NOTIFY_PATH,
                NOTIFY_DEST,
                "Notify",
                params,
                GLib.VariantType("(u)"),
                Gio.DBusCallFlags.NONE,
                -1,
                None,
            )
        except GLib.Error:
            return 0
        if reply is None:
            return 0
        return int(reply.unpack()[0])

    def _close(self, notify_id):
        if not notify_id:
            return
        try:
            self.bus.call_sync(
                NOTIFY_DEST,
                NOTIFY_PATH,
                NOTIFY_DEST,
                "CloseNotification",
                GLib.Variant("(u)", (notify_id,)),
                None,
                Gio.DBusCallFlags.NONE,
                -1,
                None,
            )
        except GLib.Error:
            pass

    def _on_action(self, _conn, _sender, _path, _iface, _signal, params):
        notify_id, action_key = params.unpack()
        if action_key != ACTION_OPEN_UPDATER:
            return
        if notify_id == self.failure_id or notify_id == self.update_id:
            self._open_updater()

    def _open_updater(self):
        # Preferred path: the app is already running (or DBusActivatable via
        # its desktop file); activate its "open" action on the session bus.
        try:
            self.bus.call_sync(
                UPDATER_DEST,
                UPDATER_PATH,
                "org.gtk.Actions",
                "Activate",
                GLib.Variant("(sava{sv})", ("open", [], {})),
                None,
                Gio.DBusCallFlags.NONE,
                2000,
                None,
            )
            return
        except GLib.Error as exc:
            print(f"{APP_NAME}: updater activation failed ({exc}); spawning", file=sys.stderr)
        try:
            GLib.spawn_async(
                argv=[UPDATER_BIN],
                flags=GLib.SpawnFlags.SEARCH_PATH,
            )
        except GLib.Error as exc:
            print(f"{APP_NAME}: cannot open the updater: {exc}", file=sys.stderr)

    def _close_failure(self):
        self._close(self.failure_id)
        self.failure_id = 0

    def _close_update(self):
        self._close(self.update_id)
        self.update_id = 0

    # --- failure alert ------------------------------------------------------

    def _start_inhibit(self):
        if self.inhibit is not None or not os.access(INHIBIT_BIN, os.X_OK):
            return
        try:
            proc = Gio.Subprocess.new(
                [
                    INHIBIT_BIN,
                    "--what=shutdown:sleep",
                    "--mode=block",
                    "--who=" + APP_NAME,
                    "--why=Pixelbook 固件更新未完成，固件可能不完整，重启可能导致无法开机",
                    "--",
                    SLEEP_BIN,
                    "infinity",
                ],
                Gio.SubprocessFlags.STDOUT_SILENCE | Gio.SubprocessFlags.STDERR_SILENCE,
            )
        except GLib.Error as exc:
            print(f"{APP_NAME}: cannot start shutdown inhibitor: {exc}", file=sys.stderr)
            return
        self.inhibit = proc
        # if logind refuses the lock and systemd-inhibit exits, forget it so
        # the next dangerous poll respawns it; otherwise we would believe a
        # shutdown block is held when none is
        proc.wait_async(None, self._on_inhibit_exit, proc)

    def _on_inhibit_exit(self, source, result, proc):
        try:
            source.wait_finish(result)
        except GLib.Error:
            pass
        if self.inhibit is proc:
            self.inhibit = None

    def _stop_inhibit(self):
        proc = self.inhibit
        self.inhibit = None
        if proc is not None:
            # SIGKILL releases the inhibitor lock immediately; the pending
            # wait_async reaps the child (its callback no-ops because
            # self.inhibit is already None/different)
            proc.force_exit()

    # --- update notice ------------------------------------------------------

    def _maybe_notify_update(self):
        if read_marker() is not None or os.path.exists(NEEDS_REBOOT):
            self._close_update()
            return
        check = read_check()
        available_date = check.get("available_date")
        if not isinstance(available_date, str) or not re.fullmatch(r"[0-9]{8}", available_date):
            self._close_update()
            return
        if not check.get("downloaded"):
            self._close_update()
            return
        if available_date != self.notified_date:
            self.notified_date = available_date
            self._cleanup_flags(available_date)
        if os.path.exists(notified_path(available_date)) or self.update_id:
            return
        version = check.get("available_version") or available_date
        release = check.get("release_date") or ""
        body = f"{version}"
        if release:
            body += f"（{release}）"
        body += " 已下载并通过校验，打开更新器即可安装。"
        notify_id = self._notify("有可用的 Pixelbook 固件更新", body, "system-software-update", 1, self.update_id)
        if notify_id:
            self.update_id = notify_id
            self._mark_notified(available_date)

    def _cleanup_flags(self, keep):
        directory = state_home()
        keep_name = f"notified-{keep}"
        try:
            names = os.listdir(directory)
        except OSError:
            return
        for name in names:
            if name.startswith("notified-") and name != keep_name:
                try:
                    os.unlink(os.path.join(directory, name))
                except OSError:
                    pass

    def _mark_notified(self, available_date):
        path = notified_path(available_date)
        try:
            with open(path, "w", encoding="utf-8") as handle:
                handle.write(available_date + "\n")
            os.chmod(path, 0o644)
        except OSError as exc:
            print(f"{APP_NAME}: cannot record notification: {exc}", file=sys.stderr)

    # --- polling ------------------------------------------------------------

    def _on_file_changed(self, _monitor, _file, _other_file, _event):
        if self.tick_pending:
            return
        self.tick_pending = True
        GLib.timeout_add(300, self._run_pending_tick)

    def _run_pending_tick(self):
        self.tick_pending = False
        self.tick()
        return False

    def tick(self):
        self._arm_monitor()
        marker = read_marker()
        if marker is None:
            self._stop_inhibit()
            self._close_failure()
            # a fresh failure after a clean state must notify immediately
            self.last_failure_notify = -INTERVAL
        else:
            fields, age = marker
            if is_dangerous(fields, age):
                self._start_inhibit()
            else:
                self._stop_inhibit()
            now = time.monotonic()
            if should_notify(fields, age) and now - self.last_failure_notify >= INTERVAL:
                title, body = message(fields, age)
                notify_id = self._notify(title, body, "dialog-error", 2, self.failure_id)
                if notify_id:
                    self.failure_id = notify_id
                self.last_failure_notify = now
        self._maybe_notify_update()
        return True


def main():
    try:
        loop = GLib.MainLoop()
        alert = FlashAlert(loop)
        # the only normal exit is the session/systemd tearing us down; the
        # service stays resident so update notices and class-B inhibits do
        # not depend on the exact moment a file appeared
        alert.tick()

        # GLib.timeout_add() removes the source when the callback returns
        # FALSE/None, so the repeat must be explicit: this tick is the
        # backstop when FileMonitor cannot watch the state directory (missing
        # or a filesystem without change notification) and it also re-sends
        # the failure notification every INTERVAL until the marker is gone.
        def periodic_tick():
            alert.tick()
            return True

        GLib.timeout_add(int(POLL_INTERVAL * 1000), periodic_tick)
        loop.run()
    except Exception:
        # a session-bus race etc. must not end the alert: exit non-zero and
        # let the unit's Restart=on-failure bring us back
        traceback.print_exc()
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
