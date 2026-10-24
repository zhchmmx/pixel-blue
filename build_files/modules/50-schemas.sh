#!/bin/bash
# GNOME Shell default dash favorites and extension defaults
#
# Both are pinned in
# system_files/usr/share/glib-2.0/schemas/zz0-pixel-blue-modifications.gschema.override,
# which the `cp -avf /ctx/system_files/. /` in 00-system-files.sh places in
# /usr/share/glib-2.0/schemas/. Schemas must be recompiled for the override to
# take effect, so do it after all package operations.
#
# The tablet-mode keyboard backlight extension (issue #72) ships its payload in
# system_files/usr/share/gnome-shell/extensions/. System extensions are loaded
# but stay disabled until their uuid appears in org.gnome.shell
# enabled-extensions, so the override states that key in full: override files
# replace whole keys and the zz0-* file sorts last, so it wins over Fedora's
# org.gnome.shell.gschema.override (fedora-release-identity-silverblue) instead
# of merging with it. Fedora's F44 value only lists
# background-logo@fedorahosted.org, the extension removed in 30-drop-firefox.sh,
# so there is nothing to inherit; following a future Fedora addition is a
# deliberate edit in that file, not build-time detection.
#
# Note: this only changes the default for users who never customized their own
# favorites or extensions; it does not override a user's existing dconf value.

set -ouex pipefail

glib-compile-schemas /usr/share/glib-2.0/schemas
