#!/bin/bash
# Flatpak repositories

set -ouex pipefail

# Upstream (fedora-silverblue) ships flatpak-add-fedora-repos.service, enabled
# via a systemd preset. On first boot it registers the Fedora flatpak registry
# (oci+https://registry.fedoraproject.org) plus a disabled fedora-testing
# remote. Mask the service so those are never added. The Flathub remote
# shipped at system_files/etc/flatpak/remotes.d/flathub.flatpakrepo (copied to
# /etc/flatpak/remotes.d/ by 00-system-files.sh) is registered by
# flatpak-system-helper instead.
# Note: this only affects fresh deployments of this image; an existing install
# that already added the Fedora remotes keeps them under /var/lib/flatpak.

ln -s /dev/null /etc/systemd/system/flatpak-add-fedora-repos.service

# Issue #120: an ISO-installed system gets a flathub remote that has no GPG key,
# so the first `flatpak update` dies with
# "Failed to load summary from remote flathub: ... No public key (RSA key ID
# 562702E9E3ED7EE8)".
#
# Two upstream behaviours combine into that. Anaconda does not read the
# .flatpakrepo above: pyanaconda's flatpak_manager.add_remote() builds the
# remote from a name and a URL only (Remote.new() + set_url()), so it lands with
# gpg-verify on and an empty keyring. And flatpak then refuses to apply the
# .flatpakrepo after the fact -- _flatpak_dir_find_new_flatpakrepos() skips a
# preconfigured remote whose name is already present -- so the key that *is*
# shipped in that file is never imported.
#
# A remote without its own keyring (var/lib/flatpak/repo/flathub.trustedkeys.gpg)
# makes ostree fall back to the global keyring directory,
# /usr/share/ostree/trusted.gpg.d/ (ostree-repo.c,
# _ostree_repo_gpg_prepare_verifier). So the image ships the Flathub signing key
# there: no flatpak state is touched at runtime, and systems that already have
# the keyless remote are fixed by the next update instead of only fresh installs.
#
# The key must stay byte-identical to the GPGKey of the .flatpakrepo above (both
# are what https://dl.flathub.org/repo/flathub.gpg serves). Checked here so the
# two copies cannot drift apart silently -- the keyring is a binary file and a
# rotated Flathub key that only reached one of them would look fine until the
# first update.
keyring=/usr/share/ostree/trusted.gpg.d/flathub.gpg
descriptor=/etc/flatpak/remotes.d/flathub.flatpakrepo

if ! cmp -s <(sed -n 's/^GPGKey=//p' "${descriptor}" | base64 -d) "${keyring}"; then
  echo "error: ${keyring} is not the GPGKey of ${descriptor}" >&2
  echo "       refresh both from https://dl.flathub.org/repo/flathub.gpg" >&2
  exit 1
fi
