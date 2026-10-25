#!/bin/bash
# Enable system services

set -ouex pipefail

systemctl enable keyd
systemctl enable podman.socket
