#!/bin/bash
# Enable user services

set -ouex pipefail

systemctl --global enable wacom-stylus-keyfile.timer
systemctl --global enable wacom-stylus-keyfile.service

# The ALS daemon is a per-user resident service; it reads
# org.gnome.settings-daemon.plugins.power ambient-enabled inline and pauses
# when the toggle is off. Enabling it globally means it starts for every
# login session, whether the toggle is on or off — daemon checks gsettings
# each cycle and no-ops until the user turns it on.
systemctl --global enable als-brightness.service

# Resident firmware notifier: watches the failure marker and check.json so a
# class-B flash failure keeps a shutdown/sleep inhibitor in place and the
# "open updater" notification works even when the updater window is closed.
systemctl --global enable pixel-blue-firmware-alert.service
