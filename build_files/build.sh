#!/bin/bash
#
# Build driver. The actual work lives in build_files/modules/, executed in
# filename order (the zero-padded numeric prefix is the order).
#
# Ordering is load-bearing:
#   00-system-files.sh     copy system_files/ to / (must run first)
#   05..30                 repositories, package installs/swaps, removals
#   35..55                 hardware config, services, schema compilation
#   60-branding-dracut.sh  LOGO= rewrite + initramfs regeneration (must run
#                          after the pixel-blue-logos swap in 10-logos.sh)
#   85-cleanup.sh          trim the dnf cache
#   90-os-name.sh          os-release NAME/PRETTY_NAME, last so no dnf5
#                          operation can restore the stock file
#
# Modules are standalone bash scripts, each with its own `set -ouex pipefail`,
# so they can also be run individually while debugging. The Containerfile
# keeps the whole build in a single RUN layer: the modules share the same
# container filesystem and dnf cache, and splitting into more RUN steps would
# only add layers.

set -ouex pipefail

for module in /ctx/modules/*.sh; do
  echo "=== running $(basename "$module")"
  bash "$module"
done
