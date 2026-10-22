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


### MrChromebox UEFI firmware update support (Issue #69)
#
# Pixel Blue checks for MrChromebox UEFI Full ROM updates with an fwupd
# out-of-tree plugin plus a local metadata remote, so updates show up as an
# Update button in GNOME Software.  The daemon (root) flashes with flashrom,
# mirroring the upstream firmware-util.sh logic (auto-clear software
# write-protect, back up, write, verify); the metadata is generated at
# runtime by system_files/usr/libexec/firmware-update-check.sh, driven by
# firmware-update-check.timer.  The plugin source lives in plugin/ and is
# compiled here against the fwupd-devel that exactly matches the installed
# daemon; the one hand-mirrored struct (FuPluginClass) is validated against
# the real upstream source of that exact version below, so nothing needs to
# be version-pinned.

# Runtime packages: fwupd (daemon + fwupdmgr), flashrom (flashing), curl
# (full binary; the base image only ships curl-minimal).
dnf5 install -y fwupd flashrom curl

# Toolchain used to build the plugin; removed again afterwards (flashrom,
# fwupd and curl stay as runtime packages).
#
# The plugin is compiled directly with gcc.  Fedora's fwupd-devel does not
# ship the fwupdplugin headers or a fwupdplugin.pc file, so the plugin
# declares the small API surface it uses itself in
# plugin/mrchromebox/mrchromebox-plugin.h.  Because nothing here is version
# pinned (the base image owns fwupd and must keep taking security updates),
# the ABI assumption is guarded mechanically instead - see the layout check
# below and the runtime class-size gate in fu_plugin_startup().
# The module must be named "libfu_plugin_<name>.so" for the daemon to load
# it, and it has to live in ${libdir}/fwupd-<fwupd-version> (the daemon's
# FU_PATH_KIND_LIBDIR_PKG).
dnf5 install -y fwupd-devel gcc glib2-devel pkgconf-pkg-config
FWUPD_VER="$(rpm -q fwupd --qf '%{VERSION}')"
FLASHROM_VER="$(rpm -q flashrom --qf '%{VERSION}')"

# Floor assertions: fu_plugin_add_device only exists from fwupd 2.1.1
# (2.0.x exported fu_plugin_device_add and the dlopen would die on the
# undefined symbol); the flashrom output/option strings the plugin parses
# were verified against flashrom 1.x.
ver_ge() { [ "$(printf '%s\n%s\n' "$1" "$2" | sort -V | head -n1)" = "$2" ]; }
if ! ver_ge "$FWUPD_VER" 2.1.1; then
	echo "error: fwupd $FWUPD_VER < 2.1.1; fu_plugin_add_device is missing" >&2
	exit 1
fi
if ! ver_ge "$FLASHROM_VER" 1.2; then
	echo "error: flashrom $FLASHROM_VER < 1.2; the plugin parses flashrom 1.x output" >&2
	exit 1
fi

# FuPluginClass prefix guard: fetch the real fu-plugin.h of the EXACT
# installed fwupd and require the hand-mirrored struct to be a field-order
# prefix of the upstream one.  Appending fields at the END is ABI-safe for
# us (fu_plugin_init_vfuncs only fills the mirrored slots); a field inserted
# in the MIDDLE shifts every later vfunc offset while the plugin still
# compiles cleanly - the daemon would then call write_firmware at the wrong
# slot and segfault mid-flash, so the build must fail instead.
curl -sfL --max-time 30 \
	"https://raw.githubusercontent.com/fwupd/fwupd/${FWUPD_VER}/libfwupdplugin/fu-plugin.h" \
	-o /tmp/fu-plugin.h
python3 - "$FWUPD_VER" <<'PYEOF'
import re
import sys


def fields(path):
    src = open(path, encoding="utf-8").read()
    m = re.search(r"struct _FuPluginClass \{(.*?)\n\};", src, re.S)
    if m is None:
        sys.exit(f"cannot find struct _FuPluginClass in {path}")
    body = re.sub(r"/\*.*?\*/", "", m.group(1), flags=re.S)
    return ["parent_class"] + re.findall(r"\(\*([A-Za-z_0-9]+)\)\s*\(", body)


shim = fields("/ctx/plugin/mrchromebox/mrchromebox-plugin.h")
real = fields("/tmp/fu-plugin.h")
if real[: len(shim)] != shim:
    sys.exit(
        "FuPluginClass layout drift vs fwupd %s: mirrored prefix %s is no "
        "longer a field-order prefix of upstream %s; update "
        "plugin/mrchromebox/mrchromebox-plugin.h before shipping" % (sys.argv[1], shim, real)
    )
print(
    "mrchromebox: FuPluginClass shim (%d fields) verified against upstream "
    "fwupd %s (%d fields)" % (len(shim), sys.argv[1], len(real))
)
PYEOF
FWUPD_PLUGIN_DIR="$(pkg-config --variable=libdir fwupd)/fwupd-$(rpm -q fwupd --qf '%{VERSION}')"
# pkg-config prints all flags on ONE line, so split words with read -r -a;
# mapfile would split by lines and collapse every -I/-D into one argument.
read -r -a fwupd_cflags < <(pkg-config --cflags fwupd gio-2.0 glib-2.0)
gcc -shared -fPIC -O2 \
	-I/ctx/plugin/mrchromebox \
	"${fwupd_cflags[@]}" \
	/ctx/plugin/mrchromebox/mrchromebox.c \
	-o /tmp/libfu_plugin_mrchromebox.so
install -D -m 0755 /tmp/libfu_plugin_mrchromebox.so "${FWUPD_PLUGIN_DIR}/libfu_plugin_mrchromebox.so"
# Remove the build toolchain, but keep pkgconf-pkg-config: it is a base-image
# dependency of kmod (required by systemd-udev), so removing it breaks deps.
dnf5 remove -y fwupd-devel gcc glib2-devel

#### Enable system services
systemctl enable keyd
systemctl enable podman.socket
systemctl enable firmware-update-check.timer

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
systemctl --global enable pixel-blue-flash-alert.path

# The ALS daemon is a per-user resident service; it reads
# org.gnome.settings-daemon.plugins.power ambient-enabled inline and pauses
# when the toggle is off. Enabling it globally means it starts for every
# login session, whether the toggle is on or off — daemon checks gsettings
# each cycle and no-ops until the user turns it on.
systemctl --global enable als-brightness.service


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
