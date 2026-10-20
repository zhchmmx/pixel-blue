#!/bin/sh
set -e
dir="${XDG_CACHE_HOME:-$HOME/.cache}/gnome-control-center/wacom"
mkdir -p "$dir"
printf '[2d1f:5143]\nStyli=generic;\n' > "$dir/devices"
