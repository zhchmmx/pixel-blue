#!/usr/bin/env bash
#
# firmware-update-check.sh - check MrChromebox UEFI firmware updates for the
# Pixelbook (eve) and generate the local fwupd metadata consumed by the
# pixel-blue-mrchromebox remote (Pixel Blue, Issue #69).
#
# Ported from the upstream MrChromebox/scripts sources.sh/functions.sh
# logic: parse release_current_version / release_current_date (plus the
# per-board FW_HOTFIX override), compare the DMI BIOS release date, confirm
# the ROM image is available, download it together with its .sha1 file,
# verify the checksum and atomically publish firmware.xml.gz.  If the latest
# release is already installed, publish zero-release metadata and drop any
# stale failure marker.
#
# Runs as a root systemd oneshot (firmware-update-check.timer).  Failures are
# deliberately non-fatal: keep the previous metadata and let the next run
# retry.
set -euo pipefail

# Testability overrides; unused in the shipped image.
: "${MRCHROMEBOX_STATE_DIR:=/var/lib/pixel-blue/fwupd}"
: "${MRCHROMEBOX_DMI_DIR:=/sys/class/dmi/id}"
: "${MRCHROMEBOX_EFI_DIR:=/sys/firmware/efi}"
: "${MRCHROMEBOX_BOARD:=eve}"
: "${MRCHROMEBOX_SOURCES_URL:=https://raw.githubusercontent.com/MrChromebox/scripts/main/sources.sh}"
: "${MRCHROMEBOX_BASE_URL:=https://www.mrchromebox.tech/files/firmware/full_rom/}"
: "${MRCHROMEBOX_SKIP_DOWNLOAD:=0}"
: "${MRCHROMEBOX_GUID:=44163a1d-13d2-42c7-bce3-fb8483125bb2}"

log() {
  printf 'firmware-update-check: %s\n' "$*" >&2
}

# MM/DD/YYYY -> YYYYMMDD
date_to_number() {
  local m d y
  [[ "$1" =~ ^([0-9]{1,2})/([0-9]{1,2})/([0-9]{4})$ ]] || return 1
  m=${BASH_REMATCH[1]}
  d=${BASH_REMATCH[2]}
  y=${BASH_REMATCH[3]}
  printf '%s' "$((10#$y * 10000 + 10#$m * 100 + 10#$d))"
}

url_available() {
  curl -sfI -L --max-time 20 "$1" 2>/dev/null | grep -qiE '^HTTP/[0-9.]+ 200'
}

# Download "$url" (ROM) and "$url.sha1", verify the SHA-1 and move the ROM
# into place.  Prints "sha1 sha256 size" on success, returns non-zero on any
# failure (already logged).
download_and_verify() {
  local url="$1"
  local file="$2"
  local rom_tmp sha_tmp expected actual

  rom_tmp=$(mktemp -p "${MRCHROMEBOX_STATE_DIR}" firmware.rom.XXXXXX)
  sha_tmp=$(mktemp -p "${MRCHROMEBOX_STATE_DIR}" firmware.rom.sha1.XXXXXX)
  if ! curl -sfL --max-time 300 "$url" -o "$rom_tmp" 2>/dev/null; then
    log "failed to download $url"
    rm -f "$rom_tmp" "$sha_tmp"
    return 1
  fi
  if ! curl -sfL --max-time 30 "${url}.sha1" -o "$sha_tmp" 2>/dev/null; then
    log "failed to download ${url}.sha1"
    rm -f "$rom_tmp" "$sha_tmp"
    return 1
  fi
  expected=$(awk '{print $1}' "$sha_tmp")
  actual=$(sha1sum "$rom_tmp" | awk '{print $1}')
  if [[ -z "$expected" || "$expected" != "$actual" ]]; then
    log "sha1 mismatch for $file (expected ${expected:-<empty>}, got $actual)"
    rm -f "$rom_tmp" "$sha_tmp"
    return 1
  fi
  mv -f "$rom_tmp" "${MRCHROMEBOX_STATE_DIR}/${file}"
  rm -f "$sha_tmp"
  printf '%s %s %s\n' \
    "$actual" \
    "$(sha256sum "${MRCHROMEBOX_STATE_DIR}/${file}" | awk '{print $1}')" \
    "$(stat -c %s "${MRCHROMEBOX_STATE_DIR}/${file}")"
}

refresh_fwupd() {
  if ! command -v fwupdmgr >/dev/null 2>&1; then
    log "fwupdmgr not found; the fwupd daemon reload will pick the change up"
    return 0
  fi
  if ! fwupdmgr refresh pixel-blue-mrchromebox >/dev/null 2>&1; then
    log "fwupdmgr refresh failed (non-fatal; daemon inotify reload will pick the change up)"
  fi
}

atomically_publish() {
  local xml_tmp="$1"

  gzip -cf "$xml_tmp" >"${xml_tmp}.gz"
  mv -f "${xml_tmp}.gz" "${MRCHROMEBOX_STATE_DIR}/firmware.xml.gz"
  chmod 644 "${MRCHROMEBOX_STATE_DIR}/firmware.xml.gz"
  rm -f "$xml_tmp"
  refresh_fwupd
}

# Each dated release leaves a ~16 MB ROM behind; keep only the image the
# published metadata currently points at ("" = none) and backup.rom.
cleanup_stale_roms() {
  local keep="${1:-}"
  local f

  for f in "${MRCHROMEBOX_STATE_DIR}"/coreboot_edk2-*-mrchromebox_*.rom; do
    [[ -e "$f" ]] || continue
    if [[ -n "$keep" && "$f" == "${MRCHROMEBOX_STATE_DIR}/${keep}" ]]; then
      continue
    fi
    rm -f "$f"
    log "removed superseded ROM $(basename "$f")"
  done
}

publish_metadata_release() {
  local version="$1"
  local date="$2"
  local file="$3"
  local sha1="$4"
  local sha256="$5"
  local size="$6"
  local release_date xml_tmp

  release_date="${date:0:4}-${date:4:2}-${date:6:2}"
  xml_tmp=$(mktemp -p "${MRCHROMEBOX_STATE_DIR}" firmware.xml.XXXXXX)
  cat >"$xml_tmp" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<components version="0.9">
  <component type="firmware">
    <id>org.mrchromebox.pixelblue.firmware</id>
    <name>MrChromebox UEFI Full ROM</name>
    <summary>Custom UEFI firmware for the Google Pixelbook</summary>
    <description><p>Update for the MrChromebox UEFI Full ROM running on this Pixelbook. Clicking Update flashes the new firmware directly. If the flash fails, the desktop alert says whether a reboot is safe (the flash was never touched) or forbidden (the write started or was interrupted); in both cases click Update again to retry.</p></description>
    <provides>
      <firmware type="flashed">${MRCHROMEBOX_GUID}</firmware>
    </provides>
    <metadata>
      <value key="VendorId">DMI:GOOGLE</value>
    </metadata>
    <releases>
      <release version="${version}" date="${release_date}">
        <checksum type="sha1" filename="${file}">${sha1}</checksum>
        <checksum type="sha256" filename="${file}">${sha256}</checksum>
        <location>${file}</location>
        <size type="download">${size}</size>
        <size type="installed">${size}</size>
      </release>
    </releases>
  </component>
</components>
EOF
  atomically_publish "$xml_tmp"
}

publish_metadata_empty() {
  local xml_tmp

  xml_tmp=$(mktemp -p "${MRCHROMEBOX_STATE_DIR}" firmware.xml.XXXXXX)
  cat >"$xml_tmp" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<components version="0.9">
  <component type="firmware">
    <id>org.mrchromebox.pixelblue.firmware</id>
    <name>MrChromebox UEFI Full ROM</name>
    <summary>Custom UEFI firmware for the Google Pixelbook</summary>
    <description><p>MrChromebox UEFI Full ROM firmware for this Pixelbook. Updates are flashed from the GNOME Software Updates page when MrChromebox publishes a new release.</p></description>
    <provides>
      <firmware type="flashed">${MRCHROMEBOX_GUID}</firmware>
    </provides>
    <metadata>
      <value key="VendorId">DMI:GOOGLE</value>
    </metadata>
    <releases />
  </component>
</components>
EOF
  atomically_publish "$xml_tmp"
}

main() {
  local bios_version bios_date installed
  local sources_tmp latest_version latest_date hotfix
  local rom_file rom_url checksum_line sha1 sha256 size

  # Gate 1: UEFI boot only.
  if [[ ! -d "${MRCHROMEBOX_EFI_DIR}" ]]; then
    return 0
  fi
  # Gate 2: custom (non-Google) firmware only.
  bios_version=$(cat "${MRCHROMEBOX_DMI_DIR}/bios_version" 2>/dev/null) || return 0
  if [[ "$bios_version" == Google_* ]]; then
    return 0
  fi
  # Installed firmware date.
  bios_date=$(cat "${MRCHROMEBOX_DMI_DIR}/bios_date" 2>/dev/null) || return 0
  if ! installed=$(date_to_number "$bios_date"); then
    log "invalid bios_date '$bios_date'"
    return 0
  fi

  # Fetch and parse the upstream release info.
  sources_tmp=$(mktemp)
  if ! curl -sfL --max-time 30 "${MRCHROMEBOX_SOURCES_URL}" -o "$sources_tmp" 2>/dev/null; then
    log "cannot fetch ${MRCHROMEBOX_SOURCES_URL}; keeping previous metadata"
    rm -f "$sources_tmp"
    return 0
  fi
  latest_version=$(sed -n 's/^export release_current_version="\([^"]*\)".*/\1/p' "$sources_tmp" | head -n 1)
  latest_date=$(sed -n 's/^export release_current_date="\([0-9]\{8\}\)".*/\1/p' "$sources_tmp" | head -n 1)
  # Only active (non-commented) hotfix entries count; upstream keeps stale
  # entries commented out, e.g. "# [eve]=20260715".
  hotfix=$(sed -n '/^[[:space:]]*#/d; s/.*\[eve\]=\([0-9]\{8\}\).*/\1/p' "$sources_tmp" | head -n 1)
  rm -f "$sources_tmp"
  if [[ -z "$latest_version" || -z "$latest_date" ]]; then
    log "cannot parse release fields from sources.sh; keeping previous metadata"
    return 0
  fi
  if [[ -n "$hotfix" ]]; then
    latest_date="$hotfix"
  fi

  mkdir -p "${MRCHROMEBOX_STATE_DIR}"

  # No update: publish zero-release metadata, drop any stale marker and all
  # cached ROMs (nothing is pending anymore).
  if ((installed >= latest_date)); then
    rm -f "${MRCHROMEBOX_STATE_DIR}/flash-failed"
    publish_metadata_empty
    cleanup_stale_roms ""
    log "firmware up to date (installed $installed, latest $latest_date)"
    return 0
  fi

  # Update available: versioned layout, flat root fallback, hotfix flat.
  rom_file="coreboot_edk2-${MRCHROMEBOX_BOARD}-mrchromebox_${latest_date}.rom"
  if [[ -n "$hotfix" ]]; then
    rom_url="${MRCHROMEBOX_BASE_URL}${rom_file}"
  else
    rom_url="${MRCHROMEBOX_BASE_URL}MrChromebox-${latest_version}/${rom_file}"
    if ! url_available "$rom_url"; then
      log "$rom_url not found; falling back to flat layout"
      rom_url="${MRCHROMEBOX_BASE_URL}${rom_file}"
    fi
  fi

  if [[ "${MRCHROMEBOX_SKIP_DOWNLOAD}" == 1 ]]; then
    # Testing mode: the ROM must already be staged in the state dir.
    if [[ ! -f "${MRCHROMEBOX_STATE_DIR}/${rom_file}" ]]; then
      log "MRCHROMEBOX_SKIP_DOWNLOAD=1 but ${rom_file} is not staged; keeping previous metadata"
      return 0
    fi
    sha1=$(sha1sum "${MRCHROMEBOX_STATE_DIR}/${rom_file}" | awk '{print $1}')
    sha256=$(sha256sum "${MRCHROMEBOX_STATE_DIR}/${rom_file}" | awk '{print $1}')
    size=$(stat -c %s "${MRCHROMEBOX_STATE_DIR}/${rom_file}")
  else
    if ! url_available "$rom_url"; then
      log "firmware image not available at $rom_url; keeping previous metadata"
      return 0
    fi
    if ! checksum_line=$(download_and_verify "$rom_url" "$rom_file"); then
      return 0
    fi
    read -r sha1 sha256 size <<<"$checksum_line"
  fi

  # Publish version = release date (YYYYMMDD), the SAME version space as the
  # plugin device (DMI bios_date as a NUMBER); upstream's release_current_version
  # (e.g. "2609.2") is a different space and would compare as OLDER than any
  # date - fwupd would then never show the update button.
  publish_metadata_release "$latest_date" "$latest_date" "$rom_file" "$sha1" "$sha256" "$size"
  cleanup_stale_roms "$rom_file"
  log "published update metadata for ${latest_version} (${latest_date})"
}

main "$@"
