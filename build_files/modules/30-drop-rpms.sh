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

# Replace GNOME Software with the Bazaar flatpak (issue #87): refs in
# flatpak_refs/, dash favorite swapped in the gschema override.
# --no-autoremove is load-bearing: Fedora's gnome-software hard-Requires
# flatpak/flatpak-libs, epiphany-runtime and appstream-data, so autoremove
# could orphan-erase them and take Bazaar and every installed flatpak with it.
# fwupd is left in place (it is part of the base image); this image no longer
# uses it, because firmware updates go through the Pixel Blue D-Bus service.
dnf5 remove -y --no-autoremove gnome-software gnome-software-rpm-ostree
# The Firmware flatpak carries the same app id as the gnome-firmware RPM;
# remove the RPM when the base ships it, to avoid a duplicate icon if a user
# installs the flatpak from Flathub. (F44's silverblue base does not, hence
# the guard rather than a bare remove.)
if rpm -q gnome-firmware > /dev/null; then
  dnf5 remove -y --no-autoremove gnome-firmware
fi
