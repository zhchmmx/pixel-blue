#!/usr/bin/env bash
#
# firmware-update-check.sh - weekly MrChromebox UEFI Full ROM check for the
# Pixelbook (eve).
#
# Runs as a root systemd oneshot (firmware-update-check.timer).  It never
# writes SPI flash: it only resolves the latest upstream release, downloads
# and verifies the ROM image into cache/ and publishes check.json for the
# daemon/GUI.  Flashing is exclusively user initiated through StartUpdate on
# tech.pixelblue.Firmware1.
#
# Release metadata and the installed-date helpers come from the vendored
# upstream scripts (vendor/mrchromebox, pinned commit, CI-checked interface);
# nothing is fetched from the network except the ROM image itself.
#
# Failures are deliberately non-fatal: keep the previous check.json and let
# the next run retry.
#
# The vendored scripts are sourced at a runtime path and define the helpers
# and variables used below; shellcheck cannot see through that.
# shellcheck disable=SC1091,SC2034,SC2154
set -euo pipefail

# Testability overrides; unused in the shipped image.
: "${MRCHROMEBOX_STATE_DIR:=/var/lib/pixel-blue/firmware}"
: "${MRCHROMEBOX_DMI_DIR:=/sys/class/dmi/id}"
: "${MRCHROMEBOX_EFI_DIR:=/sys/firmware/efi}"
: "${MRCHROMEBOX_VENDOR_DIR:=/usr/lib/pixel-blue/mrchromebox}"
: "${MRCHROMEBOX_BOARD:=eve}"
: "${MRCHROMEBOX_SKIP_DOWNLOAD:=0}"
: "${MRCHROMEBOX_SKIP_LOOKUP:=0}"
: "${MRCHROMEBOX_NOTES_URL:=https://docs.mrchromebox.tech/docs/firmware/full-rom.html}"

CACHE_DIR="${MRCHROMEBOX_STATE_DIR}/cache"
CHECK_JSON="${MRCHROMEBOX_STATE_DIR}/check.json"
MARKER="${MRCHROMEBOX_STATE_DIR}/flash-failed"

BIOS_VERSION=""
INSTALLED_YMD=""

SOURCES_SH="${MRCHROMEBOX_VENDOR_DIR}/sources.sh"
FUNCTIONS_SH="${MRCHROMEBOX_VENDOR_DIR}/functions.sh"

log() {
  printf 'firmware-update-check: %s\n' "$*" >&2
}

# MM/DD/YYYY -> YYYYMMDD, used only to validate the DMI date before the
# vendored helpers parse it.
date_to_number() {
  local m d y
  [[ "$1" =~ ^([0-9]{1,2})/([0-9]{1,2})/([0-9]{4})$ ]] || return 1
  m=${BASH_REMATCH[1]}
  d=${BASH_REMATCH[2]}
  y=${BASH_REMATCH[3]}
  printf '%s' "$((10#$y * 10000 + 10#$m * 100 + 10#$d))"
}

json_escape() {
  printf '%s' "$1" | sed 's/\\/\\\\/g; s/"/\\"/g'
}

# Download "$url" (ROM) and "$url.sha1", verify the SHA-1 and move the ROM
# into cache/.  Prints "sha1 sha256 size" on success.
download_and_verify() {
  local url="$1"
  local file="$2"
  local rom_tmp sha_tmp expected actual

  rom_tmp=$(mktemp -p "$CACHE_DIR" "${file}.XXXXXX") || return 1
  sha_tmp=$(mktemp -p "$CACHE_DIR" "${file}.sha1.XXXXXX") || {
    rm -f "$rom_tmp"
    return 1
  }
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
  if [[ ! "$expected" =~ ^[0-9a-f]{40}$ || "$expected" != "$actual" ]]; then
    log "sha1 mismatch for $file (expected ${expected:-<empty>}, got $actual)"
    rm -f "$rom_tmp" "$sha_tmp"
    return 1
  fi
  mv -f "$rom_tmp" "${CACHE_DIR}/${file}"
  rm -f "$sha_tmp"
  printf '%s %s %s\n' \
    "$actual" \
    "$(sha256sum "${CACHE_DIR}/${file}" | awk '{print $1}')" \
    "$(stat -c %s "${CACHE_DIR}/${file}")"
}

# Each dated release leaves a ~16 MB ROM behind; keep only the image the
# published check.json currently points at ("" = none).
cleanup_stale_roms() {
  local keep="${1:-}"
  local f

  for f in "${CACHE_DIR}"/coreboot_edk2-*-mrchromebox_*.rom; do
    [[ -e "$f" ]] || continue
    if [[ -n "$keep" && "$f" == "${CACHE_DIR}/${keep}" ]]; then
      continue
    fi
    rm -f "$f"
    log "removed superseded ROM $(basename "$f")"
  done
}

# Only a completed class-A failure may be cleared here.  Class B means the
# SPI write was interrupted: rebooting is forbidden until a successful retry,
# so the marker and the alert must survive an "up to date" check.
clear_class_a_marker() {
  local klass state
  [[ -f "$MARKER" ]] || return 0
  klass=$(sed -n 's/^class=//p' "$MARKER" | head -n 1)
  state=$(sed -n 's/^state=//p' "$MARKER" | head -n 1)
  if [[ "$state" = "failed" && "$klass" = "A" ]]; then
    rm -f "$MARKER"
    log "cleared stale class-A failure marker"
  fi
}

publish_no_update() {
  local tmp checked_at
  checked_at=$(date +%s)
  tmp=$(mktemp -p "$MRCHROMEBOX_STATE_DIR" check.json.XXXXXX)
  cat >"$tmp" <<EOF
{
  "schema": 1,
  "installed_version": "$(json_escape "$BIOS_VERSION")",
  "installed_date": "${INSTALLED_YMD}",
  "available_version": null,
  "available_date": null,
  "release_date": null,
  "notes_url": "${MRCHROMEBOX_NOTES_URL}",
  "downloaded": false,
  "rom_file": null,
  "sha1": null,
  "sha256": null,
  "size": 0,
  "checked_at": ${checked_at}
}
EOF
  chmod 644 "$tmp"
  mv -f "$tmp" "$CHECK_JSON"
}

publish_update() {
  local available_version="$1" available_date="$2" rom_file="$3"
  local sha1="$4" sha256="$5" size="$6"
  local release_date tmp checked_at

  release_date="${available_date:0:4}-${available_date:4:2}-${available_date:6:2}"
  checked_at=$(date +%s)
  tmp=$(mktemp -p "$MRCHROMEBOX_STATE_DIR" check.json.XXXXXX)
  cat >"$tmp" <<EOF
{
  "schema": 1,
  "installed_version": "$(json_escape "$BIOS_VERSION")",
  "installed_date": "${INSTALLED_YMD}",
  "available_version": "$(json_escape "$available_version")",
  "available_date": "${available_date}",
  "release_date": "${release_date}",
  "notes_url": "${MRCHROMEBOX_NOTES_URL}",
  "downloaded": true,
  "rom_file": "$(json_escape "$rom_file")",
  "sha1": "${sha1}",
  "sha256": "${sha256}",
  "size": ${size},
  "checked_at": ${checked_at}
}
EOF
  chmod 644 "$tmp"
  mv -f "$tmp" "$CHECK_JSON"
}

main() {
  local bios_date installed available_version rom_file rom_url base
  local latest_ymd

  # Gate 1: UEFI boot only.
  if [[ ! -d "${MRCHROMEBOX_EFI_DIR}" ]]; then
    log "not booted in UEFI mode; nothing to do"
    return 0
  fi
  # Gate 2: custom (non-Google) firmware only.
  BIOS_VERSION=$(cat "${MRCHROMEBOX_DMI_DIR}/bios_version" 2>/dev/null) || return 0
  if [[ "$BIOS_VERSION" == Google_* ]]; then
    log "still running Google stock firmware; nothing to do"
    return 0
  fi
  bios_date=$(cat "${MRCHROMEBOX_DMI_DIR}/bios_date" 2>/dev/null) || return 0
  if ! INSTALLED_YMD=$(date_to_number "$bios_date"); then
    log "invalid bios_date '$bios_date'"
    return 0
  fi

  mkdir -p "${MRCHROMEBOX_STATE_DIR}" "$CACHE_DIR" "${MRCHROMEBOX_STATE_DIR}/export"
  chmod 755 "${MRCHROMEBOX_STATE_DIR}" "$CACHE_DIR"

  # A manual CheckNow from the GUI can race the timer; serialize the whole
  # lookup/download/publish sequence.
  if command -v flock >/dev/null 2>&1; then
    exec 9>"${MRCHROMEBOX_STATE_DIR}/check.lock"
    if ! flock -w 600 9; then
      log "another check is still running; skipping"
      return 0
    fi
  else
    log "flock not found; running without the serialization lock"
  fi
  # Safe only after taking the lock: another run's in-flight temp file must
  # not be deleted from under it.
  rm -f "${MRCHROMEBOX_STATE_DIR}"/check.json.*

  if [[ ! -r "$SOURCES_SH" || ! -r "$FUNCTIONS_SH" ]]; then
    log "vendored MrChromebox scripts missing under ${MRCHROMEBOX_VENDOR_DIR}"
    return 0
  fi
  # shellcheck source=/dev/null
  if ! source "$SOURCES_SH"; then
    log "cannot source ${SOURCES_SH}; keeping previous metadata"
    return 0
  fi
  # shellcheck source=/dev/null
  if ! source "$FUNCTIONS_SH"; then
    log "cannot source ${FUNCTIONS_SH}; keeping previous metadata"
    return 0
  fi

  device="$MRCHROMEBOX_BOARD"
  firmwareType="Full ROM / UEFI"
  fwDate="$bios_date"
  isChromeOS=false
  isChromiumOS=false
  isIntel=true
  flashrom_params=""

  INSTALLED_YMD=$(fullrom_installed_yyyymmdd) || {
    log "cannot parse installed firmware date '$bios_date'"
    return 0
  }
  if ! fullrom_slot_info latest; then
    log "cannot resolve the latest release from sources.sh"
    return 0
  fi
  latest_ymd="$_fullrom_slot_date"
  if [[ ! "$latest_ymd" =~ ^[0-9]{8}$ ]]; then
    log "latest release date '$latest_ymd' is invalid"
    return 0
  fi
  if ! rom_file=$(fullrom_resolve_slot latest); then
    log "cannot build the ROM file name for ${latest_ymd}"
    return 0
  fi
  if [[ "$_fullrom_slot_is_hotfix" = true ]]; then
    available_version="hotfix-${latest_ymd}"
  else
    available_version="$(fullrom_slot_label latest)"
  fi

  # Nothing newer than what is already installed: publish zero-release
  # metadata, drop a stale class-A failure marker and free the ROM cache.
  if ! fullrom_date_newer_than_installed "$latest_ymd"; then
    publish_no_update
    cleanup_stale_roms ""
    clear_class_a_marker
    log "firmware up to date (installed ${INSTALLED_YMD}, latest ${latest_ymd})"
    return 0
  fi

  if ! base=$(fullrom_cdn_base_for_slot latest); then
    log "cannot resolve the download directory for ${latest_ymd}"
    return 0
  fi
  rom_url="${base}${rom_file}"

  if [[ "$MRCHROMEBOX_SKIP_DOWNLOAD" != 1 && "$MRCHROMEBOX_SKIP_LOOKUP" != 1 ]]; then
    if ! fullrom_http_available "$rom_url"; then
      if [[ "$base" != "$fullrom_source" ]] && fullrom_http_available "${fullrom_source}${rom_file}"; then
        rom_url="${fullrom_source}${rom_file}"
      else
        log "firmware image not available at ${rom_url}; keeping previous metadata"
        return 0
      fi
    fi
  fi

  local cached="${CACHE_DIR}/${rom_file}"
  local checksum_line sha1 sha256 size
  if [[ "$MRCHROMEBOX_SKIP_DOWNLOAD" = 1 || "$MRCHROMEBOX_SKIP_LOOKUP" = 1 ]]; then
    if [[ ! -f "$cached" ]]; then
      log "staged mode but ${rom_file} is not in ${CACHE_DIR}; keeping previous metadata"
      return 0
    fi
    sha1=$(sha1sum "$cached" | awk '{print $1}')
    sha256=$(sha256sum "$cached" | awk '{print $1}')
    size=$(stat -c %s "$cached")
  else
    if ! checksum_line=$(download_and_verify "$rom_url" "$rom_file"); then
      return 0
    fi
    read -r sha1 sha256 size <<<"$checksum_line"
  fi

  publish_update "$available_version" "$latest_ymd" "$rom_file" "$sha1" "$sha256" "$size"
  cleanup_stale_roms "$rom_file"
  log "published update ${available_version} (${latest_ymd}); installed ${INSTALLED_YMD}"
}

main "$@"
