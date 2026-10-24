#!/bin/bash
# Clean up dnf cache to reduce image size

set -ouex pipefail

dnf5 clean all
