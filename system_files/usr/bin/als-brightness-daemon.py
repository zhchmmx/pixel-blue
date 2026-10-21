#!/usr/bin/python3
"""Ambient-light auto-brightness daemon for Pixel Blue.

This daemon reproduces the ChromeOS powerd behaviour on the Pixelbook (eve):

  * read the ambient light sensor straight from IIO sysfs (the sensor stays
    visible to iio-sensor-proxy so GNOME keeps showing its auto-brightness
    switch, but a D-Bus policy blocks gnome-settings-daemon from claiming it
    and fighting us for the backlight),
  * smooth the raw lux readings with an exponential moving average,
  * pick one of the seven eve brightness steps, using the vendor's down/up
    lux thresholds as hysteresis,
  * convert the step's user-visible percentage to a hardware level with the
    ChromeOS square curve,
  * ramp to the new level and write it through systemd-logind
    (Session.SetBrightness, no root required),
  * yield control when somebody else changes the brightness, and take it back
    once the ambient light has changed by more than 100 lux.

The step table lives in /etc/pixel-blue/als-steps.conf (preferred) or
/usr/lib/pixel-blue/als-steps.conf.
"""

import glob
import logging
import math
import os
import signal
import sys
import time
from dataclasses import dataclass
from pathlib import Path

import dbus

LOG = logging.getLogger("als-brightness")

CONF_PATHS = (
    Path("/etc/pixel-blue/als-steps.conf"),
    Path("/usr/lib/pixel-blue/als-steps.conf"),
)

POLL_INTERVAL = 1.0  # seconds between ambient light samples
EMA_TIME_CONSTANT = 2.0  # low-pass filter time constant, seconds
SUSPEND_GAP = 5.0  # treat a larger sample gap as a suspend/resume
RAMP_DURATION = 0.5  # seconds, matches the powerd transition
RAMP_TICK = 0.03
MANUAL_TOLERANCE = 2.0  # percent of max brightness treated as "not ours"
RECLAIM_LUX = 100.0  # ambient change needed to take control back
MIN_VISIBLE_FRACTION = 0.006  # eve min_visible_level / max_level (~0.6%)
CURVE_OFFSET = 6.25  # ChromeOS: lowest usable user-visible percentage
ERROR_LOG_INTERVAL = 60.0


@dataclass(frozen=True)
class Step:
    ui_percent: float
    brightness_percent: float
    down_lux: float
    up_lux: float

    @property
    def has_down(self) -> bool:
        return self.down_lux >= 0

    @property
    def has_up(self) -> bool:
        return self.up_lux >= 0


def load_steps() -> list:
    for path in CONF_PATHS:
        if not path.is_file():
            continue
        steps = []
        with path.open(encoding="utf-8") as handle:
            for lineno, line in enumerate(handle, start=1):
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                fields = line.split()
                if len(fields) != 4:
                    raise ValueError(f"{path}:{lineno}: expected 4 columns, got {len(fields)}")
                try:
                    steps.append(Step(*(float(field) for field in fields)))
                except ValueError as exc:
                    raise ValueError(f"{path}:{lineno}: {exc}") from exc
        if not steps:
            raise ValueError(f"{path}: no brightness steps found")
        LOG.info("loaded %d brightness steps from %s", len(steps), path)
        return steps
    raise FileNotFoundError("no als-steps.conf found")


def find_sensor() -> tuple:
    """Return (path, scale) for the first ambient light sensor in sysfs."""
    for device in sorted(glob.glob("/sys/bus/iio/devices/iio:device*")):
        base = Path(device)
        direct = base / "in_illuminance_input"
        if direct.exists():
            return direct, 1.0, "lux"
        raw = base / "in_illuminance_raw"
        if raw.exists():
            scale = 1.0
            scale_path = base / "in_illuminance_scale"
            try:
                scale = float(scale_path.read_text(encoding="utf-8").strip())
            except (OSError, ValueError):
                pass
            return raw, scale, "raw"
    raise FileNotFoundError("no IIO ambient light sensor found")


def read_lux(sensor: Path, scale: float) -> float | None:
    try:
        return max(0.0, float(sensor.read_text(encoding="utf-8").strip()) * scale)
    except (OSError, ValueError):
        return None


def find_backlight() -> tuple:
    """Return (device name, max_level, brightness path) for the panel."""
    preferred = Path("/sys/class/backlight/intel_backlight")
    candidates = []
    if preferred.exists():
        candidates.append(preferred)
    candidates.extend(
        Path(entry) for entry in sorted(glob.glob("/sys/class/backlight/*"))
        if Path(entry) != preferred
    )
    for directory in candidates:
        try:
            max_level = int((directory / "max_brightness").read_text(encoding="utf-8").strip())
        except (OSError, ValueError):
            continue
        if max_level > 0:
            return directory.name, max_level, directory / "brightness"
    raise FileNotFoundError("no backlight device found")


def read_brightness_percent(brightness: Path, max_level: int) -> float | None:
    try:
        raw = int(brightness.read_text(encoding="utf-8").strip())
    except (OSError, ValueError):
        return None
    return 100.0 * raw / max_level


def ui_to_hardware_percent(ui_percent: float, min_visible: int, max_level: int) -> float:
    """ChromeOS PercentToLevel(), expressed as a percentage of max_level."""
    fraction = (ui_percent - CURVE_OFFSET) / (100.0 - CURVE_OFFSET)
    fraction = min(1.0, max(0.0, fraction))
    min_percent = 100.0 * min_visible / max_level
    return min_percent + fraction * fraction * (100.0 - min_percent)


def select_step(steps: list, current: int, lux: float) -> int:
    while (
        current + 1 < len(steps)
        and steps[current].has_up
        and lux > steps[current].up_lux
    ):
        current += 1
    while (
        current > 0
        and steps[current].has_down
        and lux < steps[current].down_lux
    ):
        current -= 1
    return current


def find_logind_session(bus, manager) -> str:
    """Return the logind session that is allowed to write the backlight.

    Services started by the per-user systemd manager live in user@UID.service,
    which is not part of any logind session, so GetSessionByPID() on our own
    PID fails with "PID ... does not belong to any known session". Fall back
    to XDG_SESSION_ID and then to the active session of the same user.
    """
    session_id = os.environ.get("XDG_SESSION_ID", "").strip()
    if session_id:
        try:
            return str(manager.GetSession(session_id))
        except dbus.DBusException:
            pass

    try:
        return str(manager.GetSessionByPID(dbus.UInt32(os.getpid())))
    except dbus.DBusException:
        pass

    uid = os.getuid()
    fallback = None
    for _id, session_uid, _user, _seat, path in manager.ListSessions():
        if int(session_uid) != uid:
            continue
        if fallback is None:
            fallback = str(path)
        props = dbus.Interface(
            bus.get_object("org.freedesktop.login1", path),
            "org.freedesktop.DBus.Properties",
        )
        try:
            if bool(props.Get("org.freedesktop.login1.Session", "Active")):
                return str(path)
        except dbus.DBusException:
            continue

    if fallback is not None:
        return fallback

    raise RuntimeError("no logind session found for uid %d" % uid)


class Backlight:
    """Write the panel brightness through systemd-logind."""

    def __init__(self, device: str, max_level: int) -> None:
        self._device = device
        self._max_level = max_level
        bus = dbus.SystemBus()
        manager = dbus.Interface(
            bus.get_object("org.freedesktop.login1", "/org/freedesktop/login1"),
            "org.freedesktop.login1.Manager",
        )
        session_path = find_logind_session(bus, manager)
        self._session = dbus.Interface(
            bus.get_object("org.freedesktop.login1", session_path),
            "org.freedesktop.login1.Session",
        )
        self._last_error = 0.0

    def set_percent(self, percent: float) -> bool:
        # logind's SetBrightness() takes an absolute value in the
        # 0..max_brightness range and writes it straight to sysfs, not a
        # percentage.
        value = int(round(percent * self._max_level / 100.0))
        value = max(1, min(self._max_level, value))
        try:
            self._session.SetBrightness("backlight", self._device, dbus.UInt32(value))
        except dbus.DBusException as exc:
            now = time.monotonic()
            if now - self._last_error >= ERROR_LOG_INTERVAL:
                self._last_error = now
                LOG.error(
                    "SetBrightness(%d of %d) failed: %s", value, self._max_level, exc
                )
            return False
        return True


def ramp(backlight: Backlight, start: float, target: float, last_sent: list) -> None:
    delta = target - start
    if abs(delta) < 1.0:
        return
    steps = max(1, min(20, int(abs(delta))))
    tick = min(RAMP_TICK, RAMP_DURATION / steps)
    for index in range(1, steps + 1):
        percent = start + delta * index / steps
        value = max(1, min(100, int(round(percent))))
        if last_sent[0] is None or value != last_sent[0]:
            if backlight.set_percent(percent):
                last_sent[0] = value
        time.sleep(tick)


def main() -> int:
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s %(levelname)s %(message)s",
        stream=sys.stderr,
    )

    try:
        steps = load_steps()
        sensor, scale, unit = find_sensor()
        device, max_level, brightness_path = find_backlight()
    except (OSError, ValueError) as exc:
        LOG.error("%s", exc)
        return 1

    min_visible = max(1, int(round(max_level * MIN_VISIBLE_FRACTION)))
    LOG.info(
        "sensor=%s (%s), backlight=%s (max %d, min visible %d)",
        sensor, unit, device, max_level, min_visible,
    )

    try:
        backlight = Backlight(device, max_level)
    except (dbus.DBusException, RuntimeError) as exc:
        LOG.error("cannot attach to the logind session: %s", exc)
        return 1

    running = True

    def stop(_signum, _frame):
        nonlocal running
        running = False

    signal.signal(signal.SIGTERM, stop)
    signal.signal(signal.SIGINT, stop)

    current_step = 0
    smoothed = None
    last_sample = time.monotonic()
    last_sent = [read_brightness_percent(brightness_path, max_level)]
    yielded = False
    yield_lux = 0.0

    while running:
        time.sleep(POLL_INTERVAL)
        if not running:
            break

        now = time.monotonic()
        elapsed = now - last_sample
        last_sample = now

        lux = read_lux(sensor, scale)
        if lux is None:
            continue

        if smoothed is None or elapsed > SUSPEND_GAP:
            smoothed = lux
        else:
            alpha = 1.0 - math.exp(-elapsed / EMA_TIME_CONSTANT)
            smoothed += alpha * (lux - smoothed)

        observed = read_brightness_percent(brightness_path, max_level)
        if observed is not None:
            if elapsed > SUSPEND_GAP:
                # After a suspend/resume the backlight may have been restored
                # to a different value; rebaseline instead of yielding.
                last_sent[0] = int(round(observed))
            elif last_sent[0] is not None:
                if abs(observed - last_sent[0]) > MANUAL_TOLERANCE and not yielded:
                    yielded = True
                    yield_lux = smoothed
                    LOG.info(
                        "brightness changed by another process (%.1f%%), yielding",
                        observed,
                    )
            else:
                last_sent[0] = int(round(observed))

        if yielded:
            if abs(smoothed - yield_lux) > RECLAIM_LUX:
                yielded = False
                LOG.info("ambient light changed by more than %.0f lux, taking control back", RECLAIM_LUX)
            else:
                continue

        target_step = select_step(steps, current_step, smoothed)
        if target_step != current_step:
            LOG.info(
                "step %d -> %d (%.1f lux, ui %.2f%%)",
                current_step + 1,
                target_step + 1,
                smoothed,
                steps[target_step].ui_percent,
            )
            current_step = target_step

        target_percent = ui_to_hardware_percent(
            steps[current_step].brightness_percent, min_visible, max_level
        )
        if last_sent[0] is None or abs(target_percent - last_sent[0]) >= 1.0:
            start = last_sent[0] if last_sent[0] is not None else target_percent
            ramp(backlight, start, target_percent, last_sent)

    LOG.info("stopped")
    return 0


if __name__ == "__main__":
    sys.exit(main())
