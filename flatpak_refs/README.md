# Pixel Blue default Flatpak applications

Each file in this directory contains one Flatpak ref. The set is downloaded from
Flathub at ISO build time, embedded in the ISO, and deployed to
`/var/lib/flatpak` on the target machine during installation — no network
needed on the target machine.

Consumed by:

- CI: `.github/workflows/build-disk.yml` (`build-iso` job) passes this directory
  to `jasonn3/build-container-installer` via `flatpak_remote_refs_dir`.
- Local builds: `just build-iso`.

## Rules

- One ref per file: `app/<APP_ID>/x86_64/stable` (or `runtime/...` for runtimes).
- File names are free-form; `README.md` and `Makefile` are ignored by the
  collector.
- Verify an app exists on Flathub before adding it — the authoritative check is
  the ref itself (HTTP 200 = installable):
  `https://dl.flathub.org/repo/refs/heads/<ref>`
- The list mirrors what the official Fedora Silverblue ISO preinstalls (see
  `flatpak_remote_refs` in pungi-fedora), trimmed for this image.

## Deliberate exclusions

- **org.gnome.Evince** — the base image ships its successor *Papers* as an RPM.
- Anything that already exists in the base image as an RPM (Nautilus, Epiphany,
  Totem, …) — preinstalling both an RPM and a Flatpak of the same app causes
  duplicate launchers and default-app confusion (the classic dual-Firefox
  problem).

## Adding / removing apps

Add or delete a file here. The change takes effect on the next ISO build; the
installed set is synchronized on first boot (`flatpak preinstall` semantics),
and users who uninstall an app will not get it reinstalled.
