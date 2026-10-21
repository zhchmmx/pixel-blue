#!/bin/bash
# Ambient brightness master switch: drive als-brightness.service from the GNOME
# "Automatic Brightness" toggle (org.gnome.settings-daemon.plugins.power
# ambient-enabled).
#
#   toggle ON  -> start als-brightness.service (vendor ALS steps + curve)
#   toggle OFF -> stop als-brightness.service  (manual brightness control)
#
# The ALS stays published by iio-sensor-proxy (so GNOME shows the toggle), and
# /etc/dbus-1/system.d/50-pixel-blue-als.conf stops gnome-settings-daemon from
# claiming it, so it never competes with the daemon for the backlight.
# Enabled for all users by default; see README.md for the disable commands.

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
