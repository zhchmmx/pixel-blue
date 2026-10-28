#!/usr/bin/env bash
# Offline payload gate for the image-builder live ISO.
#
# With the payload embedded in the ISO's squashfs, the Anaconda bootc command
# only installs offline when its --source-imgref matches the embedded
# containers-storage reference EXACTLY; otherwise the installer silently
# ignores the local copy and pulls from the network (documented in the
# bootc-images CI template that first hit this). This script asserts on the
# finished ISO, without booting it:
#   1. /LiveOS/squashfs.img is present and its embedded container storage
#      carries the payload reference;
#   2. the baked interactive-defaults.ks installs from
#      "containers-storage:<payload>" and records "<payload>" as the origin.
#
# Needs root for the loop mount and squashfs-tools for the inspection.
#
# Usage: sudo ./verify-iso-offline.sh <image.iso> <payload-ref>
#   e.g. ghcr.io/zhchmmx/pixel-blue:latest

set -euo pipefail

iso="${1:?usage: verify-iso-offline.sh <image.iso> <payload-ref>}"
payload_ref="${2:?usage: verify-iso-offline.sh <image.iso> <payload-ref>}"

if [ "$(id -u)" -ne 0 ]; then
  echo "error: must run as root (loop mount)" >&2
  exit 1
fi

work="$(mktemp -d)"
cleanup() {
  umount "${work}/mnt" 2>/dev/null || true
  rm -rf "${work}"
}
trap cleanup EXIT

mkdir -p "${work}/mnt"
mount -o ro,loop "${iso}" "${work}/mnt"
if [ ! -f "${work}/mnt/LiveOS/squashfs.img" ]; then
  echo "error: ${iso} has no /LiveOS/squashfs.img (not a container-native live ISO?)" >&2
  exit 1
fi
cp "${work}/mnt/LiveOS/squashfs.img" "${work}/live.img"
umount "${work}/mnt"

# Extract one file out of the live squashfs by its path relative to the
# squashfs root (the -l listing may prefix the same path with an internal
# directory name; match on the suffix and strip the prefix). The listing is
# metadata-only, so cache it once for both lookups.
unsquashfs -l "${work}/live.img" > "${work}/listing"
extract_file() {
  local needle="${1}" dest="${2}" path
  path="$(awk -v n="${needle}" '$NF ~ ("(^|/)" n "$") { print $NF; exit }' "${work}/listing")"
  [ -n "${path}" ] || return 1
  path="${path#"${path%%"${needle}"*}"}"
  unsquashfs -quiet -dest "${dest}" -force "${work}/live.img" "${path}" >/dev/null 2>&1 || true
  find "${dest}" -name "$(basename "${needle}")" -type f -print -quit
}

# 1. The kickstart must install from the embedded containers-storage copy and
#    record the registry reference as the origin.
ks_file="$(extract_file usr/share/anaconda/interactive-defaults.ks "${work}/ks" || true)"
[ -n "${ks_file}" ] || { echo "error: interactive-defaults.ks missing from the live rootfs" >&2; exit 1; }
grep -F -- "bootc --source-imgref containers-storage:${payload_ref} --target-imgref ${payload_ref}" \
  "${ks_file}"

# 2. The payload must be in the embedded container storage: the tagged
#    reference lives in the storage metadata db (sqlite, text-ish; the layer
#    data itself sits under var/lib/containers/storage/overlay/).
db_file="$(extract_file var/lib/containers/storage/db.sql "${work}/db" || true)"
[ -n "${db_file}" ] || {
  echo "error: no containers-storage metadata db inside the ISO squashfs" >&2
  exit 1
}
grep -a -F -- "${payload_ref}" "${db_file}" >/dev/null || {
  echo "error: ${payload_ref} not found in the embedded container storage" >&2
  exit 1
}

echo "OK: ${iso} embeds ${payload_ref} and installs from containers-storage (offline)."
