#!/bin/bash
# Repositories and base packages.

set -ouex pipefail

# Packages can be installed from any enabled yum repo on the image.
# This image builds from the vanilla fedora-silverblue base (not a ublue main
# image), so RPM Fusion repos are NOT pre-enabled; add them below.

# Install base packages from fedora repos
dnf5 install -y git git-credential-oauth

# Add RPM Fusion repos (free for ffmpeg/codecs/mesa-freeworld, nonfree for
# the full intel-media-driver). The release packages are not in the Fedora
# repos, so install them from the official RPM Fusion URLs, as documented on
# rpmfusion.org.
dnf5 install -y \
  "https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm" \
  "https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm"
# D-Bus bindings and GSettings (PyGObject) used by the ALS auto-brightness
# daemon (system_files/usr/bin/als-brightness-daemon.py): logind for the
# backlight, Gio.Settings for the ambient-enabled toggle
dnf5 install -y python3-dbus python3-gobject-base

# Add Terra repo (for keyd package) and install keyd
# Pixelbook (eve) keyboard mapping requires keyd
dnf5 install -y --nogpgcheck --repofrompath 'terra,https://repos.fyralabs.com/terra$releasever' terra-release
dnf5 install -y keyd
