# Pixel Blue

This repository is based on Universal Blue's official template and built as a Fedora Silverblue image for Pixelbook.

This repository includes verified solutions from [WeirdTreeThing](https://github.com/WeirdTreeThing) and other respected contributors.

Please note that this is just an experimental project; use it at your own risk.

# How to Use

There are several ways for Pixelbook users to install this image on their device or just switch from another Universal Blue-based OS.

## Option 1: Install Pixel Blue from ISO file

Visit [Pixel Blue Download Page](https://pixel-blue.pages.dev/) to download the latest `.iso` file.

### Building the ISO locally

```bash
just build          # build localhost/pixel-blue:latest
just build-iso      # wraps it into build/pixel-blue.iso with osbuild image-builder
just run-vm-iso     # boot the ISO in QEMU
```

`just rebuild-iso` does the first two steps in one go.

The ISO is an **installer**, not a live session: it boots into the graphical
Anaconda installer, where you pick the target disk and keep the default
`Automatic` layout — the same path a Fedora Silverblue install takes.

It is built with [lorax](https://github.com/weldr/lorax), the tool Fedora and
RHEL use to produce `boot.iso`. That means the installer environment itself —
Anaconda, the SELinux contexts, the logind `ReserveVT` drop-in, the
`anaconda-shell` VT, the dracut modules — comes from lorax, not from this
repository. `iso/build-iso.sh` adds only what is ours, through the templates in
`iso/templates/`:

- the published `pixel-blue` image, exported as an OCI layout and copied onto
  the media; the appended kickstart installs it with
  `ostreecontainer --transport=oci`, then pins it for updates with
  `bootc switch` in a `%post`
- `/flatpak/repo`, an ostree repo built from `flatpak_refs/`. Anaconda installs
  every app in it into the target's `/var/lib/flatpak` **during the
  installation**, so the Flatpaks are there on the first boot — no network, no
  first-boot script
- `iso/pixel-blue.conf`, an Anaconda profile inheriting `fedora-silverblue`,
  selected by appending `VARIANT_ID=pixel-blue` to the installer's os-release

The install stays interactive because the appended kickstart carries no storage
directives: the Installation Destination page asks which disk to use.

## Option 2: Switch to Pixel Blue

Pixel Blue is published in three channels. Pick the one you want and use the matching command.

### Latest channel (default)

`latest` follows the upstream Universal Blue `latest` image and always tracks the newest Fedora.

```bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:latest
```

### Testing channel

`testing` is the preview channel for `latest`. It is built from the same upstream `latest` base, so it only differs by the changes that are being validated before they land in `latest`. Use it if you want to test upcoming changes early.

```bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:testing
```

### Pinning to a specific build

Every build is also published with a version tag, so you can freeze to an exact build:

```bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:20261024.42
```

After that, you may reboot your device to see the changes. Hosts tracking a moving channel (`latest` or `testing`) receive new builds automatically; users pinned to a version tag will not receive updates until they switch back to a channel.

# Suspend and Sleep (S3 vs Modern Standby)

By default this image uses the kernel's `s2idle` (Modern Standby) suspend path,
which keeps most of the platform powered. On the Pixelbook that shows up as fast
lid-close drain — the battery can drop noticeably overnight. ChromeOS itself uses
a deeper suspend, so if you want standby behaviour closer to it, you can opt into
S3 (suspend-to-RAM). This is left as a user choice; the image does not force
either mode.

## Enabling S3 (lower standby drain)

S3 is selected at the firmware level, which cannot be baked into the image:

1. Reboot into the UEFI firmware settings.
2. Disable **Modern Standby (S0ix)**; the system then falls back to S3 (deep).
3. Boot Linux and verify as shown below.

Keep Modern Standby instead if you value instant resume and background
network/notifications over low standby drain — that is a valid trade-off, just
expect higher lid-close power draw.

# Keyboard Layout (ChromeOS-style)

The Pixelbook keyboard is remapped via [keyd](https://github.com/rvaiya/keyd). The mappings are:

## Top row (bare press)

| Physical key                                            | ChromeOS role                | Emits on Pixel Blue                                     |
| ------------------------------------------------------- | ---------------------------- | ------------------------------------------------------- |
| `Esc`                                                   | Cancel                       | `Esc` (native)                                          |
| `Back`                                                  | Back                         | `Back` (native media key)                               |
| `Refresh`                                               | Forward                      | `Refresh` (native media key)                            |
| `Fullscreen` (`zoom`)                                   | Fullscreen                   | `F11` (bare `KEY_ZOOM` does nothing in GNOME)           |
| `Overview` (`scale`)                                    | Show windows / Desks         | `Print` → **screenshot UI** (owner choice; see caveats) |
| `Brightness −` / `+`                                    | Brightness                   | native pass-through                                     |
| `Play/Pause`, `Mute`, `Vol −` / `+`                     | Media                        | native pass-through                                     |
| `Three-lines` (menu key, scancode `0x5d` → `dashboard`) | System menu (Quick Settings) | `Super+S` → **Quick Settings** (GNOME 45+ native)       |

## Search (Meta) held

| Combo                          | Emits                               | Function                                                                                        |
| ------------------------------ | ----------------------------------- | ----------------------------------------------------------------------------------------------- |
| `Search` + `Back`…`Vol +`      | `F1`–`F10`                          | the official top-row F-keys                                                                     |
| `Search` + `Three-lines`       | `F12`                               | F12 (ChromeOS has no F12 key)                                                                   |
| `Search` + `←` / `→`           | bare `Home` / `End`                 | start / end of line (emitted without `Super`, so it is not read as a GNOME window-tiling combo) |
| `Search` + `Shift` + `←` / `→` | `Shift+Home` / `Shift+End`          | select to line start / end                                                                      |
| `Search` + `Ctrl` + `←` / `→`  | `Ctrl+Home` / `Ctrl+End`            | start / end of document                                                                         |
| `Search` + `[` / `]`           | `Super+Page_Up` / `Super+Page_Down` | previous / next workspace                                                                       |
| `Search` + `Shift` + `[` / `]` | `Super+Shift+Page_Up` / `…_Down`    | move window to previous / next workspace                                                        |
| `Search` + `Alt`               | `Caps Lock`                         | Caps Lock (either order)                                                                        |

## Alt held

| Combo                        | Emits              | Function            |
| ---------------------------- | ------------------ | ------------------- |
| `Alt` + `Backspace`          | `Delete`           | forward delete      |
| `Alt` + `Search`             | `Caps Lock`        | Caps Lock           |
| `Alt` + `Brightness −` / `+` | keyboard backlight | native pass-through |

# Modifications to the Upstream

- Add audio support from [WeirdTreeThing](https://github.com/WeirdTreeThing).
- Add keymap via [keyd](https://github.com/rvaiya/keyd).
- Add RPM Fusion and full multimedia codecs with VA-API hardware decoding.
- Improve Pixelbook Pen support.
- Add experimental auto brightness using the Pixelbook (eve) vendor curve.
- Add tablet-mode keyboard backlight (issue #72).
- Add [Waydroid](https://waydro.id/) (Android in a container) from the Fedora repos.

# Learn More

For more information, please email me at [1055773545@qq.com](mailto:1055773545@qq.com).

You can also refer to the template I use, which contains more information about what each file in this repo does.
