#!/bin/bash
# Branding: os-release LOGO, and initramfs regeneration
#
# The Pixel Blue artwork (issue #54: logo masters, hicolor icon, pixmaps and
# the plymouth watermark) is no longer overlaid from this repo; it ships as
# the pixel-blue-logos RPM, installed from Copr in 10-logos.sh. What stays
# repo-side are the levers a package cannot hold: the os-release rewrites
# (NAME/PRETTY_NAME must keep tracking the base image) and the initramfs
# regeneration.
#
# Must run after 10-logos.sh: dracut bakes the selected plymouth theme's
# artwork into the initrd, so regenerating before the logo swap would ship
# the stock watermark.

set -ouex pipefail

# Point os-release's LOGO= at the Pixel Blue icon so GNOME surfaces that
# render the distribution logo (Overview, GNOME Settings' About panel) pick
# up Pixel Blue artwork once the RPM is installed. Only the LOGO key is
# rewritten here: NAME/PRETTY_NAME are rebranded by 90-os-name.sh, and
# ID=fedora and the VARIANT* keys stay untouched (Flatpak detection and the
# Anaconda Silverblue ISO profile depend on them).

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
# RPM installed by 10-logos.sh owns the branded copy of that path
# (fedora-logos, the package the swap removes, shipped the stock one).
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
# Pixel Blue by 90-os-name.sh.
