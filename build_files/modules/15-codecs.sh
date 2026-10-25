#!/bin/bash
# Multimedia codecs and hardware video acceleration.

set -ouex pipefail

# Full ffmpeg replaces the stripped-down ffmpeg-free shipped by Fedora; the
# swap is the method recommended by RPM Fusion. --allowerasing is required:
# ffmpeg-free pulls in the *-free library subpackages (libswscale-free,
# libavcodec-free, ...) which file-conflict with the complete ffmpeg-libs, and
# swap alone will not remove them. The gstreamer1-plugins-* packages provide
# H.264/H.265 and other restricted codecs to GTK/GStreamer based apps,
# including the GNOME video decoders. gstreamer1-vaapi wires those decoders
# into VA-API.
dnf5 swap -y ffmpeg-free ffmpeg --allowerasing --enablerepo=rpmfusion-free
dnf5 install -y \
  gstreamer1-plugins-good \
  gstreamer1-plugins-bad-free \
  gstreamer1-plugins-bad-freeworld \
  gstreamer1-plugins-ugly-free \
  gstreamer1-plugins-ugly \
  gstreamer1-plugin-libav \
  gstreamer1-vaapi

# Intel VA-API user-space driver for the Pixelbook (eve) Kaby Lake iGPU.
# intel-media-driver (iHD) is the actively maintained driver and provides the
# full decode/encode set: H.264, HEVC 8/10-bit and VP9 8/10-bit. Fedora's
# libva-intel-media-driver build (a mandatory @multimedia package in the base
# image) has the patented codecs stripped, so swap it for the complete one
# from RPM Fusion nonfree (fedy and other distro tools use the same swap).
# libva-utils ships vainfo for verification.
#
# Note: we deliberately do NOT swap mesa-va-drivers for the RPM Fusion
# -freeworld build. Intel GPUs do not use Mesa for video acceleration (Mesa VA
# drivers only serve AMD/nouveau); the freeworld mesa swap is what gives AMD
# cards H.264/H.265 decode. On this Intel Pixelbook it provides no benefit, and
# it is fragile (its mesa version frequently lags Fedora's updates, breaking the
# mesa-dri-drivers/libgallium version pins) plus it is a documented cause of
# GNOME login-screen crashes. iHD above already handles all Intel HW decode.
dnf5 swap -y libva-intel-media-driver intel-media-driver --enablerepo=rpmfusion-nonfree
dnf5 install -y libva-utils
