%post --erroronfail --nochroot --interpreter=/usr/bin/bash
set -euxo pipefail

# The live image already carries the Flathub apps in /var/lib/flatpak (the
# ISO layer installs them from flatpak_refs/). Copy that system installation
# into the target deployment so first boot has Bazaar/Firmware/etc. without
# downloading them again.
deployment="$(ostree rev-parse --repo=/mnt/sysimage/ostree/repo ostree/0/1/0)"
target="/mnt/sysimage/ostree/deploy/default/deploy/${deployment}.0/var/lib/"

if [ ! -d /var/lib/flatpak ]; then
  echo "error: /var/lib/flatpak missing from the live image" >&2
  exit 1
fi

mkdir -p "$target"
rsync -aAXUHKP --filter='-x security.selinux' /var/lib/flatpak "$target"
%end
