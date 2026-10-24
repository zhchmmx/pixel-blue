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
# D-Bus bindings and GSettings (PyGObject) used by the ALS auto-brightness
# daemon (system_files/usr/bin/als-brightness-daemon.py): logind for the
# backlight, Gio.Settings for the ambient-enabled toggle
dnf5 install -y python3-dbus python3-gobject-base

# Add Terra repo (for keyd package) and install keyd
# Pixelbook (eve) keyboard mapping requires keyd
dnf5 install -y --nogpgcheck --repofrompath 'terra,https://repos.fyralabs.com/terra$releasever' terra-release
dnf5 install -y keyd
dnf5 install -y PackageKit-bootc

### Pixel Blue branding RPM (pixel-blue-logos)
#
# The logo payload lives in the pixel-blue-logos RPM (dist-git-style sibling
# repo zhchmmx/pixel-blue-logos, generic-logos pattern: Provides: system-logos,
# Conflicts: fedora-logos), published through Copr.
#
# `dnf5 copr enable` is a dnf5-plugins subcommand, so install the plugin first
# (idempotent if the base already ships it). The enable command fetches the
# project metadata from the hub at enable time and writes
# /etc/yum.repos.d/_copr:copr.fedorainfracloud.org:zhchmmx:pixel-blue-logos.repo
# -- which also means a missing Copr project (or one without the
# fedora-$releasever-$basearch chroot matching this base) fails HERE, loudly.
# -y is load-bearing: without it the plugin's third-party-repo confirmation
# reads empty stdin in the build container and silently exits 0 without
# writing the repo file. Like the RPM Fusion/Terra repos above, the repo file
# is baked into /etc and stays live on deployed hosts for runtime layering
# (bootc upgrades pull OCI images, never packages, so the image's own content
# is not updateable from Copr).
dnf5 install -y dnf5-plugins
dnf5 copr enable -y zhchmmx/pixel-blue-logos
# A plain install would fail on the package's Conflicts: fedora-logos (the
# base image ships fedora-logos). swap erases and installs in one transaction;
# the package's Provides: system-logos keeps every dependency on the virtual
# name satisfied (gdm, plymouth-graphics-libs and the background-logo
# extension all require system-logos, verified against F44 repodata).
# Deliberately NO --allowerasing here: it would turn a mis-packaged Provides
# into a silent erase of gdm/plymouth with a green build -- the solver must
# fail loudly instead.
dnf5 swap -y fedora-logos pixel-blue-logos

### Multimedia codecs and hardware video acceleration
#
# Full ffmpeg replaces the stripped-down ffmpeg-free shipped by Fedora; the
# swap is the method recommended by RPM Fusion. --allowerasing is required:
# ffmpeg-free pulls in the *-free library subpackages (libswscale-free,
# libavcodec-free, ...) which file-conflict with the complete ffmpeg-libs, and
# swap alone will not remove them. The gstreamer1-plugins-* packages provide
# H.264/H.265 and other restricted codecs to GTK/GStreamer based apps,
# including the GNOME video decoders. gstreamer1-vaapi wires those decoders
# into VA-API.
dnf5 swap -y ffmpeg-free ffmpeg --allowerasing --enablerepo=rpmfusion-free
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
# libva-utils ships vainfo for verification.
#
# Note: we deliberately do NOT swap mesa-va-drivers for the RPM Fusion
# -freeworld build. Intel GPUs do not use Mesa for video acceleration (Mesa VA
# drivers only serve AMD/nouveau); the freeworld mesa swap is what gives AMD
# cards H.264/H.265 decode. On this Intel Pixelbook it provides no benefit, and
# it is fragile (its mesa version frequently lags Fedora's updates, breaking the
# mesa-dri-drivers/libgallium version pins) plus it is a documented cause of
# GNOME login-screen crashes. iHD above already handles all Intel HW decode.
dnf5 swap -y libva-intel-media-driver intel-media-driver --enablerepo=rpmfusion-nonfree
dnf5 install -y libva-utils

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
dnf5 remove -y gnome-shell-extension-background-logo

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
#
# The keymap data lives in dedicated files under system_files/ and is installed
# by the `cp -a /ctx/system_files` at the top of this script:
#   system_files/etc/keyd/cros.conf                    -> /etc/keyd/cros.conf
#   system_files/usr/lib/udev/hwdb.d/70-pixel-keyboard.hwdb -> Three-lines key -> dashboard, Assistant key -> F23
# Edit those files directly; validate with `sudo keyd check` on a device.

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


#### Enable system services
systemctl enable keyd
systemctl enable podman.socket

### GNOME Shell default dash favorites
#
# Pin the default apps shown on the GNOME Shell dash. The list is provided as a
# gschema override in system_files/usr/share/glib-2.0/schemas/, which the
# "cp -avf /ctx/system_files/. /" at the top of this script places in
# /usr/share/glib-2.0/schemas/. Schemas must be recompiled for the override to
# take effect, so do it after all package operations.
#
# Note: this only changes the default for users who never customized their own
# favorites; it does not override a user's existing dconf value.

glib-compile-schemas /usr/share/glib-2.0/schemas
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
# The Pixel Blue artwork (issue #54: logo masters, hicolor icon, pixmaps and
# the plymouth watermark) is no longer overlaid from this repo; it ships as
# the pixel-blue-logos RPM, installed from Copr in the "Pixel Blue branding
# RPM" section above. What stays repo-side are the levers a package cannot
# hold: the os-release rewrites (NAME/PRETTY_NAME must keep tracking the base
# image) and the initramfs regeneration.
#
# Point os-release's LOGO= at the Pixel Blue icon so GNOME surfaces that
# render the distribution logo (Overview, GNOME Software) pick up Pixel Blue
# artwork once the RPM is installed. Only the LOGO key is rewritten here:
# NAME/PRETTY_NAME are rebranded by the #56 section at the end of this
# script, and ID=fedora and the VARIANT* keys stay untouched (Flatpak /
# GNOME Software detection and the Anaconda Silverblue ISO profile depend on
# them).

if grep -q '^LOGO=' /usr/lib/os-release; then
  sed -i 's|^LOGO=.*|LOGO=pixel-blue-logo-icon|' /usr/lib/os-release
else
  echo 'LOGO=pixel-blue-logo-icon' >> /usr/lib/os-release
fi

# Boot splash: plymouth renders the splash artwork from the initramfs, and it
# only reads PNGs. Fedora's default theme is "bgrt"
# (/usr/share/plymouth/plymouthd.defaults); its bgrt.plymouth is a two-step
# theme whose ImageDir is themes/spinner, so the artwork that actually shows
# at boot/shutdown is spinner/watermark.png. Rebranding that one PNG rebrands
# the splash without touching theme selection; dracut's plymouth module bakes
# only the *selected* theme's artwork into the initrd. The pixel-blue-logos
# RPM installed above owns the branded copy of that path (fedora-logos, the
# package the swap removes, shipped the stock one).
# Note: the shutdown splash reads /usr directly (plymouthd --mode=shutdown is
# started from the real root), but the boot/login splash uses the initrd copy
# baked by the dracut below -- and the rpm-ostree rechunk step re-exports the
# tree with a base-artwork initrd, so the Justfile's ostree-rechunk recipe
# regenerates once more on the final image.

# bootc ships the image's initramfs verbatim at deploy time (nothing is
# regenerated on the target), so re-run dracut here to bake the final
# spinner/watermark.png into the boot splash (per
# docs.fedoraproject.org/en-US/bootc/initramfs/). --no-hostonly keeps the
# initrd generic (hardware detection inside the build container would
# otherwise bake in the CI runner's devices).
# Name the kernel *and* the output file. In a container dracut defaults to
# /boot/initramfs-$kver.img, but bootc boots the image's
# /usr/lib/modules/$kver/initramfs.img, so a bare --regenerate-all leaves the
# image's initrd untouched (that is what issue #75's stale boot splash was).
# Same target-selection as ublue-os/bazzite's build_files/build-initramfs and
# ublue-os/bluefin's 19-initramfs.sh; chmod 600 matches how those images ship
# the initrd.
# --add ostree is load-bearing: bootc/ostree images switch root through
# /usr/lib/ostree/ostree-prepare-root, which prepares /sysroot from the
# deployment. Fedora's dracut config does not include the ostree module by
# default (and --no-hostonly does not either), so a regenerated initrd without
# it boots into "The switch root path does not appear to be an OS tree.
# /usr is missing." and panics. Every ublue image adds it explicitly too.
# /root is a symlink to /var/roothome on bootc bases and the target does not
# exist in the build container yet; dracut-install aborts on it ("ERROR:
# installing '/root'"), so create it first (empty; systemd-tmpfiles owns the
# real directory at runtime).
mkdir -p /var/roothome && chmod 700 /var/roothome
for kdir in /usr/lib/modules/*; do
  [ -f "$kdir/modules.dep" ] || continue
  dracut --no-hostonly --add ostree --force "$kdir/initramfs.img" "$(basename "$kdir")"
  chmod 0600 "$kdir/initramfs.img"
  # Guard the failure that shipped once: an initrd without the ostree module
  # builds green but panics at switch-root ("/usr is missing"). Assert by
  # content so a silently dropped module fails the build here instead of
  # bricking an update.
  lsinitrd "$kdir/initramfs.img" > /tmp/lsinitrd.list 2>/dev/null
  if ! grep -q 'usr/lib/ostree/ostree-prepare-root' /tmp/lsinitrd.list; then
    echo "error: ostree module missing from $kdir/initramfs.img" >&2
    exit 1
  fi
done
# Assert it landed *by content*: the stock fedora-logos watermark lives at
# the same path, so a bare path grep in lsinitrd would also pass for a
# freshly-exported-but-unrebranded initrd (this is exactly how issue #75's
# stale boot splash looked). Compare the baked copy against the live /usr file.
wm=/usr/share/plymouth/themes/spinner/watermark.png
wm_want="$(sha256sum "$wm" | cut -d' ' -f1)"
splash_baked=0
for img in /usr/lib/modules/*/initramfs.img /usr/lib/modules/*/images/pxeboot/initramfs.img; do
  [ -f "$img" ] || continue
  # Capture before matching: grep -q closing the pipe early would SIGPIPE
  # lsinitrd and pipefail would report failure for a perfectly good initrd.
  if lsinitrd "$img" > /tmp/lsinitrd.list 2>/dev/null &&
     grep -q 'themes/spinner/watermark.png' /tmp/lsinitrd.list; then
    lsinitrd -f "$wm" "$img" > /tmp/wm_baked.png 2>/dev/null || true
    if [ "$(sha256sum /tmp/wm_baked.png | cut -d' ' -f1)" = "$wm_want" ]; then
      splash_baked=1
    fi
  fi
done
if [ "$splash_baked" -ne 1 ]; then
  echo "error: /usr plymouth watermark not byte-identical in any regenerated initramfs" >&2
  exit 1
fi

# First-boot wizard: current gnome-initial-setup (GTK4 rewrite) ships no
# vendor logo path in its conf (README documents only the [pages] group:
# skip / new_user_only / existing_user_only), so there is nothing to wire
# here. The wizard's title string comes from PRETTY_NAME, rebranded to
# Pixel Blue by the #56 section at the end of this script.

# Clean up dnf cache to reduce image size
dnf5 clean all

### Unify the OS display name (issue #56)
#
# The installed system would otherwise show the upstream identity
# ("Fedora Linux <version> (Silverblue)" and its NAME/VARIANT parts)
# in gnome-initial-setup, gnome-tour, GNOME Settings -> About and the
# GRUB entry titles. Rewrite the brand keys of /usr/lib/os-release so
# all of them say "Pixel Blue".
#
# Only NAME and PRETTY_NAME are touched: ID, VERSION*, VARIANT*,
# PLATFORM_ID and the URL keys must stay intact, because Flatpak,
# GNOME Software and the Anaconda Silverblue profile detection depend
# on them.
#
# PRETTY_NAME is rewritten wholesale (not by prefix sed, because the
# upstream value is "Fedora Linux <version> (Silverblue)" and a prefix
# swap would leave stray words behind); the version is taken from
# VERSION_ID so the string keeps tracking the base image instead of
# being hardcoded.
#
# This runs last so no dnf5 operation can restore the stock file
# afterwards. bootc symlinks /etc/os-release to /usr/lib/os-release, so
# editing the latter covers both.

osrel_version_id="$(sed -n 's/^VERSION_ID=//p' /usr/lib/os-release)"
if [ -z "$osrel_version_id" ]; then
    echo "error: could not extract VERSION_ID from /usr/lib/os-release" >&2
    exit 1
fi

sed -i -e 's|^NAME=.*|NAME="Pixel Blue"|' \
       -e "s|^PRETTY_NAME=.*|PRETTY_NAME=\"Pixel Blue ${osrel_version_id}\"|" \
       /usr/lib/os-release

# Fail the build loudly if the rewrite did not land.
grep -q '^NAME="Pixel Blue"$' /usr/lib/os-release
grep -q '^PRETTY_NAME="Pixel Blue ' /usr/lib/os-release
