#!/bin/bash
# Remove Firefox

set -ouex pipefail

# Upstream ships Firefox as an RPM (firefox + firefox-langpacks) baked into
# the base ostree commit and installed into read-only /usr, where it cannot
# be removed at runtime. Drop both packages from the image. Firefox stays
# available to users via Flathub (flatpak install flathub org.mozilla.firefox);
# runtime rpm-ostree layering is rejected on this image (LockLayering=true,
# see system_files/etc/rpm-ostreed.conf).

dnf5 remove -y firefox firefox-langpacks
dnf5 remove -y gnome-shell-extension-background-logo
