#!/bin/bash
# Remove Firefox

set -ouex pipefail

# Upstream ships Firefox as an RPM (firefox + firefox-langpacks) baked into
# the base ostree commit and installed into read-only /usr, where it cannot
# be removed at runtime. Drop both packages from the image. Firefox stays
# available to users via Flathub (flatpak install flathub org.mozilla.firefox);
# the OS itself moves as whole bootc images (staged by uupd, see 12-uupd.sh),
# not per-package layering.

dnf5 remove -y firefox firefox-langpacks
dnf5 remove -y gnome-shell-extension-background-logo
