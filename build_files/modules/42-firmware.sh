#!/bin/bash
# Firmware updater.
#
# Updates are handled by the fwupd plugin built from build_files/fwupd-plugin:
# it exposes the MrChromebox UEFI Full ROM as a fwupd device and drives the
# vendored MrChromebox helpers. flashrom is the image's own package: updates
# never download a programmer binary at runtime.
#
# Fedora's fwupd-devel ships fwupd.h/libfwupd only, not the libfwupdplugin
# headers a plugin compiles against, so vendor/fwupd carries the pinned
# headers listed in vendor/fwupd/VERSION. The plugin is installed into the
# versioned plugin directory fwupd loads from and the build toolchain is
# removed again afterwards.
#
# vendor/mrchromebox is verbatim upstream at the commit recorded in
# VENDOR_COMMIT (CI re-checks the manifest and the function names). Only the
# scripts needed at runtime are installed:
#   functions.sh, sources.sh       sourced by the update helper
#   download-fullrom.sh            manual/rescue tool, never called by timers

set -ouex pipefail

dnf5 install -y flashrom curl gcab

dnf5 install -y gcc glib2-devel libxmlb-devel fwupd-devel

fwupd_plugindir=""
for candidate in /usr/lib64/fwupd-*; do
  if [ -f "${candidate}/libfwupdplugin.so" ]; then
    fwupd_plugindir="${candidate}"
    break
  fi
done
if [ -z "${fwupd_plugindir}" ]; then
  echo "42-firmware: no fwupd plugin directory found" >&2
  exit 1
fi

read -r -a fwupd_cflags <<<"$(pkg-config --cflags glib-2.0 gobject-2.0 gio-2.0 xmlb)"
read -r -a fwupd_libs <<<"$(pkg-config --libs glib-2.0 gobject-2.0 gio-2.0 xmlb)"
read -r -a fwupd_link <<<"$(pkg-config --libs fwupd)"

gcc -O2 -fPIC -std=c11 -Wall -Wextra -Wno-unused-parameter -Wl,--no-undefined \
  -I/ctx/vendor/fwupd/include \
  -I/ctx/vendor/fwupd/include/libfwupd \
  -I/ctx/vendor/fwupd/include/libfwupdplugin \
  -I/usr/include/fwupd-3 -I/usr/include/fwupd-3/libfwupd \
  "${fwupd_cflags[@]}" \
  -shared -o "${fwupd_plugindir}/libfu_plugin_mrchromebox.so" \
  /ctx/fwupd-plugin/fu-mrchromebox-plugin.c \
  /ctx/fwupd-plugin/fu-mrchromebox-device.c \
  -L"${fwupd_plugindir}" -lfwupdplugin \
  "${fwupd_link[@]}" \
  -Wl,-rpath,"${fwupd_plugindir}" \
  "${fwupd_libs[@]}"

dnf5 remove -y --no-autoremove gcc glib2-devel libxmlb-devel fwupd-devel

install -d -m 0755 /usr/lib/pixel-blue/mrchromebox
install -m 0644 /ctx/vendor/mrchromebox/functions.sh /usr/lib/pixel-blue/mrchromebox/functions.sh
install -m 0644 /ctx/vendor/mrchromebox/sources.sh /usr/lib/pixel-blue/mrchromebox/sources.sh
install -m 0755 /ctx/vendor/mrchromebox/download-fullrom.sh /usr/lib/pixel-blue/mrchromebox/download-fullrom.sh
install -m 0644 /ctx/vendor/mrchromebox/VENDOR_COMMIT /usr/lib/pixel-blue/mrchromebox/VENDOR_COMMIT
