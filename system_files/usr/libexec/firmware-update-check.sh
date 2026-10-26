#!/usr/bin/env bash
#
# firmware-update-check.sh - weekly MrChromebox UEFI Full ROM check for the
# Pixelbook (eve).
#
# Runs as a root systemd oneshot (firmware-update-check.timer).  It never
# writes SPI flash: it resolves the latest upstream release, downloads and
# verifies the ROM image, wraps it into a fwupd cabinet under cab/.  fwupd
# watches that directory and rebuilds its metadata by itself, so publishing a
# cabinet needs no refresh call.  Installing is exclusively user initiated
# through fwupd (`fwupdmgr update`), which drives the mrchromebox plugin; that
# plugin owns preflight, backup, write protection and flashing.
#
# Release metadata and the installed-date helpers come from the vendored
# upstream scripts (vendor/mrchromebox, pinned commit, CI-checked interface);
# nothing is fetched from the network except the ROM image itself.
#
# Failures are deliberately non-fatal: keep the previous cabinet and let the
# next run retry.
#
# The vendored scripts are sourced at a runtime path and define the helpers
# and variables used below; shellcheck cannot see through that.
# shellcheck disable=SC1091,SC2034,SC2154
set -euo pipefail

# Testability overrides; unused in the shipped image.
: "${MRCHROMEBOX_STATE_DIR:=/var/lib/pixel-blue/firmware}"
# The published cabinets live under fwupd's own state directory: it is labeled
# fwupd_var_lib_t, and `fwupdmgr clean` deletes every file under the parent of
# a directory remote. Keeping them apart means a clean can never take
# backup.rom with it.
: "${MRCHROMEBOX_FWUPD_DIR:=/var/lib/fwupd/pixel-blue}"
: "${MRCHROMEBOX_DMI_DIR:=/sys/class/dmi/id}"
: "${MRCHROMEBOX_EFI_DIR:=/sys/firmware/efi}"
: "${MRCHROMEBOX_VENDOR_DIR:=/usr/lib/pixel-blue/mrchromebox}"
: "${MRCHROMEBOX_BOARD:=eve}"
: "${MRCHROMEBOX_SKIP_DOWNLOAD:=0}"
: "${MRCHROMEBOX_SKIP_LOOKUP:=0}"
: "${MRCHROMEBOX_NOTES_URL:=https://docs.mrchromebox.tech/docs/firmware/full-rom.html}"

CAB_DIR="${MRCHROMEBOX_FWUPD_DIR}/cab"
CACHE_DIR="${MRCHROMEBOX_STATE_DIR}/cache"

# Must match FU_MRCHROMEBOX_INSTANCE_ID in build_files/fwupd-plugin; fwupd
# matches a release to a device through this GUID.  It is
# fwupd_guid_hash_string("mrchromebox:board:eve").
COMPONENT_ID="tech.pixelblue.mrchromebox.eve"
COMPONENT_GUID="6942c9af-0f36-5798-8a97-11decdc7c16a"
METAINFO_NAME="${COMPONENT_ID}.metainfo.xml"

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

# Download "$url" (ROM) and "$url.sha1", verify the SHA-1 and move the ROM
# into cache/.  Prints "sha256 size" on success.
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
  printf '%s %s\n' \
    "$(sha256sum "${CACHE_DIR}/${file}" | awk '{print $1}')" \
    "$(stat -c %s "${CACHE_DIR}/${file}")"
}

# Each dated release leaves a ~16 MB ROM behind; keep only the image the
# published cabinet currently points at ("" = none).
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

# Keep at most one cabinet ("" = none), so fwupd does not advertise a release
# that upstream has since withdrawn.
cleanup_stale_cabs() {
  local keep="${1:-}"
  local f

  for f in "${CAB_DIR}"/*.cab; do
    [[ -e "$f" ]] || continue
    if [[ -n "$keep" && "$f" == "${CAB_DIR}/${keep}" ]]; then
      continue
    fi
    rm -f "$f"
    log "removed superseded cabinet $(basename "$f")"
  done
}

# Wrap the verified ROM in a cabinet fwupd understands.  Nothing signs this
# metadata: fwupd treats every release from a directory remote as trusted
# (fu-release.c, fu_release_ensure_trust_flags) without touching the
# OnlyTrusted default, and the payload is already pinned to the upstream .sha1
# because the cabinet is generated locally from that exact file.
build_cabinet() {
  local version="$1" date_ymd="$2" rom_name="$3" sha256="$4" size="$5"
  local release_date tmpdir label

  release_date="${date_ymd:0:4}-${date_ymd:4:2}-${date_ymd:6:2}"
  label="$6"

  tmpdir=$(mktemp -d "${CAB_DIR}/.build.XXXXXX") || return 1

  cp "${CACHE_DIR}/${rom_name}" "${tmpdir}/${rom_name}" || {
    rm -rf "$tmpdir"
    return 1
  }

  cat >"${tmpdir}/${METAINFO_NAME}" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<component type="firmware">
  <id>${COMPONENT_ID}</id>
  <name>MrChromebox UEFI Full ROM</name>
  <summary>Coreboot UEFI Full ROM for the Google Pixelbook (eve)</summary>
  <description>
    <p>MrChromebox Full ROM release ${label} (${release_date}) for the
    Google Pixelbook. Upstream notes:
    ${MRCHROMEBOX_NOTES_URL}</p>
  </description>
  <provides>
    <firmware type="flashed">${COMPONENT_GUID}</firmware>
  </provides>
  <releases>
    <release version="${version}" date="${release_date}" urgency="high">
      <checksum type="sha256" target="content" filename="${rom_name}">${sha256}</checksum>
      <size type="installed">${size}</size>
    </release>
  </releases>
  <custom>
    <value key="LVFS::VersionFormat">plain</value>
  </custom>
</component>
EOF

  # gcab keeps the payload deflated; fwupd reads it with libgcab.
  if ! (cd "$tmpdir" && gcab -z -c firmware.cab "$rom_name" "$METAINFO_NAME"); then
    log "cannot build the firmware cabinet"
    rm -rf "$tmpdir"
    return 1
  fi
  chmod 644 "${tmpdir}/firmware.cab"
  mv -f "${tmpdir}/firmware.cab" "${CAB_DIR}/${version}.cab" || {
    rm -rf "$tmpdir"
    return 1
  }
  rm -rf "$tmpdir"
}

main() {
  local bios_date rom_file rom_url base
  local latest_ymd label cab_name available_version

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

  mkdir -p "${MRCHROMEBOX_STATE_DIR}" "$CACHE_DIR" "$MRCHROMEBOX_FWUPD_DIR" "$CAB_DIR"
  chmod 755 "${MRCHROMEBOX_STATE_DIR}" "$CACHE_DIR" "$MRCHROMEBOX_FWUPD_DIR" "$CAB_DIR"

  # A manual refresh from a fwupd frontend can race the timer; serialize the
  # whole lookup/download/publish sequence.
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
  rm -rf "${CAB_DIR}"/.build.* "${CAB_DIR}"/.cab.*

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
  label=$(fullrom_slot_label latest) || label=""
  # The plugin exposes the DMI date as an 8-digit plain version; keep the
  # release version in the same shape so fwupd can order the two.
  available_version="$latest_ymd"
  cab_name="${available_version}.cab"

  # Nothing newer than what is already installed: drop any advertised release
  # and free the ROM cache.
  if ! fullrom_date_newer_than_installed "$latest_ymd"; then
    cleanup_stale_cabs ""
    cleanup_stale_roms ""
    log "firmware up to date (installed ${INSTALLED_YMD}, latest ${latest_ymd})"
    return 0
  fi

  # Already published for this release: nothing to download again.
  if [[ -s "${CAB_DIR}/${cab_name}" && "$MRCHROMEBOX_SKIP_DOWNLOAD" != 1 ]]; then
    cleanup_stale_cabs "$cab_name"
    log "cabinet for ${available_version} already published"
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
  local checksum_line sha256 size
  if [[ "$MRCHROMEBOX_SKIP_DOWNLOAD" = 1 || "$MRCHROMEBOX_SKIP_LOOKUP" = 1 ]]; then
    if [[ ! -f "$cached" ]]; then
      log "staged mode but ${rom_file} is not in ${CACHE_DIR}; keeping previous metadata"
      return 0
    fi
    sha256=$(sha256sum "$cached" | awk '{print $1}')
    size=$(stat -c %s "$cached")
  else
    if ! checksum_line=$(download_and_verify "$rom_url" "$rom_file"); then
      return 0
    fi
    read -r sha256 size <<<"$checksum_line"
  fi

  if ! build_cabinet "$available_version" "$latest_ymd" "$rom_file" "$sha256" "$size" "$label"; then
    log "keeping previous metadata"
    return 0
  fi

  cleanup_stale_cabs "$cab_name"
  cleanup_stale_roms "$rom_file"
  log "published ${cab_name} (${label:-${latest_ymd}}); installed ${INSTALLED_YMD}"
}

main "$@"
