set dotenv-filename := "image-template.env"
set dotenv-load

export image_name := env_var("IMAGE_NAME")
export repo_organization := env_var("REPO_ORGANIZATION")
export image_desc := env_var("IMAGE_DESC")
export image_keywords := env_var("IMAGE_KEYWORDS")
export image_logo_url := env_var("IMAGE_LOGO_URL")
export default_tag := env_var("DEFAULT_TAG")
export bib_image := env_var("BIB_IMAGE")
export fedora_version := env_var("FEDORA_VERSION")

alias build-vm := build-qcow2
alias rebuild-vm := rebuild-qcow2
alias run-vm := run-vm-qcow2

[private]
default:
    @just --list

# Check Just Syntax
[group('Just')]
check:
    #!/usr/bin/env bash
    find . -type f -name "*.just" | while read -r file; do
    	echo "Checking syntax: $file"
    	just --unstable --fmt --check -f $file
    done
    echo "Checking syntax: Justfile"
    just --unstable --fmt --check -f Justfile

# Fix Just Syntax
[group('Just')]
fix:
    #!/usr/bin/env bash
    find . -type f -name "*.just" | while read -r file; do
    	echo "Checking syntax: $file"
    	just --unstable --fmt -f $file
    done
    echo "Checking syntax: Justfile"
    just --unstable --fmt -f Justfile || { exit 1; }

# Clean Repo
[group('Utility')]
clean:
    #!/usr/bin/env bash
    set -eoux pipefail
    touch _build
    find *_build* -exec rm -rf {} \;
    rm -f previous.manifest.json
    rm -f changelog.md
    rm -f output.env
    rm -rf output/
    rm -rf build/

# Sudo Clean Repo
[group('Utility')]
[private]
sudo-clean:
    just sudoif just clean

# sudoif bash function
[group('Utility')]
[private]
sudoif command *args:
    #!/usr/bin/env bash
    function sudoif(){
        if [[ "${UID}" -eq 0 ]]; then
            "$@"
        elif [[ "$(command -v sudo)" && -n "${SSH_ASKPASS:-}" ]] && [[ -n "${DISPLAY:-}" || -n "${WAYLAND_DISPLAY:-}" ]]; then
            sudo --askpass "$@" || exit 1
        elif [[ "$(command -v sudo)" ]]; then
            sudo "$@" || exit 1
        else
            exit 1
        fi
    }
    sudoif {{ command }} {{ args }}

# The version label is <UTC date>.<build number> (for example 20261024.42): in CI
# that build number is the GitHub Actions run number, locally it falls back to
# BUILD_NUMBER or the commit count. The commit itself goes to
# org.opencontainers.image.revision.

# Build the image using the specified parameters
build $target_image=image_name $tag=default_tag:
    #!/usr/bin/env bash

    set -euox pipefail

    IMAGE_VERSION="${IMAGE_VERSION:-$(date -u +%Y%m%d).${BUILD_NUMBER:-$(git rev-list --count HEAD)}}"

    LABELS=()
    if [[ -z "$(git status -s)" ]]; then
        GIT_SHA=$(git rev-parse --short HEAD)
        LABELS+=("--label" "io.artifacthub.package.readme-url=https://raw.githubusercontent.com/{{ repo_organization }}/{{ image_name }}/${GIT_SHA}/README.md")
        LABELS+=("--label" "org.opencontainers.image.documentation=https://raw.githubusercontent.com/{{ repo_organization }}/{{ image_name }}/${GIT_SHA}/README.md")
        LABELS+=("--label" "org.opencontainers.image.source=https://github.com/{{ repo_organization }}/{{ image_name }}/blob/${GIT_SHA}/Containerfile")
        LABELS+=("--label" "org.opencontainers.image.url=https://github.com/{{ repo_organization }}/{{ image_name }}/tree/${GIT_SHA}")
        LABELS+=("--label" "org.opencontainers.image.version=${IMAGE_VERSION}")
        LABELS+=("--label" "org.opencontainers.image.revision=${GIT_SHA}")
    fi

    LABELS+=("--label" "io.artifacthub.package.deprecated=false")
    LABELS+=("--label" "io.artifacthub.package.keywords={{ image_keywords }}")
    LABELS+=("--label" "io.artifacthub.package.license=Apache-2.0")
    LABELS+=("--label" "io.artifacthub.package.logo-url={{ image_logo_url }}")
    LABELS+=("--label" "io.artifacthub.package.prerelease=false")
    LABELS+=("--label" "org.opencontainers.image.created=$(date -u +%Y\-%m\-%d\T%H\:%M\:%S\Z)")
    LABELS+=("--label" "org.opencontainers.image.description={{ image_desc }}")
    LABELS+=("--label" "org.opencontainers.image.title={{ image_name }}")
    LABELS+=("--label" "org.opencontainers.image.vendor={{ repo_organization }}")

    PODMAN_BUILD_ARGS=("${LABELS[@]}" --pull=newer --tag "${target_image}:${tag}" --file Containerfile)

    podman build "${PODMAN_BUILD_ARGS[@]}" .

# Split the image for smaller updates (New)!
rechunk $target_image=image_name $tag=default_tag:
    #!/usr/bin/env bash

    set -xeuo pipefail

    # TODO: pin chunkah image to hash once mature enough
    CHUNKAH_CONFIG_FILE="$(mktemp)"

    # Not /tmp: a chunkah run makes a complete copy of the image and can outgrow
    # the scratch space there
    CHUNKAH_OUTPUT_DIR="$(mktemp -d ./"${target_image}"_chunkah_XXXXXX)"

    trap 'rm -f "${CHUNKAH_CONFIG_FILE}"; rm -rf "${CHUNKAH_OUTPUT_DIR}"' EXIT
    podman inspect "${target_image}:${tag}" > "${CHUNKAH_CONFIG_FILE}"

    podman run --rm \
      --mount=type=image,src="${target_image}:${tag}",target=/chunkah \
      -v "${CHUNKAH_CONFIG_FILE}:/chunkah-config.json:ro,Z" \
      -v "${CHUNKAH_OUTPUT_DIR}:/run/out:Z" \
      quay.io/coreos/chunkah:latest \
      build \
      --verbose \
      --compressed \
      --max-layers 128 \
      --prune /sysroot/ \
      --label ostree.commit- --label ostree.final-diffid- \
      --config /chunkah-config.json \
      --output oci:/run/out/chunked

    CHUNKED_IMAGE="$(podman pull "oci:${CHUNKAH_OUTPUT_DIR}/chunked")"
    podman tag "${CHUNKED_IMAGE}" "${target_image}:${tag}"

# The chunker re-exports the whole tree through an ostree commit, and the
# initramfs that ships in the chunked image is the base kernel-layer one again
# even though 60-branding-dracut.sh regenerated it (issue #75: the boot/GDM-login
# splash read the stale initrd copy, regardless of the final /usr artwork). So
# after rechunking, regenerate the initramfs once more on the finished image as
# one derived layer, asserting the baked watermark byte-matches the live /usr
# copy (the pixel-blue-logos RPM installed in 10-logos.sh owns that path after
# the swap; the check itself is payload-agnostic). The dracut flags and their
# reasoning are documented in 60-branding-dracut.sh.

# Split the image for smaller updates (Classical)!
ostree-rechunk $target_image=image_name $tag=default_tag:
    #!/usr/bin/env bash

    set -xeuo pipefail

    IMAGE_VERSION="${IMAGE_VERSION:-$(date -u +%Y%m%d).${BUILD_NUMBER:-$(git rev-list --count HEAD)}}"

    RPM_OSTREE_CHUNKER_IMAGE="localhost/${target_image}:${tag}"

    GRAPHROOT="$(podman info --format '{{ '{{.Store.GraphRoot}}' }}')"

    podman run --rm --pull=never --privileged \
      --mount=type=image,src="${target_image}:${tag}",target=/rpm-ostree \
      --mount=type=bind,src=${GRAPHROOT},target=/run/host-container-storage,rw \
      --mount=type=tmpfs,target=/run/rpm-ostree-storage \
      --entrypoint /usr/bin/rpm-ostree \
      "${RPM_OSTREE_CHUNKER_IMAGE}" \
      compose build-chunked-oci \
      --max-layers 127 \
      --format-version=2 \
      --bootc \
      --rootfs /rpm-ostree \
      --output "containers-storage:[overlay@/run/host-container-storage+/run/rpm-ostree-storage]localhost/${target_image}:${tag}"

    # Regenerate the initramfs on the rechunked image, as the recipe doc says
    # above. Appending this as a derived layer keeps the chunked layering (and
    # thus delta updates) intact.
    # Re-apply the build metadata labels on the FINAL image: the chunker
    # re-exports from the ostree commit and does not carry over the labels set
    # by `just build`, so they must be re-stamped here (observed on GHCR: the
    # version label was null).
    LABELS=()
    if [[ -z "$(git status -s)" ]]; then
        GIT_SHA=$(git rev-parse --short HEAD)
        LABELS+=("--label" "io.artifacthub.package.readme-url=https://raw.githubusercontent.com/{{ repo_organization }}/{{ image_name }}/${GIT_SHA}/README.md")
        LABELS+=("--label" "org.opencontainers.image.documentation=https://raw.githubusercontent.com/{{ repo_organization }}/{{ image_name }}/${GIT_SHA}/README.md")
        LABELS+=("--label" "org.opencontainers.image.source=https://github.com/{{ repo_organization }}/{{ image_name }}/blob/${GIT_SHA}/Containerfile")
        LABELS+=("--label" "org.opencontainers.image.url=https://github.com/{{ repo_organization }}/{{ image_name }}/tree/${GIT_SHA}")
        LABELS+=("--label" "org.opencontainers.image.version=${IMAGE_VERSION}")
        LABELS+=("--label" "org.opencontainers.image.revision=${GIT_SHA}")
    fi
    LABELS+=("--label" "io.artifacthub.package.deprecated=false")
    LABELS+=("--label" "io.artifacthub.package.keywords={{ image_keywords }}")
    LABELS+=("--label" "io.artifacthub.package.license=Apache-2.0")
    LABELS+=("--label" "io.artifacthub.package.logo-url={{ image_logo_url }}")
    LABELS+=("--label" "io.artifacthub.package.prerelease=false")
    LABELS+=("--label" "org.opencontainers.image.created=$(date -u +%Y\-%m\-%d\T%H\:%M\:%S\Z)")
    LABELS+=("--label" "org.opencontainers.image.description={{ image_desc }}")
    LABELS+=("--label" "org.opencontainers.image.title={{ image_name }}")
    LABELS+=("--label" "org.opencontainers.image.vendor={{ repo_organization }}")

    SPLASH_CONTAINERFILE="$(mktemp)"
    # (written with printf, not a heredoc: `just --fmt` requires every recipe
    # line to be indented, which a terminating heredoc marker cannot be)
    printf '%s\n' \
      'ARG BASE' \
      'FROM ${BASE}' \
      'RUN --mount=type=tmpfs,dst=/tmp mkdir -p /var/roothome && chmod 700 /var/roothome && \' \
      '    for kdir in /usr/lib/modules/*; do \' \
      '        [ -f "$kdir/modules.dep" ] || continue; \' \
      '        dracut --no-hostonly --add ostree --force "$kdir/initramfs.img" "$(basename "$kdir")" || exit 1; \' \
      '        chmod 0600 "$kdir/initramfs.img"; \' \
      '        lsinitrd "$kdir/initramfs.img" > /tmp/lsinitrd.list; \' \
      '        grep -q "usr/lib/ostree/ostree-prepare-root" /tmp/lsinitrd.list || exit 1; \' \
      '    done && \' \
      '    wm=/usr/share/plymouth/themes/spinner/watermark.png && \' \
      '    want=$(sha256sum "$wm" | cut -d" " -f1) && \' \
      '    ok=0 && \' \
      '    for img in /usr/lib/modules/*/initramfs.img; do \' \
      '        lsinitrd "$img" > /tmp/lsinitrd.list; \' \
      '        grep -q "themes/spinner/watermark.png" /tmp/lsinitrd.list || continue; \' \
      '        lsinitrd -f "$wm" "$img" > /tmp/wm_baked.png; \' \
      '        got=$(sha256sum /tmp/wm_baked.png | cut -d" " -f1); \' \
      '        [ "$got" = "$want" ] && ok=1; \' \
      '    done; \' \
      '    if [ "$ok" != 1 ]; then echo "plymouth theme files in initrd:"; \' \
      '        for img in /usr/lib/modules/*/initramfs.img; do lsinitrd "$img" | grep "themes/"; done; \' \
      '        exit 1; fi; \' \
      '    [ "$ok" = 1 ]' \
      > "${SPLASH_CONTAINERFILE}"
    podman build --pull=never --layers \
      "${LABELS[@]}" \
      --build-arg BASE="localhost/${target_image}:${tag}" \
      --tag "${target_image}:${tag}" \
      --file "${SPLASH_CONTAINERFILE}" .
    rm -f "${SPLASH_CONTAINERFILE}"

# Generate Default Tag
[group('Utility')]
generate-default-tag $tag=default_tag:
    #!/usr/bin/env bash
    set -eoux pipefail

    echo "${tag}"

# Generate Tags
[group('Utility')]
generate-build-tags $target_image=image_name $tag=default_tag:
    #!/usr/bin/env bash
    set -eoux pipefail

    IMAGE_VERSION="${IMAGE_VERSION:-$(date -u +%Y%m%d).${BUILD_NUMBER:-$(git rev-list --count HEAD)}}"

    BUILD_TAGS=()

    # Version-based pin tag, used to freeze a specific build
    if [[ -z "$(git status -s)" ]]; then
        BUILD_TAGS+=("${IMAGE_VERSION}")
    fi

    # Moving channel tag (latest or testing)
    BUILD_TAGS+=("${tag}")

    echo "${BUILD_TAGS[@]}"

# Tag Images
[group('Utility')]
tag-images $target_image=image_name $tag=default_tag tags="":
    #!/usr/bin/env bash
    set -eoux pipefail

    IMAGE=$(podman inspect ${target_image}:${tag} | jq -r .[].Id)
    podman untag ${IMAGE}

    for tag in {{ tags }}; do
        podman tag $IMAGE "${target_image}:${tag}"
    done

    podman images

# Image Name
[group('Utility')]
[private]
image_name $target_image=image_name:
    #!/usr/bin/env bash
    set -eoux pipefail

    echo "${image_name}"

# Load the image into rootful podman: copy it out of the user store when it is
# already there, otherwise pull it from the remote repository. No-op when the
# recipe is already running as root or under sudo.
_rootful_load_image $target_image=image_name $tag=default_tag:
    #!/usr/bin/env bash
    set -eoux pipefail

    if [[ -n "${SUDO_USER:-}" || "${UID}" -eq "0" ]]; then
        echo "Already root or running under sudo, no need to load image from user podman."
        exit 0
    fi

    set +e
    resolved_tag=$(podman inspect -t image "${target_image}:${tag}" | jq -r '.[].RepoTags.[0]')
    return_code=$?
    set -e

    USER_IMG_ID=$(podman images --filter reference="${target_image}:${tag}" --format "'{{ '{{.ID}}' }}'")

    if [[ $return_code -eq 0 ]]; then
        ID=$(just sudoif podman images --filter reference="${target_image}:${tag}" --format "'{{ '{{.ID}}' }}'")
        if [[ "$ID" != "$USER_IMG_ID" ]]; then
            COPYTMP=$(mktemp -p "${PWD}" -d -t _build_podman_scp.XXXXXXXXXX)
            just sudoif TMPDIR=${COPYTMP} podman image scp ${UID}@localhost::"${target_image}:${tag}" root@localhost::"${target_image}:${tag}"
            rm -rf "${COPYTMP}"
        fi
    else
        just sudoif podman pull "${target_image}:${tag}"
    fi

# Build a bootc bootable image using Bootc Image Builder (BIB)
_build-bib $target_image $tag $type $config: (_rootful_load_image target_image tag)
    #!/usr/bin/env bash
    set -euo pipefail

    args="--type ${type} "
    args+="--use-librepo=True "
    args+="--rootfs=btrfs"

    BUILDTMP=$(mktemp -p "${PWD}" -d -t _build-bib.XXXXXXXXXX)

    sudo podman run \
      --rm \
      -it \
      --privileged \
      --pull=newer \
      --net=host \
      --security-opt label=type:unconfined_t \
      -v $(pwd)/${config}:/config.toml:ro \
      -v $BUILDTMP:/output \
      -v /var/lib/containers/storage:/var/lib/containers/storage \
      "${bib_image}" \
      ${args} \
      "${target_image}:${tag}"

    mkdir -p output
    sudo mv -f $BUILDTMP/* output/
    sudo rmdir $BUILDTMP
    sudo chown -R $USER:$USER output/

# Podman builds the image from the Containerfile and creates a bootable image
_rebuild-bib $target_image $tag $type $config: (build target_image tag) && (_build-bib target_image tag type config)

# Build a QCOW2 virtual machine image
[group('Build Virtal Machine Image')]
build-qcow2 $target_image=("localhost/" + image_name) $tag=default_tag: && (_build-bib target_image tag "qcow2" "disk_config/disk.toml")

# Build a RAW virtual machine image
[group('Build Virtal Machine Image')]
build-raw $target_image=("localhost/" + image_name) $tag=default_tag: && (_build-bib target_image tag "raw" "disk_config/disk.toml")

# The ISO boots the graphical Anaconda installer and installs the published
# Pixel Blue image from an OCI layout on the media, plus the Flatpaks in
# flatpak_refs/ out of an ostree repo also on the media -- both offline, both
# before the first boot. There is no live desktop: this is an installer.
#
# $payload_image is the published reference: it is what gets installed and what
# the installed system follows for updates. The rationale for building with
# lorax is in iso/build-iso.sh.

# Build the install ISO with lorax
[group('Build Virtal Machine Image')]
build-iso $payload_image=("ghcr.io/" + repo_organization + "/" + image_name + ":" + default_tag):
    #!/usr/bin/env bash
    set -euox pipefail

    mkdir -p build
    out="$(mktemp -d -p "${PWD}" -t _build_iso.XXXXXXXXXX)"
    trap 'rm -rf "${out}"' EXIT

    sudo podman run --rm --privileged \
      -v "${PWD}:/src:ro" \
      -v "${out}:/out" \
      -e PAYLOAD_REF="{{ payload_image }}" \
      -e PAYLOAD_TARGET_REF="{{ payload_image }}" \
      -e VERSION="{{ fedora_version }}" \
      -e RELEASE="1" \
      "registry.fedoraproject.org/fedora:{{ fedora_version }}" \
      bash /src/iso/build-iso.sh

    sudo chown "$(id -u):$(id -g)" "${out}/pixel-blue.iso"
    mv -f "${out}/pixel-blue.iso" build/pixel-blue.iso

# Rebuild the install ISO (rebuilds the container image first)
[group('Build Virtal Machine Image')]
rebuild-iso $payload_image=("ghcr.io/" + repo_organization + "/" + image_name + ":" + default_tag): build && (build-iso payload_image)

# Rebuild a QCOW2 virtual machine image
[group('Build Virtal Machine Image')]
rebuild-qcow2 $target_image=("localhost/" + image_name) $tag=default_tag: && (_rebuild-bib target_image tag "qcow2" "disk_config/disk.toml")

# Rebuild a RAW virtual machine image
[group('Build Virtal Machine Image')]
rebuild-raw $target_image=("localhost/" + image_name) $tag=default_tag: && (_rebuild-bib target_image tag "raw" "disk_config/disk.toml")

# Run a virtual machine with the specified image type
_run-vm $target_image $tag $type:
    #!/usr/bin/env bash
    set -eoux pipefail

    image_file="output/${type}/disk.${type}"
    if [[ $type == iso ]]; then
        image_file="build/pixel-blue.iso"
    fi

    if [[ ! -f "${image_file}" ]]; then
        just "build-${type}" "$target_image" "$tag"
    fi

    port=8006
    while grep -q :${port} <<< $(ss -tunalp); do
        port=$(( port + 1 ))
    done
    echo "Using Port: ${port}"
    echo "Connect to http://localhost:${port}"

    run_args=()
    run_args+=(--rm --privileged)
    run_args+=(--pull=newer)
    run_args+=(--publish "127.0.0.1:${port}:8006")
    run_args+=(--env "CPU_CORES=4")
    run_args+=(--env "RAM_SIZE=8G")
    run_args+=(--env "DISK_SIZE=64G")
    run_args+=(--env "TPM=Y")
    run_args+=(--env "GPU=Y")
    run_args+=(--device=/dev/kvm)
    run_args+=(--volume "${PWD}/${image_file}":"/boot.${type}")
    run_args+=(docker.io/qemux/qemu)

    (sleep 30 && xdg-open http://localhost:"$port") &
    podman run "${run_args[@]}"

# Run a virtual machine from a QCOW2 image
[group('Run Virtal Machine')]
run-vm-qcow2 $target_image=("localhost/" + image_name) $tag=default_tag: && (_run-vm target_image tag "qcow2")

# Run a virtual machine from a RAW image
[group('Run Virtal Machine')]
run-vm-raw $target_image=("localhost/" + image_name) $tag=default_tag: && (_run-vm target_image tag "raw")

# Run a virtual machine from an ISO
[group('Run Virtal Machine')]
run-vm-iso $target_image=("localhost/" + image_name) $tag=default_tag: && (_run-vm target_image tag "iso")

# Run a virtual machine using systemd-vmspawn
[group('Run Virtal Machine')]
spawn-vm rebuild="0" type="qcow2" ram="6G":
    #!/usr/bin/env bash

    set -euo pipefail

    [ "{{ rebuild }}" -eq 1 ] && echo "Rebuilding the ISO" && just build-vm {{ rebuild }} {{ type }}

    systemd-vmspawn \
      -M "bootc-image" \
      --console=gui \
      --cpus=2 \
      --ram=$(echo {{ ram }}| numfmt --from=iec) \
      --network-user-mode \
      --vsock=false --pass-ssh-key=false \
      -i ./output/**/*.{{ type }}

# Runs shell check on all Bash scripts
lint:
    #!/usr/bin/env bash
    set -eoux pipefail
    if ! command -v shellcheck &> /dev/null; then
        echo "shellcheck could not be found. Please install it."
        exit 1
    fi
    find . -iname "*.sh" -type f -exec shellcheck "{}" ';'

# Runs shfmt on all Bash scripts
format:
    #!/usr/bin/env bash
    set -eoux pipefail
    if ! command -v shfmt &> /dev/null; then
        echo "shfmt could not be found. Please install it."
        exit 1
    fi
    find . -iname "*.sh" -type f -exec shfmt --write "{}" ';'
