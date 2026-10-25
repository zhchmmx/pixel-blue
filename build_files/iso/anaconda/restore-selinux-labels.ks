%post --erroronfail --interpreter=/usr/bin/bash
set -euxo pipefail

# The Flatpak tree is rsynced from the live image without SELinux labels
# (--filter='-x security.selinux'), so relabel it with the installed policy.
setenforce 0 || true
restorecon -R /etc/selinux
restorecon -R /var/lib/flatpak
%end
