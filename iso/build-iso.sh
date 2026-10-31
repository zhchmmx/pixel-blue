#!/bin/bash
# Build the Pixel Blue installer ISO with lorax.
#
# Runs inside a Fedora container (see .github/workflows/build-disk.yml); it
# installs its own build dependencies, so the same script also works on a
# workstation.
#
# Why lorax. It is the tool Fedora and RHEL use to produce boot.iso, and it
# brings the whole installer environment with it: the anaconda packages, the
# SELinux contexts (it runs setfiles over the tree), the logind ReserveVT
# drop-in, the anaconda-shell VT plumbing, the dracut modules. This repository
# used to reimplement all of that by hand in a Containerfile to feed
# image-builder's bootc-generic-iso, and every one of those pieces broke in a
# way that cost a build to find.
#
# What this script adds on top of a stock lorax run is only what is ours: the
# payload image, our Flatpas, our Anaconda profile. Everything about *how* the
# installer runs is lorax's.
#
# Environment:
#   PAYLOAD_REF         image to install, e.g. ghcr.io/zhchmmx/pixel-blue:latest
#   PAYLOAD_TARGET_REF  reference the installed system follows (default PAYLOAD_REF)
#   PRODUCT             product name shown by the installer (default "Pixel Blue")
#   VERSION             Fedora release to build the installer from (default 44)
#   RELEASE             buildstamp release number (default 1)
#   VARIANT             release variant lorax installs (default Silverblue)
#   ARCH                build architecture (default x86_64)
#   VOLID               ISO volume label, max 32 chars (default Pixel-Blue-Installer)
#   WORK                scratch dir (default /work)
#   OUT                 output dir (default /out)

set -euo pipefail

PAYLOAD_REF="${PAYLOAD_REF:?PAYLOAD_REF must be set, e.g. ghcr.io/owner/image:tag}"
PAYLOAD_TARGET_REF="${PAYLOAD_TARGET_REF:-${PAYLOAD_REF}}"
PRODUCT="${PRODUCT:-Pixel Blue}"
VERSION="${VERSION:-44}"
RELEASE="${RELEASE:-1}"
VARIANT="${VARIANT:-Silverblue}"
ARCH="${ARCH:-x86_64}"
VOLID="${VOLID:-Pixel-Blue-Installer}"
WORK="${WORK:-/work}"
OUT="${OUT:-/out}"
SRC="${SRC:-/src}"

step() { echo; echo "=== $* ==="; }

step "build dependencies"
dnf install -y --setopt=install_weak_deps=False \
  lorax lorax-templates-generic skopeo flatpak ostree xorriso isomd5sum
command -v lorax >/dev/null || { echo "error: lorax not installed" >&2; exit 1; }

mkdir -p "${WORK}" "${OUT}"

step "flatpak repo (what the installer preinstalls)"
REFS_DIR="${SRC}/flatpak_refs" REPO="${WORK}/flatpak/repo" \
  bash "${SRC}/iso/build-flatpak-repo.sh"
test -f "${WORK}/flatpak/repo/summary" || {
  echo "error: no summary in ${WORK}/flatpak/repo -- Anaconda reads the repo through it" >&2
  exit 1
}

step "payload as an OCI layout"
rm -rf "${WORK}/payload"
skopeo copy --all "docker://${PAYLOAD_REF}" "oci:${WORK}/payload:latest"
test -f "${WORK}/payload/oci-layout" || {
  echo "error: ${WORK}/payload is not an OCI layout" >&2
  exit 1
}
du -sh "${WORK}/payload"

step "lorax"
# lorax takes repositories either as --repo FILES or as -s URLs; the container's
# own repo files are the simplest source and track the release we build for.
REPO_ARGS=()
for repo_file in /etc/yum.repos.d/*.repo; do
  REPO_ARGS+=(--repo "${repo_file}")
done
if [ "${#REPO_ARGS[@]}" -eq 0 ]; then
  echo "error: no /etc/yum.repos.d/*.repo to build the installer from" >&2
  exit 1
fi

# Fedora containers filter RPM language files, including in Lorax's installroot.
# Restore the builder's configuration on exit, even if the build fails.
language_macro=/etc/rpm/macros.image-language-conf
if [ -f "${language_macro}" ]; then
  language_backup=$(mktemp -d "${WORK}/lorax-language-macros.XXXXXX")
  trap '
    if [ -f "${language_backup}/macros.image-language-conf" ]; then
      mv -- "${language_backup}/macros.image-language-conf" "${language_macro}"
    fi
    rmdir -- "${language_backup}"
  ' EXIT
  mv -- "${language_macro}" "${language_backup}/macros.image-language-conf"
fi

lorax \
  --product "${PRODUCT}" \
  --version "${VERSION}" \
  --release "${RELEASE}" \
  --variant "${VARIANT}" \
  --buildarch "${ARCH}" \
  --volid "${VOLID}" \
  --isfinal \
  --sharedir /usr/share/lorax/templates.d/99-generic \
  --add-template "${SRC}/iso/templates/15-flatpak.tmpl" \
  --add-template "${SRC}/iso/templates/20-ks.tmpl" \
  --add-template "${SRC}/iso/templates/30-branding.tmpl" \
  --add-template-var "target_ref=${PAYLOAD_TARGET_REF}" \
  --add-template-var "profile_src=${SRC}/iso" \
  --add-template-var "assets_src=${SRC}" \
  "${REPO_ARGS[@]}" \
  "${OUT}/lorax"

BOOT_ISO="${OUT}/lorax/images/boot.iso"
test -f "${BOOT_ISO}" || { echo "error: lorax produced no ${BOOT_ISO}" >&2; exit 1; }

step "add the install sources to the media"
# lorax assembles the ISO from its own product template (x86.tmpl), which lists
# what goes at the ISO root; the add-templates above only reach the installer's
# root filesystem, i.e. the squashfs. mkksiso is the tool lorax ships for this:
# it copies extra directories to the ISO root -- reachable as
# /run/install/repo/<name>, which is what the kickstart asks for -- and re-runs
# implantisomd5, without which the media check fails on boot.
FINAL="${OUT}/pixel-blue.iso"
mkksiso \
  --add "${WORK}/payload" \
  --add "${WORK}/flatpak" \
  "${BOOT_ISO}" "${FINAL}"
test -f "${FINAL}" || { echo "error: mkksiso produced no ${FINAL}" >&2; exit 1; }

step "verify the media actually carries what the kickstart asks for"
fail=0
if xorriso -indev "${FINAL}" -ls /payload 2>/dev/null | grep -q oci-layout; then
  echo "ok: /payload is an OCI layout at the ISO root"
else
  echo "error: no OCI layout at /payload on the ISO -- the kickstart would find nothing to install" >&2
  fail=1
fi
if xorriso -indev "${FINAL}" -ls /flatpak/repo 2>/dev/null | grep -q summary; then
  echo "ok: /flatpak/repo carries a summary Anaconda can read"
else
  echo "error: /flatpak/repo is missing on the ISO -- Anaconda would preinstall nothing" >&2
  fail=1
fi
[ "${fail}" -eq 0 ] || exit 1

step "result"
du -sh "${WORK}/payload" "${WORK}/flatpak" "${BOOT_ISO}" || true
ls -lh "${FINAL}"
echo "iso-path=${FINAL}"
