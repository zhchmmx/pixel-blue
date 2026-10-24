#!/bin/bash
# Unify the OS display name (issue #56)
#
# The installed system would otherwise show the upstream identity
# ("Fedora Linux <version> (Silverblue)" and its NAME/VARIANT parts)
# in gnome-initial-setup, gnome-tour, GNOME Settings -> About and the
# GRUB entry titles. Rewrite the brand keys of /usr/lib/os-release so
# all of them say "Pixel Blue".
#
# Only NAME and PRETTY_NAME are touched: ID, VERSION*, VARIANT*,
# PLATFORM_ID and the URL keys must stay intact, because Flatpak,
# GNOME Software and the Anaconda Silverblue profile detection depend
# on them.
#
# PRETTY_NAME is rewritten wholesale (not by prefix sed, because the
# upstream value is "Fedora Linux <version> (Silverblue)" and a prefix
# swap would leave stray words behind); the version is taken from
# VERSION_ID so the string keeps tracking the base image instead of
# being hardcoded.
#
# This runs last so no dnf5 operation can restore the stock file
# afterwards. bootc symlinks /etc/os-release to /usr/lib/os-release, so
# editing the latter covers both.

set -ouex pipefail

osrel_version_id="$(sed -n 's/^VERSION_ID=//p' /usr/lib/os-release)"
if [ -z "$osrel_version_id" ]; then
    echo "error: could not extract VERSION_ID from /usr/lib/os-release" >&2
    exit 1
fi

sed -i -e 's|^NAME=.*|NAME="Pixel Blue"|' \
       -e "s|^PRETTY_NAME=.*|PRETTY_NAME=\"Pixel Blue ${osrel_version_id}\"|" \
       /usr/lib/os-release

# Fail the build loudly if the rewrite did not land.
grep -q '^NAME="Pixel Blue"$' /usr/lib/os-release
grep -q '^PRETTY_NAME="Pixel Blue ' /usr/lib/os-release
