#!/usr/bin/bash
# Build the Pixel Blue live/installer layer.
#
# This image is *not* the ISO. It is the rootfs that osbuild image-builder
# squashes into /LiveOS/squashfs.img when it builds the `bootc-generic-iso`
# artifact: the ISO's live system is exactly this image, and the installer
# (Anaconda) runs from it.
#
# Everything image-builder needs from this container lives here, because the
# container is the contract (see the ISOs page of the image-builder docs,
# osbuild.org/docs/developer-guide/projects/image-builder/advanced/bootc/isos):
#
#   1. kernel          /usr/lib/modules/<kver>/vmlinuz        (exactly one)
#   2. initramfs       /usr/lib/modules/<kver>/initramfs.img
#   3. UEFI vendor     /usr/lib/efi/shim/*/EFI/<vendor>
#   4. EFI binaries    /boot/efi/EFI/<vendor>/{shimx64,grubx64}.efi
#   5. grub modules    /usr/lib/grub/i386-pc + /usr/share/grub2/unicode.pf
#   6. ISO config      /usr/lib/image-builder/bootc/iso.yaml
#   7. build tools     mksquashfs xorriso implantisomd5 grub2-mkimage python3
#
# The script asserts every one of those at the end. Without the assertions a
# broken contract only surfaces inside osbuild, in a much less readable form.
#
# What this image deliberately does NOT contain: the install payload. The
# published Pixel Blue image is handed to image-builder by `just build-iso`
# as --bootc-installer-payload-ref, which copies it from the host's rootful
# container storage into the squashfs at /var/lib/containers/storage. That
# keeps the layer small and avoids running podman inside the build.

set -euxo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PAYLOAD_IMAGE="${PAYLOAD_IMAGE:?PAYLOAD_IMAGE must be set}"
export TMPDIR=/tmp

# /root is a symlink to /var/roothome on bootc bases and the target does not
# exist in the build container yet; dracut-install aborts on it otherwise.
mkdir -p "$(realpath /root)"

# Flatpak's bwrap sandbox writes /proc/sys/user/max_user_namespaces, which is
# read-only by default in the build container. The build runs with
# --cap-add sys_admin, which makes this remount work.
mount -o remount,rw /proc/sys

# --- Flatpak payload -------------------------------------------------------
# The Flatpak preinstalls ride along inside this image (/var/lib/flatpak) and
# the installer rsyncs them into the target at install time, so the installed
# system has them without any network access. The base image already ships
# /etc/flatpak/remotes.d/flathub.flatpakrepo; make sure the system
# installation knows the remote before installing.
flatpak remote-add --system --if-not-exists flathub \
  https://flathub.org/repo/flathub.flatpakrepo
find /ctx/flatpak_refs -maxdepth 1 -type f -print0 |
  sort -z |
  xargs -0 cat >/tmp/flatpak-refs
xargs -r flatpak install --system -y --noninteractive </tmp/flatpak-refs
installed_flatpaks="$(flatpak list --system --app --columns=application)"
if [ -z "$installed_flatpaks" ]; then
  echo "error: no Flatpak applications were installed" >&2
  exit 1
fi

# --- Live/installer packages ----------------------------------------------
# anaconda-live pulls the Web UI and its storage plugins; firefox is the Web
# UI web engine. dracut-live provides dmsquash-live for the live rootfs.
# grub2-efi-x64-cdboot + grub2-pc-modules + grub2-common provide the EFI
# binaries, the BIOS modules and unicode.pf that bootc-generic-iso puts on
# the ISO. squashfs-tools/xorriso/isomd5sum/grub2-tools are the contract's
# build tools (item 7) -- image-builder ships its own copies, but keeping
# them here means an uncontainerised `image-builder` run also works.
# rsync/ostree are used by the kickstart post-scripts.
dnf install -y \
  anaconda-live \
  dracut-live \
  firefox \
  grub2-common \
  grub2-efi-x64-cdboot \
  grub2-pc-modules \
  grub2-tools \
  grub2-tools-minimal \
  isomd5sum \
  libblockdev-btrfs \
  libblockdev-dm \
  libblockdev-lvm \
  livesys-scripts \
  ostree \
  python3 \
  rsync \
  squashfs-tools \
  xorriso

# --- Live session ----------------------------------------------------------
# livesys creates liveuser, enables GDM autologin and starts the session
# selected below. The GNOME session script also exposes the installer in the
# dash, starts the welcome screen and disables gnome-initial-setup for the
# live user.
sed -i 's/^livesys_session=.*/livesys_session="gnome"/' /etc/sysconfig/livesys
systemctl enable livesys.service livesys-late.service

# Brand the live installer entry point and make its icon resolvable. The
# pixel-blue-logos RPM provides pixel-blue-logo-icon; Anaconda and the
# welcome screen still look up the Fedora names. liveinst.desktop ships an
# unlocalized Name plus localized Name[...] entries, so delete both and add
# the branded Name; the translations would otherwise keep winning over it
# in non-English locales.
sed -i -e '/^Name=/d' -e '/^Name\[/d' \
  -e 's/^NoDisplay=true/NoDisplay=false/' \
  /usr/share/applications/liveinst.desktop
sed -i '/^\[Desktop Entry\]/a Name=Install Pixel Blue' \
  /usr/share/applications/liveinst.desktop
for size in 16x16 22x22 24x24 32x32 36x36 48x48 96x96 256x256; do
  ln -sf pixel-blue-logo-icon.png \
    "/usr/share/icons/hicolor/${size}/apps/fedora-logo-icon.png"
done
ln -sf pixel-blue-logo-icon.svg \
  /usr/share/icons/hicolor/scalable/apps/fedora-logo-icon.svg
ln -sf /usr/share/icons/hicolor/scalable/apps/pixel-blue-logo-icon.svg \
  /usr/share/icons/hicolor/scalable/apps/org.fedoraproject.AnacondaInstaller.svg
ln -sf /usr/share/icons/hicolor/256x256/apps/pixel-blue-logo-icon.png \
  /usr/share/pixmaps/fedora-logo-icon.png
# The base image's icon cache predates these symlinks (and the fedora-logos
# swap in 10-logos.sh), so refresh it or the shell may keep the stale names.
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
  gtk-update-icon-cache -f -t /usr/share/icons/hicolor
fi

# Keep the live dashboard focused on trying/installing the image. zz99 sorts
# after the image's zz0- override, so it wins for the live session only.
cat >/usr/share/glib-2.0/schemas/zz99-pixel-blue-live.gschema.override <<'EOF'
[org.gnome.shell]
favorite-apps=['anaconda.desktop', 'org.mozilla.firefox.desktop', 'org.gnome.Nautilus.desktop', 'io.github.kolunmi.Bazaar.desktop']
EOF
glib-compile-schemas /usr/share/glib-2.0/schemas

# The live root is an overlay; the daily update timer must not try to stage a
# bootc deployment into it.
systemctl disable uupd.timer || true

# liveinst passes /etc/system-release to Anaconda as the product name;
# rebrand it in the installer environment only.
if [ -e /etc/system-release ]; then
  version_id="$(sed -n 's/^VERSION_ID=//p' /usr/lib/os-release)"
  rm -f /etc/system-release
  printf 'Pixel Blue release %s (Silverblue)\n' "$version_id" >/etc/system-release
fi

# --- Live boot initramfs ---------------------------------------------------
# dmsquash-live mounts the ISO's squashfs as the live root; the autooverlay
# module gives it a writable tmpfs upper. Regenerate every kernel initramfs
# with dracut-live, keeping the image's dracut configuration. This *replaces*
# the image's generic initrd: the squashfs ships exactly one, at the
# contract's path, and it has to be the live one.
for kdir in /usr/lib/modules/*; do
  [ -f "$kdir/modules.dep" ] || continue
  kver="$(basename "$kdir")"
  DRACUT_NO_XATTR=1 dracut --force --zstd --reproducible --no-hostonly \
    --add "dmsquash-live dmsquash-live-autooverlay" \
    "$kdir/initramfs.img" "$kver"
  chmod 0600 "$kdir/initramfs.img"
  lsinitrd "$kdir/initramfs.img" >/tmp/lsinitrd.list
  # Match the basename only: dracut-install stores the script under usr/bin
  # because the initramfs ships sbin -> usr/sbin -> bin, even though the
  # module installs it as /sbin/dmsquash-live-root (which still resolves).
  if ! grep -q 'dmsquash-live-root' /tmp/lsinitrd.list; then
    echo "error: dmsquash-live missing from $kdir/initramfs.img" >&2
    exit 1
  fi
done

# --- ISO contract: EFI binaries and menu -----------------------------------
# /boot/efi is not part of the installed system; image-builder only reads it
# to build the ISO's ESP.
mkdir -p /boot/efi
cp -av /usr/lib/efi/*/*/EFI /boot/efi/
cp -v /boot/efi/EFI/fedora/grubx64.efi /boot/efi/EFI/BOOT/fbx64.efi

# The ISO label and the GRUB menu live in iso.yaml. image-builder reads it
# from /usr/lib/image-builder/bootc/; the historical
# /usr/lib/bootc-image-builder/ prefix still works but is deprecated, and the
# prefix is resolved once for all files, so never spread files across both.
install -Dm644 "$SCRIPT_DIR/iso.yaml" \
  /usr/lib/image-builder/bootc/iso.yaml

# The containers configuration is *not* needed to pull anything at build time
# any more (the payload pull happens on the host). It is needed at install
# time: install-configure-upgrade.ks seeds these files into the deployment so
# the installed system can verify the signed origin on `bootc upgrade`.
install -Dm644 /ctx/containers/policy.json /etc/containers/policy.json
install -Dm644 /ctx/containers/registries.d/pixel-blue.yaml \
  /etc/containers/registries.d/pixel-blue.yaml

# --- Live-only boot tweaks -------------------------------------------------
# / on a live ISO is an overlay whose upperdir lives under /run (a small
# tmpfs). ostree needs real space in /var/tmp while installing, so mount a
# larger tmpfs there at boot.
rm -rf /var/tmp
mkdir -p /var/tmp
cat >/etc/systemd/system/var-tmp.mount <<'EOF'
[Unit]
Description=Larger tmpfs for /var/tmp on live system

[Mount]
What=tmpfs
Where=/var/tmp
Type=tmpfs
Options=size=50%%,nr_inodes=1m,x-systemd.graceful-option=usrquota

[Install]
WantedBy=local-fs.target
EOF
systemctl enable var-tmp.mount

rm -f /etc/localtime
systemd-firstboot --timezone UTC

# --- Anaconda defaults -----------------------------------------------------
# The destination-only Silverblue flow comes from the stock fedora-silverblue
# profile: the main image preserves ID=fedora and VARIANT_ID=silverblue.
# These post-scripts add the offline payload, the Flatpak preinstalls and the
# registry origin rewrite.
install -Dm644 "$SCRIPT_DIR/anaconda/install-flatpaks.ks" \
  /usr/share/anaconda/post-scripts/install-flatpaks.ks
install -Dm644 "$SCRIPT_DIR/anaconda/restore-selinux-labels.ks" \
  /usr/share/anaconda/post-scripts/restore-selinux-labels.ks
cat >/usr/share/anaconda/post-scripts/install-configure-upgrade.ks <<'EOF'
%post --erroronfail --nochroot --interpreter=/usr/bin/bash
set -euxo pipefail

# Anaconda installs the payload from containers-storage (offline). Point the
# installed deployment's origin at the signed registry reference so bootc
# upgrade continues to work after the ISO is gone. This runs --nochroot, so
# the physical install root (and its ostree repo) is at /mnt/sysimage.
mapfile -t origins < <(find /mnt/sysimage/ostree/deploy -path '*/deploy/*.origin' -type f)
if [ "${#origins[@]}" -eq 0 ]; then
  echo "error: no bootc deployment origin found" >&2
  exit 1
fi
for origin in "${origins[@]}"; do
  if ! grep -q '^container-image-reference=' "$origin"; then
    echo "error: $origin has no container-image-reference" >&2
    exit 1
  fi
  sed -i \
    's|^container-image-reference=.*|container-image-reference=ostree-image-signed:docker://@PAYLOAD_IMAGE@|' \
    "$origin"
  grep -q '^container-image-reference=ostree-image-signed:docker://@PAYLOAD_IMAGE@$' "$origin"

  # The signed origin forces every bootc upgrade to verify the payload
  # against the host's /etc/containers config. A payload image published
  # before the matchRepository/registries.d change still carries the old
  # config, which would fail that check forever. Seed the live layer's
  # current files into the deployment: libostree initializes a deployment's
  # /etc (the origin file's <rev>.<serial> directory) at deploy time, and
  # keeps these local edits across upgrades until the image ships matching
  # files itself.
  deploy_dir="$(dirname "$origin")/$(basename "$origin" .origin)"
  install -Dm644 /etc/containers/policy.json \
    "${deploy_dir}/etc/containers/policy.json"
  install -Dm644 /etc/containers/registries.d/pixel-blue.yaml \
    "${deploy_dir}/etc/containers/registries.d/pixel-blue.yaml"
done
%end
EOF
sed -i "s|@PAYLOAD_IMAGE@|${PAYLOAD_IMAGE}|g" \
  /usr/share/anaconda/post-scripts/install-configure-upgrade.ks

cat >>/usr/share/anaconda/interactive-defaults.ks <<EOF

# Pixel Blue live install.
# The post-scripts in /usr/share/anaconda/post-scripts are auto-merged into
# the kickstart by Anaconda (in name order, so restore-selinux-labels runs
# last); %include-ing them here as well would execute every %post twice.
#
# --transport=containers-storage reads the payload that image-builder copied
# into /var/lib/containers/storage, so the install needs no network at all.
# Signature verification is skipped here (offline, and the payload is the
# image this ISO was built from); the *installed* origin is still the signed
# registry reference, so upgrades stay verified.
ostreecontainer --url=${PAYLOAD_IMAGE} --transport=containers-storage --no-signature-verification
EOF

# --- Contract assertions ---------------------------------------------------
# Fail here, with a readable message, instead of inside osbuild.
kernels=0
for kdir in /usr/lib/modules/*; do
  [ -f "$kdir/vmlinuz" ] || continue
  kernels=$((kernels + 1))
  [ -f "$kdir/initramfs.img" ] || {
    echo "error: $kdir/initramfs.img missing (contract item 2)" >&2
    exit 1
  }
done
if [ "$kernels" -ne 1 ]; then
  echo "error: expected exactly one kernel in /usr/lib/modules, found $kernels" >&2
  echo "       bootc-generic-iso defines the behaviour for multiple kernels as undefined" >&2
  exit 1
fi

vendor_dir=""
for d in /usr/lib/efi/shim/*/EFI/*; do
  [ -d "$d" ] || continue
  if [ "$(basename "$d")" = BOOT ]; then continue; fi
  vendor_dir="$(basename "$d")"
done
if [ -z "$vendor_dir" ]; then
  echo "error: no UEFI vendor directory under /usr/lib/efi/shim/*/EFI (contract item 3)" >&2
  exit 1
fi
for f in shimx64.efi grubx64.efi; do
  [ -f "/boot/efi/EFI/${vendor_dir}/${f}" ] || {
    echo "error: /boot/efi/EFI/${vendor_dir}/${f} missing (contract item 4)" >&2
    exit 1
  }
done

for f in /usr/lib/image-builder/bootc/iso.yaml \
  /usr/share/grub2/unicode.pf; do
  [ -f "$f" ] || {
    echo "error: $f missing (contract items 5/6)" >&2
    exit 1
  }
done
if [ "$(uname -m)" = x86_64 ] && [ ! -d /usr/lib/grub/i386-pc ]; then
  echo "error: /usr/lib/grub/i386-pc missing (contract item 5, grub2-pc-modules)" >&2
  exit 1
fi
for exe in mksquashfs xorriso implantisomd5 grub2-mkimage python3; do
  command -v "$exe" >/dev/null 2>&1 || {
    echo "error: $exe not found in PATH (contract item 7)" >&2
    exit 1
  }
done

# --- Size ------------------------------------------------------------------
dnf5 clean all
