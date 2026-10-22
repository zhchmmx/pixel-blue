#!/usr/bin/env bash
#
# pixel-blue-flash-alert.sh - user-domain notifier for a failed firmware
# flash (Pixel Blue, Issue #69).
#
# The fwupd plugin runs as root in the system domain and cannot reach the
# user session bus; it leaves a marker at
# /var/lib/pixel-blue/fwupd/flash-failed (class=A|B, attempts=N).  This
# script is started by the user-level pixel-blue-flash-alert.path unit the
# moment the marker appears (and on next login if it already exists), and
# sends a critical desktop notification every five minutes until the marker
# is removed by a successful retry.  Deliberately repetitive: missing the
# warning can mean a bricked machine.
set -euo pipefail

MARKER="${MRCHROMEBOX_STATE_DIR:-/var/lib/pixel-blue/fwupd}/flash-failed"
INTERVAL="${MRCHROMEBOX_ALERT_INTERVAL:-300}"

notify() {
  local class attempts title body

  class=$(sed -n 's/^class=\(.*\)$/\1/p' "$MARKER" 2>/dev/null | head -n 1)
  attempts=$(sed -n 's/^attempts=\([0-9]*\)$/\1/p' "$MARKER" 2>/dev/null | head -n 1)
  attempts=${attempts:-1}

  title="⚠️ 固件更新失败——千万不要重启电脑"
  body="回到 GNOME 软件「更新」页再点一次「更新」重试；请保持开机，不要关机或重启（已失败 ${attempts} 次）。"
  if [[ "$class" == "B" ]]; then
    body="${body} 固件可能不完整，此时重启将无法开机。"
  fi
  if ((attempts >= 3)); then
    body="${body} 连续失败请保持开机，另开终端运行 sudo firmware-util.sh 排查——仍然不要重启。"
  fi

  # GVariant text format requires quoted strings; pass each string with its
  # own quotes so arbitrary Unicode is safe.  urgency=2 (critical).
  gdbus call --session \
    --dest org.freedesktop.Notifications \
    --object-path /org/freedesktop/Notifications \
    --method org.freedesktop.Notifications.Notify \
    "pixel-blue-flash-alert" 0 "dialog-error" \
    "\"${title}\"" "\"${body}\"" "[]" '{"urgency": <2>}' 0 \
    >/dev/null 2>&1 || true
}

while [[ -e "$MARKER" ]]; do
  notify
  sleep "$INTERVAL"
done
