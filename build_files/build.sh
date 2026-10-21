#!/bin/bash

set -ouex pipefail

# Copy the contents of system_files/ of the git repo to /
cp -avf "/ctx/system_files"/. /

### Install packages

# Packages can be installed from any enabled yum repo on the image.
# RPMfusion repos are available by default in ublue main images

# Install base packages from fedora repos
dnf5 install -y git git-credential-oauth

# D-Bus bindings and GSettings (PyGObject) used by the ALS auto-brightness
# daemon (system_files/usr/bin/als-brightness-daemon.py): logind for the
# backlight, Gio.Settings for the ambient-enabled toggle
dnf5 install -y python3-dbus python3-gobject-base

# Add Terra repo (for keyd package) and install keyd
# Pixelbook (eve) keyboard mapping requires keyd
dnf5 install -y --nogpgcheck --repofrompath 'terra,https://repos.fyralabs.com/terra$releasever' terra-release
dnf5 install -y keyd

### Flatpak repositories
#
# Upstream (fedora-silverblue) ships flatpak-add-fedora-repos.service, enabled
# via a systemd preset. On first boot it registers the Fedora flatpak registry
# (oci+https://registry.fedoraproject.org) plus a disabled fedora-testing
# remote. Mask the service so those are never added. The Flathub remote
# shipped at system_files/etc/flatpak/remotes.d/flathub.flatpakrepo (copied to
# /etc/flatpak/remotes.d/ at the top of this script) is registered by
# flatpak-system-helper instead.
# Note: this only affects fresh deployments of this image; an existing install
# that already added the Fedora remotes keeps them under /var/lib/flatpak.

ln -s /dev/null /etc/systemd/system/flatpak-add-fedora-repos.service

### Remove Firefox
#
# Upstream ships Firefox as an RPM (firefox + firefox-langpacks) baked into
# the base ostree commit and installed into read-only /usr, where it cannot
# be removed at runtime. Drop both packages from the image. Firefox stays
# available to users via Flathub (flatpak install flathub org.mozilla.firefox)
# or rpm-ostree/bootc layering.

dnf5 remove -y firefox firefox-langpacks

### Audio configuration for Pixelbook (eve)
#
# Pixelbook (eve) uses Intel Kaby Lake with the AVS audio driver.
# The speaker amplifiers are MAX98927 (smart amps with built-in
# speaker protection via DSM), so no speaker disabling is needed.
# Source: https://github.com/WeirdTreeThing/chromebook-linux-audio

mkdir -p /etc/modprobe.d

# Enable AVS driver for Kaby Lake
cat > /etc/modprobe.d/snd-avs.conf << 'EOF'
options snd-intel-dspcfg dsp_driver=4
options snd-soc-avs ignore_fw_version=1
options snd-soc-avs obsolete_card_names=1
EOF

# WirePlumber configuration
mkdir -p /etc/wireplumber/wireplumber.conf.d

cat > /etc/wireplumber/wireplumber.conf.d/51-disable-pro-audio.conf << 'EOF'
monitor.alsa.rules = [
  {
    matches = [
      {
        api.alsa.card.name = "~(sof|avs|acp).*"
      }
    ]
    actions = {
      update-props = {
        api.acp.disable-pro-audio = true
      }
    }
  }
]
EOF

cat > /etc/wireplumber/wireplumber.conf.d/51-increase-headroom.conf << 'EOF'
monitor.alsa.rules = [
  {
    matches = [
      {
        node.name = "~alsa_output.*"
      }
    ]
    actions = {
      update-props = {
        api.alsa.headroom = 2048
      }
    }
  }
]
EOF

# Install Chromebook UCM configurations
# /tmp is mounted as tmpfs during build, so the cloned repo
# will not persist in the final image.
git clone --depth 1 https://github.com/WeirdTreeThing/alsa-ucm-conf-cros /tmp/alsa-ucm-conf-cros
cp -r /tmp/alsa-ucm-conf-cros/ucm2/* /usr/share/alsa/ucm2/
cp -r /tmp/alsa-ucm-conf-cros/overrides/* /usr/share/alsa/ucm2/conf.d/

### Keyboard configuration for Pixelbook (eve)
#
# Uses keyd to emulate the ChromeOS key layout. Combos are derived from the
# official Google ChromeOS shortcut docs (support.google.com/chromebook/answer/
# 183101 and /1047364); GNOME default bindings are checked against the upstream
# gsettings-desktop-schemas so keyd output avoids clashing with them. The final
# mapping table is documented in README.md (Keyboard layout).

mkdir -p /etc/keyd

cat > /etc/keyd/cros.conf << 'EOF'
[ids]
# Pixelbook and Pixelbook Go use AT Keyboard (0001:0001)
0001:0001

# NOTE: keyd only honours full-line comments (first non-space character is '#').
# Do not append trailing comments to bindings; keyd parses them as part of the
# value and silently drops the binding (keyd check then fails).

[main]
# The eve top row is assumed to emit media codes (back, refresh, zoom, scale,
# brightnessdown/brightnessup, playpause, mute, volumedown/volumeup, sleep),
# matching the pre-existing config; confirm with `keyd monitor` (README caveat).
# Only these three keys are remapped; the rest pass through as-is.
# Fullscreen: bare KEY_ZOOM does nothing in GNOME, so map it to F11.
zoom = f11
# Overview key -> screenshot UI (GNOME hangs the screenshot UI off Print).
scale = print
# Three-lines/power key -> Quick Settings (GNOME 45+ native Super+S).
sleep = M-s
# Modifier->layer bindings are implicit in keyd; declared here for clarity.
meta = layer(meta)
control = layer(control)
leftalt = layer(alt)
# Right Alt behaves as plain Alt, as on ChromeOS (default is layer(altgr)).
# Drop the next line if a real device shows right Alt does not trigger Alt combos.
rightalt = layer(alt)
shift = layer(shift)

[meta]
# Hold Search (Meta): top row = F1-F10 (official), plus ChromeOS navigation.
# Bindings here drop Super, so outputs are plain keys or explicit-modifier
# macros (M- = Super, C- = Ctrl, S- = Shift, A- = Alt).
back = f1
refresh = f2
zoom = f3
scale = f4
brightnessdown = f5
brightnessup = f6
playpause = f7
mute = f8
volumedown = f9
volumeup = f10
# Search + three-lines -> F12.
sleep = f12
# Search + Alt -> Caps Lock (either order; the [alt] layer adds the other).
leftalt = capslock
# Search + Left/Right -> Home/End (bare, so it is not read as a Super combo).
left = home
right = end
# Search + [ / ] -> previous / next workspace (GNOME native Super+Page_Up/Down).
leftbrace = M-pageup
rightbrace = M-pagedown

[alt]
# Alt combos; output drops Alt (ChromeOS semantics).
# Alt + Search -> Caps Lock.
leftmeta = capslock
# Alt + Backspace -> forward Delete.
backspace = delete
# Alt + Up/Down -> Page Up/Down (no GNOME system binding; see README caveat).
up = pageup
down = pagedown
brightnessdown = kbdillumdown
brightnessup = kbdillumup

# Composite layers must be declared after their constituent layers.
[shift+meta]
# Search + Shift + Left/Right -> select to line start / end.
left = S-home
right = S-end
# Search + Shift + [ / ] -> move window to previous / next workspace.
leftbrace = M-S-pageup
rightbrace = M-S-pagedown

[control+meta]
# Search + Ctrl + Left/Right -> start / end of document.
left = C-home
right = C-end

[control+alt]
# Ctrl + Alt + Backspace -> Ctrl + Alt + Delete.
backspace = C-A-delete
EOF

# udev hwdb: map the Pixelbook Assistant key (AT scancode 0xd8, confirmed via
# evtest MSC_SCAN) to F13; bind it to any custom shortcut in
# Settings -> Keyboard -> Custom Shortcuts. The Search/Launcher key is natively
# leftmeta and needs no remapping.
mkdir -p /etc/udev/hwdb.d
cat > /etc/udev/hwdb.d/61-pixel-keyboard.hwdb << 'EOF'
evdev:atkbd:dmi:bvn*:bvr*:bd*:svnGoogle:pn*:pvr*
 KEYBOARD_KEY_d8=f13
EOF

# libinput quirks for keyd virtual keyboard
mkdir -p /etc/libinput
cat > /etc/libinput/local-overrides.quirks << 'EOF'
[keyd virtual keyboard]
MatchName=keyd virtual keyboard
AttrKeyboardIntegration=internal
ModelTabletModeNoSuspend=1
EOF


#### Enable system services
systemctl enable keyd
systemctl enable podman.socket

#### Enable user services
systemctl --global enable wacom-stylus-keyfile.timer
systemctl --global enable wacom-stylus-keyfile.service

# The ALS daemon is a per-user resident service; it reads
# org.gnome.settings-daemon.plugins.power ambient-enabled inline and pauses
# when the toggle is off. Enabling it globally means it starts for every
# login session, whether the toggle is on or off — daemon checks gsettings
# each cycle and no-ops until the user turns it on.
systemctl --global enable als-brightness.service


# Clean up dnf cache to reduce image size
dnf5 clean all
