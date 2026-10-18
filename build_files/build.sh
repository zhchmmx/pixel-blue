#!/bin/bash

set -ouex pipefail

# Copy the contents of system_files/ of the git repo to /
cp -avf "/ctx/system_files"/. /

### Install packages

# Packages can be installed from any enabled yum repo on the image.
# RPMfusion repos are available by default in ublue main images

# Install base packages from fedora repos
dnf5 install -y git git-credential-oauth

# Add Terra repo (for keyd package) and install keyd
# Pixelbook (eve) keyboard mapping requires keyd
dnf5 install -y --nogpgcheck --repofrompath 'terra,https://repos.fyralabs.com/terra$releasever' terra-release
dnf5 install -y keyd

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
# Uses keyd to remap the top-row function keys.
# Source: https://github.com/WeirdTreeThing/cros-keyboard-map

mkdir -p /etc/keyd

cat > /etc/keyd/cros.conf << 'EOF'
[ids]
# Pixelbook and Pixelbook Go use AT Keyboard (0001:0001)
0001:0001

[main]
# Pixelbook (eve) top-row keys natively emit media keys, not F1-F10:
#   esc, back, refresh, zoom, scale, brightnessdown, brightnessup,
#   playpause, mute, volumedown, volumeup, sleep
# No remapping is needed in the main layer; they pass through as-is.
#
# keyd auto-binds modifier keys to their eponymous layers, but the
# bindings are declared explicitly here for clarity.
zoom = f11
sleep = f12
meta = layer(meta)
control = layer(control)
leftalt = layer(alt)
rightalt = layer(altgr)
shift = layer(shift)

[meta]
# Hold Meta (Search) to use the top-row keys as F1-F10.
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
sleep = f13

[alt]
backspace = delete
brightnessdown = kbdillumdown
brightnessup = kbdillumup

[control]
brightnessdown = print

[control+alt]
backspace = C-A-delete
EOF

# udev hwdb: remap the top-row key (scan code d8) to leftmeta (search key)
mkdir -p /etc/udev/hwdb.d
cat > /etc/udev/hwdb.d/61-pixel-keyboard.hwdb << 'EOF'
evdev:atkbd:dmi:bvn*:bvr*:bd*:svnGoogle:pn*:pvr*
 KEYBOARD_KEY_d8=leftmeta
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

# Clean up dnf cache to reduce image size
dnf5 clean all
