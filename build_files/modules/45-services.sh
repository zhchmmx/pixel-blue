#!/bin/bash
# Enable system services

set -ouex pipefail

systemctl enable keyd
systemctl enable podman.socket
# The uupd RPM has no systemd preset, so its timer ships disabled and must be
# enabled here for the daily bootc update check (see 12-uupd.sh).
systemctl enable uupd.timer
# Weekly MrChromebox release check. It only downloads the ROM, wraps it in a
# fwupd cabinet and refreshes the directory remote; flashing is always started
# by the user through fwupd (fwupdmgr update).
systemctl enable firmware-update-check.timer
