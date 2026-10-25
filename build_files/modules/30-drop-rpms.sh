#!/bin/bash
# Unwanted base RPMs: Firefox, GNOME System Monitor, GNOME Software

set -ouex pipefail


dnf5 remove -y firefox firefox-langpacks
dnf5 remove -y gnome-shell-extension-background-logo
dnf5 remove -y gnome-system-monitor

# Replace GNOME Software with the Bazaar and Firmware flatpaks (issue #87):
# refs in flatpak_refs/, dash favorite swapped in the gschema override.
# --no-autoremove is load-bearing: Fedora's gnome-software hard-Requires
# fwupd, flatpak/flatpak-libs, epiphany-runtime and appstream-data, so
# autoremove could orphan-erase fwupd, which the Firmware flatpak (it talks
# to the host daemon over D-Bus) and the firmware checks depend on. The
# assert fails the build if that guarantee ever breaks.
dnf5 remove -y --no-autoremove gnome-software gnome-software-rpm-ostree
rpm -q fwupd
# The Firmware flatpak carries the same app id as the gnome-firmware RPM;
# remove the RPM when the base ships it, to avoid a duplicate icon. (F44's
# silverblue base does not, hence the guard rather than a bare remove.)
if rpm -q gnome-firmware > /dev/null; then
  dnf5 remove -y --no-autoremove gnome-firmware
fi
