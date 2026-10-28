%post --erroronfail --nochroot --interpreter=/usr/bin/bash
set -euxo pipefail

# The interactive-defaults bootc command deploys the payload embedded in the
# ISO from containers-storage and records the registry reference itself as
# the deployment origin (--target-imgref). Verify both here so a silent
# network fallback or a mis-set origin fails the install loudly instead of
# bricking later bootc upgrades. This replaces the manual origin rewrite the
# old ostreecontainer flow needed.
#
# This runs --nochroot, so the live environment (podman store) and the
# physical install root (/mnt/sysimage, with its ostree repo) are both
# directly visible.

# 1. The offline payload really is in this environment's container storage.
podman image exists "@PAYLOAD_IMAGE@"

# 2. Every deployed bootc deployment must reference the payload image. Accept
#    the signed and unverified prefixes: what the bootc installer records
#    depends on its signature-verification path, and upgrade behavior differs
#    accordingly, but the image reference itself must match exactly.
mapfile -t origins < <(find /mnt/sysimage/ostree/deploy -path '*/deploy/*.origin' -type f)
if [ "${#origins[@]}" -eq 0 ]; then
  echo "error: no bootc deployment origin found" >&2
  exit 1
fi
for origin in "${origins[@]}"; do
  if ! grep -Eq '^container-image-reference=ostree-image-(signed|unverified):docker://@PAYLOAD_IMAGE@$' "$origin"; then
    echo "error: $origin does not reference @PAYLOAD_IMAGE@:" >&2
    cat "$origin" >&2
    exit 1
  fi

  # The (un)verified origin makes every bootc upgrade check the payload
  # against the host's /etc/containers config. A payload image published
  # before the matchRepository/registries.d change still carries the old
  # config, which would fail that check forever. Seed the live layer's
  # current files into the deployment: libostree initializes a deployment's
  # /etc (the origin file's <rev>.<serial> directory) at deploy time, and
  # keeps these local edits across upgrades until the image ships matching
  # files itself.
  deploy_dir="$(dirname "$origin")/$(basename "$origin" .origin)"
  install -Dm644 /etc/containers/policy.json \
    "${deploy_dir}/etc/containers/policy.json"
  install -Dm644 /etc/containers/registries.d/pixel-blue.yaml \
    "${deploy_dir}/etc/containers/registries.d/pixel-blue.yaml"
done
%end
