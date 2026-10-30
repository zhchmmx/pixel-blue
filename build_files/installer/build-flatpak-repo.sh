#!/usr/bin/bash
# Build the offline Flatpak source that Anaconda installs from.
#
# pyanaconda's FlatpakManager looks for a flatpak ostree repo at the hardcoded
# path /flatpak/repo, installs every app/ ref found there into the target's
# /var/lib/flatpak during the installation, and skips runtimes (they come in as
# dependencies). See pyanaconda/modules/payloads/payload/rpm_ostree/
# flatpak_manager.py:
#
#     LOCAL_REMOTE_NAME = "Anaconda"
#     LOCAL_REMOTE_PATH = "file:///flatpak/repo"
#
# This script turns flatpak_refs/ into exactly that repo. The export steps
# (archive-z2 + pull-local + build-update-repo) follow ublue's
# build-container-installer, which is the reference implementation of this
# flow: a plain `flatpak install` tree is not readable by Anaconda, it wants an
# ostree repo with a summary.

set -euxo pipefail

REPO=/flatpak/repo
WORK=/tmp/flatpak-build

# Keep the install tree out of /var/lib/flatpak: Anaconda creates that on the
# target itself, so an installation here would only duplicate every commit.
export FLATPAK_SYSTEM_DIR="${WORK}/flatpak"
export FLATPAK_TRIGGERSDIR="${WORK}/triggers"

mkdir -p "${FLATPAK_SYSTEM_DIR}" "${FLATPAK_TRIGGERSDIR}" "${REPO}" /var/tmp
chmod -R 1777 /var/tmp

# Flatpak's bwrap sandbox writes /proc/sys/user/max_user_namespaces, read-only
# by default in a build container. --cap-add sys_admin makes this work.
mount -o remount,rw /proc/sys || true

flatpak config --system --set languages "*"
flatpak remote-add --system --if-not-exists flathub \
  https://flathub.org/repo/flathub.flatpakrepo

# The ref files are one-per-line (see flatpak_refs/), sort for reproducibility.
xargs -r flatpak install --system -y --noninteractive < <(cat /flatpak_refs/*)

# Export every deployed ref -- apps *and* their runtimes -- into a standalone
# archive repo. Anaconda resolves runtimes as dependencies, so they have to be
# present in the repo even though it only asks for the app/ refs.
ostree init --repo="${REPO}" --mode=archive-z2
while IFS= read -r ref; do
  rev="$(ostree --repo="${FLATPAK_SYSTEM_DIR}/repo" rev-parse "flathub/${ref}")"
  ostree --repo="${REPO}" pull-local "${FLATPAK_SYSTEM_DIR}/repo" "${rev}"
  mkdir -p "$(dirname "${REPO}/refs/heads/${ref}")"
  printf '%s\n' "${rev}" >"${REPO}/refs/heads/${ref}"
done < <(ostree refs --repo="${FLATPAK_SYSTEM_DIR}/repo" |
  sed -n 's|^deploy/||p')

# Writes the summary Anaconda reads through list_remote_refs_sync().
flatpak build-update-repo "${REPO}"

rm -rf "${WORK}" /var/lib/flatpak

# Fail loudly rather than shipping an ISO that silently installs no Flatpaks.
apps="$(ostree refs --repo="${REPO}" | grep -c '^app/' || true)"
if [ "${apps}" -eq 0 ]; then
  echo "error: no app/ refs were exported into ${REPO}" >&2
  exit 1
fi
echo "exported ${apps} app refs into ${REPO}"

# The ref list Anaconda's own code never supplies.
#
# On the bootc/ostree payload, nothing ever calls FlatpakManager.set_flatpak_refs()
# -- the only caller in pyanaconda is the DNF payload (payload/dnf/dnf.py). Without
# refs the manager's install() hits its "no flatpaks are marked for install" guard
# and returns silently, so the installation finishes with nothing installed and no
# log line to show for it. Containerfile.installer patches that task to read this
# file; taking the list from the repo keeps the two from drifting apart.
ostree --repo="${REPO}" refs | grep '^app/' > /flatpak/refs
echo "wrote $(wc -l < /flatpak/refs) refs to /flatpak/refs"
