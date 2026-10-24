#!/bin/bash
# Copy the contents of system_files/ of the git repo to /.
#
# Must run first: the flatpak remote, keyd keymap, hwdb source, gschema
# override, systemd units and the ALS daemon all land here, and later
# modules (package installs, schema compilation, initramfs) build on them.

set -ouex pipefail

cp -avf "/ctx/system_files"/. /
