#!/bin/bash
# Pixel Blue branding RPM (pixel-blue-logos)
#
# The logo payload lives in the pixel-blue-logos RPM (dist-git-style sibling
# repo zhchmmx/pixel-blue-logos, generic-logos pattern: Provides: system-logos,
# Conflicts: fedora-logos), published through Copr.

set -ouex pipefail

# `dnf5 copr enable` is a dnf5-plugins subcommand, so install the plugin first
# (idempotent if the base already ships it). The enable command fetches the
# project metadata from the hub at enable time and writes
# /etc/yum.repos.d/_copr:copr.fedorainfracloud.org:zhchmmx:pixel-blue-logos.repo
# -- which also means a missing Copr project (or one without the
# fedora-$releasever-$basearch chroot matching this base) fails HERE, loudly.
# -y is load-bearing: without it the plugin's third-party-repo confirmation
# reads empty stdin in the build container and silently exits 0 without
# writing the repo file. Like the RPM Fusion/Terra repos in 05-repos.sh, the
# repo file is baked into /etc. Hosts never consult it at runtime though:
# bootc upgrades (staged daily by uupd, see 12-uupd.sh) pull OCI images,
# never packages, so the image's own content is not updateable from Copr.
dnf5 install -y dnf5-plugins
dnf5 copr enable -y zhchmmx/pixel-blue-logos
# A plain install would fail on the package's Conflicts: fedora-logos (the
# base image ships fedora-logos). swap erases and installs in one transaction;
# the package's Provides: system-logos keeps every dependency on the virtual
# name satisfied (gdm, plymouth-graphics-libs and the background-logo
# extension all require system-logos, verified against F44 repodata).
# Deliberately NO --allowerasing here: it would turn a mis-packaged Provides
# into a silent erase of gdm/plymouth with a green build -- the solver must
# fail loudly instead.
dnf5 swap -y fedora-logos pixel-blue-logos

# Drop the base image's compose-time /usr/share/icons/hicolor/icon-theme.cache.
# ostree composes generate that file, but no Fedora package maintains it
# anymore (the icon-cache scriptlets are long gone), so nothing regenerates it
# when the swap above installs pixel-blue-logos' icons. GTK treats the cache as
# authoritative unless it is older than the theme's *root* directory
# (gtk_icon_cache_new_for_path: "if (st.st_mtime < path_st.st_mtime) icon cache
# outdated"), and installing files into subdirs like 96x96/apps/ never touches
# the root dir's mtime -- so with the stale cache present GTK reads only the
# cache and every icon name added after the base compose is invisible. That is
# the first-boot bug: gnome-initial-setup's language page sets its logo
# GtkImage to the *icon name* from os-release LOGO=pixel-blue-logo-icon
# (60-branding-dracut.sh), the name is missing from the stale cache, and GTK
# renders image-missing. The live ISO does not show it because iso/build.sh
# refreshes the cache for the live session; the installed deployment is built
# from these modules, which never did.
# Delete rather than regenerate, deliberately: modules that run after this one
# still install icons (e.g. 20-waydroid), so a cache baked here would hide
# those the exact same way, and regenerating would need gtk-update-icon-cache
# in the build environment (iso/build.sh guards it with `command -v` precisely
# because its presence is not guaranteed). With no cache GTK falls back to
# scanning the directories -- always current, and the same behavior as
# scriptlet-free dnf-based Fedora systems.
rm -f /usr/share/icons/hicolor/icon-theme.cache
