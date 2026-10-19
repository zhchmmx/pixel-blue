#!/bin/bash
# Ambient brightness master switch: drive wluma from the GNOME
# "Automatic Brightness" toggle (org.gnome.settings-daemon.plugins.power
# ambient-enabled).
#
#   toggle ON  -> start wluma (smooth ambient-based auto brightness)
#   toggle OFF -> stop wluma  (manual brightness control)
#
# The udev rule 99-hide-als.rules hides the ALS from iio-sensor-proxy so that
# gnome-settings-daemon never competes with wluma for backlight control.

set -u

while true; do
  state="$(gsettings get org.gnome.settings-daemon.plugins.power ambient-enabled 2>/dev/null)"
  if [ "$state" = "true" ]; then
    systemctl --user start wluma 2>/dev/null
  else
    systemctl --user stop wluma 2>/dev/null
  fi
  sleep 3
done
