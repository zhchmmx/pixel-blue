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

# The ALS daemon is a per-user resident service; it reads
# org.gnome.settings-daemon.plugins.power ambient-enabled inline and pauses
# when the toggle is off. Enabling it globally means it starts for every
# login session, whether the toggle is on or off — daemon checks gsettings
# each cycle and no-ops until the user turns it on.
systemctl --global enable als-brightness.service


### Branding
#
# Replace the inherited Fedora logo (issue #54). The logo master lives at
# system_files/usr/share/pixel-blue/logos/ and the same artwork is installed
# as a hicolor scalable icon (pixel-blue-logo-icon.svg) by the cp at the top
# of this script. Point os-release's LOGO= at it so GNOME surfaces that
# render the distribution logo (Settings -> About, Overview, GNOME Software)
# pick up Pixel Blue artwork instead of the Fedora ring. Only the LOGO key
# is rewritten: NAME/PRETTY_NAME keep the Fedora strings and ID=fedora and
# the VARIANT* keys stay untouched (Flatpak / GNOME Software detection and
# the Anaconda Silverblue ISO profile depend on them).

if grep -q '^LOGO=' /usr/lib/os-release; then
  sed -i 's|^LOGO=.*|LOGO=pixel-blue-logo-icon|' /usr/lib/os-release
else
  echo 'LOGO=pixel-blue-logo-icon' >> /usr/lib/os-release
fi

# Boot splash (phase 2): plymouth renders the system default theme's
# watermark from the initramfs, and it only reads PNGs. Fedora's default
# theme is "spinner" and its only distro artwork is watermark.png, so we
# overlay a white mono PNG export of the symbol master there instead of
# adding a custom theme (no plymouth-set-default-theme change needed).
# Re-apply after the dnf5 transactions in case a package refresh restored
# the Fedora copies of these paths.
cp -avf "/ctx/system_files/usr/share/plymouth/." /usr/share/plymouth/
cp -avf "/ctx/system_files/usr/share/icons/hicolor/." /usr/share/icons/hicolor/
cp -avf "/ctx/system_files/usr/share/pixmaps/." /usr/share/pixmaps/

# Files copied outside the rpm transaction are invisible to the packaged
# icon-theme.cache, which would keep LOGO= resolving to the old entries;
# rebuild it (gtk-update-icon-cache is a hard dep of gtk3 on the base).
gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor

# bootc ships the image's initramfs verbatim at deploy time (nothing is
# regenerated on the target), so re-run dracut here to bake the overlaid
# spinner/watermark.png into the boot splash (per
# docs.fedoraproject.org/en-US/bootc/initramfs/). --no-hostonly keeps the
# initrd generic (hardware detection inside the build container would
# otherwise bake in the CI runner's devices).
dracut --no-hostonly --regenerate-all --force
# Assert it landed. dracut's plymouth module bakes in only the *selected*
# default theme, so this also proves spinner is that theme; if either ever
# regresses, the build fails loudly instead of booting a Fedora splash.
splash_baked=0
for img in /usr/lib/modules/*/initramfs.img /usr/lib/modules/*/images/pxeboot/initramfs.img; do
  [ -f "$img" ] || continue
  # Capture before matching: grep -q closing the pipe early would SIGPIPE
  # lsinitrd and pipefail would report failure for a perfectly good initrd.
  if lsinitrd "$img" > /tmp/lsinitrd.list 2>/dev/null &&
     grep -q 'themes/spinner/watermark.png' /tmp/lsinitrd.list; then
    splash_baked=1
  fi
done
if [ "$splash_baked" -ne 1 ]; then
  echo "error: pixel-blue plymouth watermark not found in any regenerated initramfs" >&2
  exit 1
fi

# First-boot wizard: current gnome-initial-setup (GTK4 rewrite) ships no
# vendor logo path in its conf (README documents only the [pages] group:
# skip / new_user_only / existing_user_only), so there is nothing to wire
# here. The wizard's title string comes from PRETTY_NAME, which stays
# Fedora-branded until the NAME/PRETTY_NAME decision lands (see #54).

# Clean up dnf cache to reduce image size
dnf5 clean all
