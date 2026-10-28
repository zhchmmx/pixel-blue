%post --erroronfail --nochroot --interpreter=/usr/bin/bash
set -euxo pipefail

# The live image already carries the Flathub apps in /var/lib/flatpak (the
# ISO layer installs them from flatpak_refs/). Copy that system installation
# into the target deployment so first boot has Bazaar/Firmware/etc. without
# downloading them again.
#
# The deployment checkout is found through its .origin file (under
# /mnt/sysimage/ostree/deploy/<os>/deploy/<rev>.<serial>/) rather than a
# hardcoded ostree/0/1/0 ref: the bootc installer lays the deployment out the
# same way, but its ref numbering must not be assumed here.
if [ ! -d /var/lib/flatpak ]; then
  echo "error: /var/lib/flatpak missing from the live image" >&2
  exit 1
fi

mapfile -t origins < <(find /mnt/sysimage/ostree/deploy -path '*/deploy/*.origin' -type f)
if [ "${#origins[@]}" -eq 0 ]; then
  echo "error: no bootc deployment found" >&2
  exit 1
fi
for origin in "${origins[@]}"; do
  deploy_dir="$(dirname "${origin}")/$(basename "${origin}" .origin)"
  rsync -aAXUHKP --filter='-x security.selinux' /var/lib/flatpak "${deploy_dir}/var/lib/"
done
%end
