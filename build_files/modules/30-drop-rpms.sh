#!/bin/bash
# Unwanted base RPMs: Firefox, GNOME System Monitor, GNOME Software

set -ouex pipefail

# Upstream ships these RPMs baked into the base ostree commit and installed into
# read-only /usr, where they cannot be removed at runtime. Drop them from the
# image. Firefox stays available to users via Flathub (flatpak install flathub
# org.mozilla.firefox); the OS itself moves as whole bootc images (staged by
# uupd, see 12-uupd.sh), not per-package layering.

dnf5 remove -y firefox firefox-langpacks
dnf5 remove -y gnome-shell-extension-background-logo
dnf5 remove -y gnome-system-monitor
