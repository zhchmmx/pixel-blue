%post --erroronfail --interpreter=/usr/bin/bash --log=/tmp/restore-selinux-labels.log
set -euxo pipefail

# The copied Flatpak store is relabelled in install-flatpaks.ks: it runs
# setfiles -r with the stateroot as alternate root, using the deployment's
# file_contexts, right after the rsync. All that is left here is the policy
# store under /etc.
setenforce 0 || true
restorecon -R /etc/selinux
%end
