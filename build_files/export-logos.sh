#!/bin/bash
# Regenerate every raster branding asset from the SVG masters in
# system_files/usr/share/pixel-blue/logos/ (issue #54). Requires cairosvg
# (`pip install cairosvg`) and runs against the repo checkout, not the image.
#
#   ./build_files/export-logos.sh
#
# PNG consumers that cannot read the SVG masters:
#   - plymouth initramfs renderer (PNG only): spinner/watermark.png
#   - forced-white consumers (dark surfaces): pixmaps/system-logo-white.png
#   - hicolor PNG ladder (legacy/low-DPI icon fallback): icons/hicolor/NNxNN/apps
#   - ArtifactHub logo (IMAGE_LOGO_URL in image-template.env): pixel-blue.png
#   - Anaconda installer sidebar (lorax runtime, see installer-branding.tmpl):
#     anaconda/pixmaps/sidebar-logo.png

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."
LOGOS=system_files/usr/share/pixel-blue/logos
ICON=system_files/usr/share/icons/hicolor/scalable/apps/pixel-blue-logo-icon.svg

# Boot splash watermark: white mono symbol, Fedora's spinner default size.
mkdir -p system_files/usr/share/plymouth/themes/spinner
cairosvg "$LOGOS/pixel-blue-symbol-white.svg" \
  -o system_files/usr/share/plymouth/themes/spinner/watermark.png \
  --output-width 400 --output-height 400

# pixmaps white logo (system-logo-white.png), fedora-logos-compatible size.
mkdir -p system_files/usr/share/pixmaps
cairosvg "$LOGOS/pixel-blue-symbol-white.svg" \
  -o system_files/usr/share/pixmaps/system-logo-white.png \
  --output-width 256 --output-height 256

# Anaconda installer sidebar logo (the Silverblue profile's stylesheet puts
# the .logo slot at the bottom of a blue sidebar; fedora-logos ships the
# white Fedora mark there at 112x97, so export the white symbol square).
# Installed into the ISO's installer environment by
# build_files/installer-branding.tmpl, NOT by the container image; this
# path is just the regeneration home.
mkdir -p system_files/usr/share/anaconda/pixmaps
cairosvg "$LOGOS/pixel-blue-symbol-white.svg" \
  -o system_files/usr/share/anaconda/pixmaps/sidebar-logo.png \
  --output-width 128 --output-height 128

# hicolor PNG ladder mirroring fedora-logos' sizes.
for s in 16 22 24 32 36 48 96 256; do
  mkdir -p "system_files/usr/share/icons/hicolor/${s}x${s}/apps"
  cairosvg "$ICON" \
    -o "system_files/usr/share/icons/hicolor/${s}x${s}/apps/pixel-blue-logo-icon.png" \
    --output-width "$s" --output-height "$s"
done

# ArtifactHub profile logo, 512x512 per their image requirements.
cairosvg "$LOGOS/pixel-blue-symbol.svg" -o pixel-blue.png --output-width 512 --output-height 512

echo "exported; verify visually before committing (e.g. watermark on black)."
