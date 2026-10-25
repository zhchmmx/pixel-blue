#!/bin/bash
# Firmware updater: small GTK4 frontend, D-Bus activated root daemon and the
# vendored MrChromebox scripts the daemon orchestrates.
#
# flashrom is the image's own package: updates never download a programmer
# binary at runtime. python3-gobject supplies the Gio/GLib bindings for both
# the daemon and the GUI; gtk4/libadwaita are explicitly listed so the Gtk and
# Adw typelibs exist even if a future base image trims them from the GNOME
# desktop packages.
#
# vendor/mrchromebox is verbatim upstream at the commit recorded in
# VENDOR_COMMIT (CI re-checks the manifest and the function names). Only the
# scripts needed at runtime are installed:
#   functions.sh, sources.sh       sourced by the daemon and weekly check
#   download-fullrom.sh            manual/rescue tool, never called by timers

set -ouex pipefail

dnf5 install -y flashrom curl python3-gobject gtk4 libadwaita

install -d -m 0755 /usr/lib/pixel-blue/mrchromebox
install -m 0644 /ctx/vendor/mrchromebox/functions.sh /usr/lib/pixel-blue/mrchromebox/functions.sh
install -m 0644 /ctx/vendor/mrchromebox/sources.sh /usr/lib/pixel-blue/mrchromebox/sources.sh
install -m 0755 /ctx/vendor/mrchromebox/download-fullrom.sh /usr/lib/pixel-blue/mrchromebox/download-fullrom.sh
install -m 0644 /ctx/vendor/mrchromebox/VENDOR_COMMIT /usr/lib/pixel-blue/mrchromebox/VENDOR_COMMIT
