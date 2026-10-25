#!/bin/bash
# Waydroid (Android in a container)

set -ouex pipefail

# waydroid and waydroid-selinux are in the Fedora repos. System images are not
# pre-downloaded; on first boot run `sudo waydroid init` to fetch them (an
# OTA-style download of the LineageOS-based Android image).
dnf5 install -y waydroid waydroid-selinux
