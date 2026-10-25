#!/bin/bash
# Enable system services

set -ouex pipefail

systemctl enable keyd
systemctl enable podman.socket
# The uupd RPM has no systemd preset, so its timer ships disabled and must be
# enabled here for the daily bootc update check (see 12-uupd.sh).
systemctl enable uupd.timer
