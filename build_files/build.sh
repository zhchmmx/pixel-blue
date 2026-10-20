#!/bin/bash

set -ouex pipefail

# Copy the contents of system_files/ of the git repo to /
cp -avf "/ctx/system_files"/. /

### Install packages

# Packages can be installed from any enabled yum repo on the image.
# This image builds from the vanilla fedora-silverblue base (not a ublue main
# image), so RPM Fusion repos are NOT pre-enabled; add them below.

# Install base packages from fedora repos
dnf5 install -y git git-credential-oauth

# Add RPM Fusion repos (free for ffmpeg/codecs/mesa-freeworld, nonfree for
# the full intel-media-driver). The release packages are not in the Fedora
# repos, so install them from the official RPM Fusion URLs, as documented on
# rpmfusion.org.
dnf5 install -y \
  "https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm" \
  "https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm"

# Add Terra repo (for keyd package) and install keyd
# Pixelbook (eve) keyboard mapping requires keyd
dnf5 install -y --nogpgcheck --repofrompath 'terra,https://repos.fyralabs.com/terra$releasever' terra-release
dnf5 install -y keyd

### Multimedia codecs and hardware video acceleration
#
# Full ffmpeg replaces the stripped-down ffmpeg-free shipped by Fedora; the
# swap is the method recommended by RPM Fusion. The gstreamer1-plugins-*
# packages provide H.264/H.265 and other restricted codecs to GTK/GStreamer
# based apps, including the GNOME video decoders. gstreamer1-vaapi wires
# those decoders into VA-API.
dnf5 swap -y ffmpeg-free ffmpeg --enablerepo=rpmfusion-free
dnf5 install -y \
  gstreamer1-plugins-good \
  gstreamer1-plugins-bad-free \
  gstreamer1-plugins-bad-freeworld \
  gstreamer1-plugins-ugly-free \
  gstreamer1-plugins-ugly \
  gstreamer1-plugin-libav \
  gstreamer1-vaapi

# Intel VA-API user-space driver for the Pixelbook (eve) Kaby Lake iGPU.
# intel-media-driver (iHD) is the actively maintained driver and provides the
# full decode/encode set: H.264, HEVC 8/10-bit and VP9 8/10-bit. Fedora's
# libva-intel-media-driver build (a mandatory @multimedia package in the base
# image) has the patented codecs stripped, so swap it for the complete one
# from RPM Fusion nonfree (fedy and other distro tools use the same swap).
# libva-utils ships vainfo for verification. mesa-va-drivers-freeworld is the
# RPM Fusion rebuild of Mesa's VA-API driver with the full set of codecs
# (notably HEVC/H.265) enabled, so swap it in for complete hardware decode.
dnf5 swap -y libva-intel-media-driver intel-media-driver --enablerepo=rpmfusion-nonfree
dnf5 install -y libva-utils
dnf5 swap -y mesa-va-drivers mesa-va-drivers-freeworld --enablerepo=rpmfusion-free

### Waydroid (Android in a container)
#
# waydroid and waydroid-selinux are in the Fedora repos. System images are not
# pre-downloaded; on first boot run `sudo waydroid init` to fetch them (an
# OTA-style download of the LineageOS-based Android image).
dnf5 install -y waydroid waydroid-selinux

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

#### Enable user services
systemctl --global enable wacom-stylus-keyfile.timer
systemctl --global enable wacom-stylus-keyfile.service


# Clean up dnf cache to reduce image size
dnf5 clean all
