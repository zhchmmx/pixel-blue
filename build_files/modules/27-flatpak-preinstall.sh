#!/bin/bash
# Flatpak preinstall manifest (native flatpak >= 1.18 mechanism)

set -ouex pipefail

# The live ISO preinstalls the apps listed in flatpak_refs/ offline (rsync of
# /var/lib/flatpak via an Anaconda post-script), which covers ISO installs
# only. Additionally ship every app ref as a preinstall config so the image
# also works for bootc-switched hosts and non-ISO installs:
#   - /etc/flatpak/preinstall.d/pixel-blue-<App>.preinstall
#   - flatpak-preinstall.service performs the first-boot (online) install
# Flatpak >= 1.18 reads these on first boot and in Bazaar's "add / remove apps
# in this OS" view, which then keeps the preinstall list syncable (the list is
# the single source of truth, so .preinstall files are generated from it here
# rather than checked in). Pattern mirrors ublue-os/bluefin (custom/flatpaks/
# default.preinstall -> /etc/flatpak/preinstall.d + systemctl enable). Both
# the files and the unit are inert on hosts whose flatpak predates the feature
# (unit existence check), so images built on older bases stay unaffected.

mkdir -p /etc/flatpak/preinstall.d
find /ctx/flatpak_refs -maxdepth 1 -type f -print0 |
  sort -z |
  xargs -0 cat |
  while read -r ref; do
    case "${ref}" in
    app/*) ;;
    *) continue ;;
    esac
    name="$(printf '%s\n' "${ref}" | cut -d/ -f2)"
    branch="$(printf '%s\n' "${ref}" | cut -d/ -f4)"
    printf '[Flatpak Preinstall %s]\nBranch=%s\n' "${name}" "${branch}" \
      >"/etc/flatpak/preinstall.d/pixel-blue-${name}.preinstall"
  done

if [ -e /usr/lib/systemd/system/flatpak-preinstall.service ]; then
  systemctl enable flatpak-preinstall.service
fi
