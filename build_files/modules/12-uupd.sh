#!/bin/bash
# Automatic system updates via uupd (issues #89, #90)

set -ouex pipefail

# uupd (https://github.com/ublue-os/uupd) is this image's system update
# mechanism, replacing the earlier rpm-ostreed auto-staging and
# PackageKit-bootc experiments (both reverted; do not reintroduce them).
# Its `system` module runs `bootc upgrade --check` and, when a new build of
# the tracked channel is published, `bootc upgrade` stages the new
# deployment. It never reboots on its own: the staged deployment applies on
# the user's next reboot. Only the `system` module stays enabled -- the
# flatpak/distrobox/brew switches are turned off in
# system_files/etc/uupd/config.json (flatpak updates belong to the software
# store, per issue #87).

# `dnf5 copr enable` is a dnf5-plugins subcommand. 10-logos.sh already
# installs it and runs before this module; repeat the install here so the
# module also works standalone (it is idempotent). -y is load-bearing:
# without it the plugin's third-party-repo confirmation reads empty stdin in
# the build container and silently exits 0 without writing the repo file.
# The ublue-os/packages COPR ships the uupd RPM (verified fedora-44+ x86_64
# repodata), so the chroot matches this fedora-silverblue base.
dnf5 install -y dnf5-plugins
dnf5 copr enable -y ublue-os/packages
# The spec has `Recommends: distrobox` and dnf5 installs weak dependencies by
# default, which would bake distrobox into the image -- suppress them.
# `bootc` and `flatpak` (the other Recommends) are already in the base, and
# `libnotify` (the notification lib) is a hard Requires, so nothing needed is
# dropped.
dnf5 install -y --setopt=install_weak_deps=False uupd
