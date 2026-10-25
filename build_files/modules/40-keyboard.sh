#!/bin/bash
# Keyboard configuration for Pixelbook (eve)
#
# Uses keyd to emulate the ChromeOS key layout. Combos are derived from the
# official Google ChromeOS shortcut docs (support.google.com/chromebook/answer/
# 183101 and /1047364); GNOME default bindings are checked against the upstream
# gsettings-desktop-schemas so keyd output avoids clashing with them. The final
# mapping table is documented in README.md (Keyboard layout).
#
# The keymap data lives in dedicated files under system_files/ and is installed
# by the `cp -a /ctx/system_files` in 00-system-files.sh:
#   system_files/etc/keyd/cros.conf                    -> /etc/keyd/cros.conf
#   system_files/usr/lib/udev/hwdb.d/70-pixel-keyboard.hwdb -> Three-lines key -> dashboard, Assistant key -> F23
# Edit those files directly; validate with `sudo keyd check` on a device.

set -ouex pipefail

# Compile the hwdb sources into the binary database udev reads at runtime
# (hwdb(7): only the compiled database is used). At build time /usr/lib is
# still writable, so ship the database inside the image like the immutable-image
# setup systemd documents; nothing else in the build runs systemd-hwdb.
systemd-hwdb update --usr

# libinput quirks for keyd virtual keyboard
mkdir -p /etc/libinput
cat > /etc/libinput/local-overrides.quirks << 'EOF'
[keyd virtual keyboard]
MatchName=keyd virtual keyboard
AttrKeyboardIntegration=internal
ModelTabletModeNoSuspend=1
EOF
