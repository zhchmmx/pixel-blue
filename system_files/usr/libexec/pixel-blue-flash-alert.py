#!/usr/bin/python3
"""Failed-firmware-flash alert for Pixel Blue (Issue #69).

The fwupd plugin runs as root in the system domain and cannot reach the
user session bus; it leaves a marker at
/var/lib/pixel-blue/fwupd/flash-failed (class=A|B, state=inprogress|failed,
attempts=N).  pixel-blue-flash-alert.path starts this script the moment the
marker appears (and on next login if it already exists).

This is a resident GLib main loop rather than a one-shot `gdbus call`
notifier, which buys three things:

  * the notification carries an "open updates page" action; receiving its
    ActionInvoked signal requires a live D-Bus connection for the whole
    lifetime of the alert,
  * while the situation is actually dangerous (class B) the loop holds a
    `systemd-inhibit --what=shutdown:sleep --mode=block` lock, so a normal
    reboot from GNOME hits the standard "an app is blocking shutdown"
    dialog and bypassing it takes an explicit override; class A failures
    never block shutdown because there the flash is intact and rebooting
    is harmless (blocking anyway would spend the user's trust),
  * a daemon crash mid-flash runs no error path at all; the alert picks up
    a state=inprogress marker whose mtime has gone stale (older than
    GRACE) and treats it as the worst case.

Deliberately repetitive: notifications repeat every INTERVAL until the
marker disappears (a successful retry removes it; the documented manual
stop-loss is `sudo rm` of the marker).
"""

import os
import sys
import time
import traceback

from gi.repository import Gio, GLib

STATE_DIR = os.environ.get("MRCHROMEBOX_STATE_DIR", "/var/lib/pixel-blue/fwupd")
MARKER = os.path.join(STATE_DIR, "flash-failed")
INTERVAL = float(os.environ.get("MRCHROMEBOX_ALERT_INTERVAL", "300"))
# inprogress marker considered stale (and therefore a crashed writer) only
# after this much time without any rewrite; a healthy attempt rewrites the
# sentinel when the write starts and removes it on completion, both well
# inside GRACE.
GRACE = float(os.environ.get("MRCHROMEBOX_ALERT_GRACE", "600"))
POLL_INTERVAL = 15.0  # seconds between marker re-reads

APP_NAME = "pixel-blue-flash-alert"
ACTION_OPEN_UPDATES = "open-updates"
NOTIFY_DEST = "org.freedesktop.Notifications"
NOTIFY_PATH = "/org/freedesktop/Notifications"
INHIBIT_BIN = "/usr/bin/systemd-inhibit"
SLEEP_BIN = "/usr/bin/sleep"


def read_marker():
    """Return (fields, age_seconds) or None when the marker is gone."""
    try:
        with open(MARKER, encoding="utf-8") as handle:
            text = handle.read()
        mtime = os.path.getmtime(MARKER)
    except FileNotFoundError:
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


def should_notify(fields, age):
    """A failed marker alerts immediately; an inprogress one only once stale."""
    return fields["state"] == "failed" or age >= GRACE


def is_dangerous(fields, age):
    """Class B plus a finished failure, or a class B sentinel gone stale."""
    if fields["class"] != "B":
        return False
    return fields["state"] == "failed" or age >= GRACE


def message(fields, age):
    """(title, body) graded by class and state, same copy as the old shell alert."""
    attempts = fields.get("attempts", "1")
    dangerous = is_dangerous(fields, age)
    if fields["state"] == "failed":
        if dangerous:
            title = "\u26a0\ufe0f 固件更新失败——千万不要重启电脑"
            body = (
                f"固件可能不完整，此时重启将无法开机。回到 GNOME 软件「更新」页再点一次「更新」重试"
                f"（已失败 {attempts} 次）。在重试成功前，关机与挂起已被临时禁止。"
            )
        else:
            title = "\u26a0\ufe0f 固件更新失败（当前固件完好，重启无害）"
            body = (
                "写入尚未开始，固件未被修改，可以正常重启或关机。回到 GNOME 软件「更新」页再点一次"
                f"「更新」重试（已失败 {attempts} 次）。"
            )
    else:
        # stale in-progress sentinel: the daemon died mid-attempt without
        # running any error path; class B means the write may have started.
        if dangerous:
            title = "\u26a0\ufe0f 固件更新疑似异常中止——千万不要重启电脑"
            body = (
                "更新进程在写入过程中消失，固件可能不完整，此时重启可能无法开机。"
                "回到 GNOME 软件「更新」页再点一次「更新」重试；关机与挂起已被临时禁止。"
            )
        else:
            title = "\u26a0\ufe0f 固件更新疑似异常中止（当前固件完好，重启无害）"
            body = "更新进程在写入开始前消失，固件未被修改，重启无害。回到「更新」页重试即可。"
    if attempts.isdigit() and int(attempts) >= 3:
        if dangerous:
            body += " 连续失败请保持开机，另开终端运行 sudo firmware-util.sh 排查——仍然不要重启。"
        else:
            body += " 连续失败请另开终端运行 sudo firmware-util.sh 排查。"
    return title, body


class FlashAlert:
    def __init__(self, loop):
        self.loop = loop
        self.replace_id = 0
        # monotonic clock starts at boot, so seed the gate to fire the first
        # notification immediately even on a freshly started machine
        self.last_notify = -INTERVAL
        self.inhibit = None
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

    # --- notifications ------------------------------------------------------

    def _notify(self, title, body):
        params = GLib.Variant(
            "(susssasa{sv}i)",
            (
                APP_NAME,
                self.replace_id,
                "dialog-error",
                title,
                body,
                [ACTION_OPEN_UPDATES, "打开更新页"],
                {"urgency": GLib.Variant("i", 2)},
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
            return
        if reply is not None:
            self.replace_id = reply.unpack()[0]

    def _close(self):
        if not self.replace_id:
            return
        try:
            self.bus.call_sync(
                NOTIFY_DEST,
                NOTIFY_PATH,
                NOTIFY_DEST,
                "CloseNotification",
                GLib.Variant("(u)", (self.replace_id,)),
                None,
                Gio.DBusCallFlags.NONE,
                -1,
                None,
            )
        except GLib.Error:
            pass

    def _on_action(self, _conn, _sender, _path, _iface, _signal, params):
        notify_id, action_key = params.unpack()
        if action_key == ACTION_OPEN_UPDATES and notify_id == self.replace_id:
            self._open_updates_page()

    def _open_updates_page(self):
        # Preferred path: activate GNOME Software's exported application
        # action "set-mode" ('updates') straight on the session bus.  This
        # works whether or not an instance is already running (D-Bus
        # auto-start launches it in the session's own environment), so the
        # notifier unit never needs a compositor environment itself.
        try:
            self.bus.call_sync(
                "org.gnome.Software",
                "/org/gnome/Software",
                "org.gtk.Actions",
                "Activate",
                GLib.Variant(
                    "(ssa{sv})",
                    ("set-mode", GLib.Variant("s", "updates"), {}),
                ),
                None,
                Gio.DBusCallFlags.NONE,
                -1,
                None,
            )
            return
        except GLib.Error as exc:
            print(
                f"{APP_NAME}: GNOME Software action failed ({exc}); spawning",
                file=sys.stderr,
            )
        # Fallback: plain launch; only works when the unit inherited the
        # session display environment.
        try:
            GLib.spawn_async(
                argv=["gnome-software", "--mode", "updates"],
                flags=GLib.SpawnFlags.SEARCH_PATH,
            )
        except GLib.Error as exc:
            print(f"{APP_NAME}: cannot open GNOME Software: {exc}", file=sys.stderr)

    # --- shutdown/sleep inhibit (class B only) -------------------------------

    def _start_inhibit(self):
        if self.inhibit is not None or not os.access(INHIBIT_BIN, os.X_OK):
            return
        proc = Gio.Subprocess.new(
            [
                INHIBIT_BIN,
                "--what=shutdown:sleep",
                "--mode=block",
                "--who=" + APP_NAME,
                "--why=MrChromebox 固件更新失败，固件可能不完整，重启可能导致无法开机",
                "--",
                SLEEP_BIN,
                "infinity",
            ],
            Gio.SubprocessFlags.STDOUT_SILENCE | Gio.SubprocessFlags.STDERR_SILENCE,
        )
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
            # SIGTERM releases the inhibitor lock immediately; the pending
            # wait_async reaps the child (its callback no-ops because
            # self.inhibit is already None/different)
            proc.force_quit()

    # --- marker polling ------------------------------------------------------

    def tick(self):
        marker = read_marker()
        if marker is None:
            self._stop_inhibit()
            self._close()
            self.loop.quit()
            return False
        fields, age = marker
        if is_dangerous(fields, age):
            self._start_inhibit()
        else:
            self._stop_inhibit()
        now = time.monotonic()
        if should_notify(fields, age) and now - self.last_notify >= INTERVAL:
            self._notify(*message(fields, age))
            self.last_notify = now
        return True


def main():
    if not os.path.exists(MARKER):
        # The path unit can fire right as a retry removes the marker; the old
        # shell loop exited immediately in that case too.
        return 0
    try:
        loop = GLib.MainLoop()
        alert = FlashAlert(loop)
        if not alert.tick():
            return 0
        GLib.timeout_add(int(POLL_INTERVAL * 1000), alert.tick)
        # the only normal exit is tick() quitting the loop after it stopped
        # the inhibit, closed the notification and cleaned up
        loop.run()
    except Exception:
        # a session-bus race etc. must not end the alert: exit non-zero and
        # let the unit's Restart=on-failure bring us back
        traceback.print_exc()
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
