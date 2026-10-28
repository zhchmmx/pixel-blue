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

# 2. The payload must be in the embedded container storage. Which file holds
#    the tagged reference is a containers-storage implementation detail, and
#    it differs by backend: the JSON backend (current) keeps image records in
#    the <driver>-images/images.json index and layer records in
#    <driver>-layers/layers.json, with the per-image big-data (manifest,
#    signatures) under <driver>-images/<id>/ and no per-layer link files;
#    the legacy sqlite backend used db.sql. Grep the whole small-metadata
#    set for both the overlay and the vfs driver: db.sql(.bak), storage.json,
#    (overlay|vfs)-images/images.json, (overlay|vfs)-images/<id>/{link,
#    image.json}, (overlay|vfs)-layers/layers.json, (overlay|vfs)-layers/
#    <id>/link, overlay/<id>/link, overlay/layers/*, references/*.
#    Layer content under overlay/<id>/diff is not searched.
meta_paths="$(awk '$NF ~ /^(squashfs-root\/)?var\/lib\/containers\/storage\/(db\.sql[^/]*|storage\.json|overlay\/[^/]+\/link|overlay\/layers\/[^/]+|references\/.*|(overlay|vfs)-images\/images\.json|(overlay|vfs)-images\/[^/]+\/(link|image\.json)|(overlay|vfs)-layers\/layers\.json|(overlay|vfs)-layers\/[^/]+\/link)$/ { print $NF }' "${work}/listing")"
[ -n "${meta_paths}" ] || {
  echo "error: no containers-storage metadata inside the ISO squashfs" >&2
  exit 1
}
# unsquashfs resolves path arguments relative to the single top-level
# directory stored in the archive (here "squashfs-root"); passing the
# listed path verbatim silently extracts NOTHING (exit 0, empty dest).
# Strip that first component, but keep already-bare paths intact.
rel_paths=()
while read -r p; do
  [ -n "${p}" ] || continue
  if [[ "${p}" == var/* ]]; then
    rel_paths+=("${p}")
  else
    rel_paths+=("${p#*/}")
  fi
done <<< "${meta_paths}"
for rel in "${rel_paths[@]}"; do
  unsquashfs -quiet -dest "${work}/meta" -force "${work}/live.img" "${rel}" >/dev/null 2>&1 || true
done
if ! grep -a -r -F -- "${payload_ref}" "${work}/meta" >/dev/null; then
  echo "error: ${payload_ref} not found in the embedded container storage metadata" >&2
  echo "diagnostic - metadata candidates by kind:" >&2
  printf 'db.sql*: %s | images.json: %s | image big-data: %s | layers.json: %s | overlay/*/link: %s | overlay/layers: %s | references: %s\n' \
    "$(printf '%s\n' "${meta_paths}" | grep -c '/db\.sql')" \
    "$(printf '%s\n' "${meta_paths}" | grep -c -- '-images/images\.json')" \
    "$(printf '%s\n' "${meta_paths}" | grep -c -- '-images/[^/]*/')" \
    "$(printf '%s\n' "${meta_paths}" | grep -c -- '-layers/layers\.json')" \
    "$(printf '%s\n' "${meta_paths}" | grep -c '/overlay/[^/]*/link')" \
    "$(printf '%s\n' "${meta_paths}" | grep -c '/overlay/layers/')" \
    "$(printf '%s\n' "${meta_paths}" | grep -c '/references/')" >&2
  echo "diagnostic - extracted meta files: $(find "${work}/meta" -type f 2>/dev/null | wc -l)" >&2
  find "${work}/meta" -type f 2>/dev/null | head -10 >&2 || true
  echo "diagnostic - store root entries in the squashfs listing:" >&2
  awk '$NF ~ /^(squashfs-root\/)?var\/lib\/containers\/storage\/[^/]+$/ { print $NF }' \
    "${work}/listing" | head -20 >&2 || true
  for db in "${work}/meta"/var/lib/containers/storage/db.sql*; do
    [ -e "${db}" ] || continue
    echo "diagnostic - $(basename "${db}"): $(wc -c <"${db}") bytes, $(grep -a -c ghcr "${db}" || true) ghcr hits" >&2
    echo "diagnostic - printable runs from $(basename "${db}"):" >&2
    grep -a -o -E '[ -~]{20,}' "${db}" 2>/dev/null | head -25 >&2 || true
  done
  for idx in "${work}/meta"/var/lib/containers/storage/*-images/images.json; do
    [ -e "${idx}" ] || continue
    echo "diagnostic - ${idx#"${work}/meta"} ($(wc -c <"${idx}") bytes):" >&2
    head -c 600 "${idx}" >&2; echo >&2
  done
  imgl="${work}/meta/var/lib/containers/storage/overlay-images"
  if [ -d "${imgl}" ]; then
    echo "diagnostic - overlay-images link contents:" >&2
    head -c 300 "${imgl}"/*/link 2>/dev/null | head -20 >&2 || true
  fi
  echo "diagnostic - ghcr/pixel-blue strings in extracted metadata:" >&2
  grep -a -h -r -o -i -E '[^"]*(ghcr|pixel-blue)[^"]*' "${work}/meta" 2>/dev/null | head -10 >&2 || true
  echo "diagnostic - store structure from the listing:" >&2
  grep -E 'containers/storage/(overlay|vfs)(-images|-layers)?(/[^/]+){0,2}$' \
    "${work}/listing" | head -40 >&2 || true
  exit 1
fi

echo "OK: ${iso} embeds ${payload_ref} and installs from containers-storage (offline)."
