#!/bin/bash
# Remove Firefox

set -ouex pipefail


dnf5 remove -y firefox firefox-langpacks
dnf5 remove -y gnome-shell-extension-background-logo
dnf5 remove -y gnome-system-monitor
