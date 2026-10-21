#!/bin/bash
# Ambient brightness master switch: drive als-brightness.service from the GNOME
# "Automatic Brightness" toggle (org.gnome.settings-daemon.plugins.power
# ambient-enabled).
#
#   toggle ON  -> start als-brightness.service (vendor ALS steps + curve)
#   toggle OFF -> stop als-brightness.service  (manual brightness control)
#
# 99-hide-als.rules hides the ALS from iio-sensor-proxy so that
# gnome-settings-daemon never competes with the daemon for the backlight.
# This watcher is not enabled by default; see README.md for the opt-in command.

set -u -o pipefail

readonly SERVICE="als-brightness.service"

while true; do
  state="$(gsettings get org.gnome.settings-daemon.plugins.power ambient-enabled 2>/dev/null || true)"
  if [[ "${state}" == "true" ]]; then
    systemctl --user start --quiet "${SERVICE}" 2>/dev/null || true
  else
    systemctl --user stop --quiet "${SERVICE}" 2>/dev/null || true
  fi
  sleep 3
done
