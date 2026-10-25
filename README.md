# Pixel Blue

This repository is based on Universal Blue's official template and built as a Fedora Silverblue image for Pixelbook.

This repository includes verified solutions from [WeirdTreeThing](https://github.com/WeirdTreeThing) and other respected contributors.

Please note that this is just an experimental project; use it at your own risk.

# How to Use

There are several ways for Pixelbook users to install this image on their device or just switch from another Universal Blue-based OS.

## Option 1: Install Pixel Blue from ISO file (Not ready yet.)



## Option 2: Switch to Pixel Blue

Pixel Blue is published in three channels. Pick the one you want and use the matching command.

### Latest channel (default)

`latest` follows the upstream Universal Blue `latest` image and always tracks the newest Fedora.

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:latest
```

### Testing channel

`testing` is the preview channel for `latest`. It is built from the same upstream `latest` base, so it only differs by the changes that are being validated before they land in `latest`. Use it if you want to test upcoming changes early.

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:testing
```


### Pinning to a specific build

Every build is also published with a version tag, so you can freeze to an exact build:

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:20261024.42
```

The tag is `<UTC build date>.<GitHub Actions run number>` (for example `20261024.42`) and contains neither the branch name nor the commit hash. Take the number from the workflow run that built the image; the commit is recorded in the image's `org.opencontainers.image.revision` label.

After that, you may reboot your device to see the changes. Hosts tracking a moving channel (`latest` or `testing`) receive new builds automatically; users pinned to a version tag will not receive updates until they switch back to a channel. See System Updates below for how that works.

> For maintainers: `latest` is built from the `main` branch and `testing` from the `testing` branch. To roll a change out, commit it on `testing`, verify the published `testing` image, then merge `testing` into `main` to promote it to `latest`.

# System Updates

Pixel Blue is a bootc image, so the OS updates atomically as a whole OCI image, never package-by-package. The image ships [uupd](https://github.com/ublue-os/uupd) as the update manager (the replacement for the earlier, reverted rpm-ostree auto-staging and PackageKit experiments):

- A systemd timer (`uupd.timer`) runs once a day at 04:00 plus a random delay of up to 15 minutes. `Persistent=true` means a machine that was off or suspended at that time catches up on the next boot. Each run executes `bootc upgrade --check` and, when a newer build of the tracked channel exists, stages the new deployment.
- Nothing ever reboots on its own. The staged deployment applies on the next reboot of your choice. With GNOME Software replaced by Bazaar (issue #87), there is no store-provided "Update and Restart" power-menu prompt for staged OS updates.
- Gates keep the daily check honest on a laptop: it defers when running on battery below 20%, when power-saver mode is on, when the CPU 5-minute load average exceeds 50 or memory use exceeds 90%, and on metered or busy networks (a docked, charging device passes the battery gate at any level). All thresholds are tunable in `/etc/uupd/config.json`, repo copy in `system_files/etc/uupd/config.json`.
- Only uupd's `system` module is enabled; its `flatpak`, `distrobox` and `brew` modules are disabled because applications are updated through the software store, not the OS timer.
- To check on demand instead of waiting for the timer: `sudo systemctl start uupd-manual.service` (the same run with the hardware gates skipped; `uupd.service` keeps them). To update fully by hand instead: `sudo bootc upgrade` and reboot. Rollback stays available with `sudo bootc rollback`.

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

The Pixelbook keyboard has no Home/End, Page Up/Down, forward Delete, Caps Lock
or dedicated F1–F12 keys. The image uses [keyd](https://github.com/rvaiya/keyd)
so these are reached with the **Search** and **Alt** keys, exactly the way
ChromeOS does it — with one deliberate exception: the page keys (`Alt+↑/↓`)
are left unmapped so apps keep them (see **Alt held**). The combos follow
the official Google shortcut list, and the
keys keyd *emits* are chosen so they do not collide with GNOME's own defaults
(verified against upstream `gsettings-desktop-schemas`). The config lives in
`system_files/etc/keyd/cros.conf` (plus `system_files/usr/lib/udev/hwdb.d/70-pixel-keyboard.hwdb`
for the Three-lines and Assistant keys); this table is the human-readable reference.

Sources: [ChromeOS keyboard shortcuts](https://support.google.com/chromebook/answer/183101) ·
[Use your keyboard / remap keys](https://support.google.com/chromebook/answer/1047364) ·
GNOME defaults in `org.gnome.desktop.wm.keybindings` and `org.gnome.shell.keybindings`.

## Top row (bare press)

| Physical key | ChromeOS role | Emits on Pixel Blue |
| --- | --- | --- |
| `Esc` | Cancel | `Esc` (native) |
| `Back` | Back | `Back` (native media key) |
| `Refresh` | Forward | `Refresh` (native media key) |
| `Fullscreen` (`zoom`) | Fullscreen | `F11` (bare `KEY_ZOOM` does nothing in GNOME) |
| `Overview` (`scale`) | Show windows / Desks | `Print` → **screenshot UI** (owner choice; see caveats) |
| `Brightness −` / `+` | Brightness | native pass-through |
| `Play/Pause`, `Mute`, `Vol −` / `+` | Media | native pass-through |
| `Three-lines` (menu key, scancode `0x5d` → `dashboard`) | System menu (Quick Settings) | `Super+S` → **Quick Settings** (GNOME 45+ native) |

## Search (Meta) held

`Search` is natively `Meta`, so its own combos (`Search` alone, `Search+L` lock,
`Search+A`, `Search+V` …) are GNOME-native and left untouched.

| Combo | Emits | Function |
| --- | --- | --- |
| `Search` + `Back`…`Vol +` | `F1`–`F10` | the official top-row F-keys |
| `Search` + `Three-lines` | `F12` | F12 (ChromeOS has no F12 key) |
| `Search` + `←` / `→` | bare `Home` / `End` | start / end of line (emitted without `Super`, so it is not read as a GNOME window-tiling combo) |
| `Search` + `Shift` + `←` / `→` | `Shift+Home` / `Shift+End` | select to line start / end |
| `Search` + `Ctrl` + `←` / `→` | `Ctrl+Home` / `Ctrl+End` | start / end of document |
| `Search` + `[` / `]` | `Super+Page_Up` / `Super+Page_Down` | previous / next workspace |
| `Search` + `Shift` + `[` / `]` | `Super+Shift+Page_Up` / `…_Down` | move window to previous / next workspace |
| `Search` + `Alt` | `Caps Lock` | Caps Lock (either order) |

## Alt held

| Combo | Emits | Function |
| --- | --- | --- |
| `Alt` + `Backspace` | `Delete` | forward delete |
| `Alt` + `Search` | `Caps Lock` | Caps Lock |
| `Alt` + `Brightness −` / `+` | keyboard backlight | native pass-through |

`Ctrl+Alt+↑/↓` and `Ctrl+Alt+←/→` are **not** remapped — GNOME binds them to workspace
switching, and the owner chose to keep that. Bare `Alt+←/→` is likewise left untouched
(apps use it for back/forward), since ChromeOS does not use it for text navigation.
The official ChromeOS page keys, `Alt+↑/↓`, are deliberately **not** mapped either:
keyd sits below the app layer, so binding them globally would cost GNOME Files its
parent-folder shortcut (`Alt+↑`) and editors their move-line bindings (`Alt+↑/↓`) —
the owner chose to keep those app shortcuts instead.

## Function keys & the Assistant key

- `F1`–`F10`: `Search` + top row. `F11`: `Fullscreen`. `F12`: `Search` + `Three-lines`.
- **Assistant** key (AT scancode `0xd8`, confirmed via `evtest`): mapped by the udev
  hwdb to `F23`. `F13` was abandoned because on some distros GNOME's
  `control-center-static` binds `F13` to launch Settings — pressing the key opened
  Settings. `F23` has no GNOME default; bind it to a custom shortcut in
  Settings → Keyboard → Custom Shortcuts if desired.
- **Three-lines** key (AT scancode `0x5d`, right of volume-up): ChromeOS names it
  `KEY_CONTROLPANEL` (and atkbd's *default* keycode for this scancode is `F13` —
  which is why older images "recognized" the key as F13/Settings, never as sleep).
  keyd only knows key codes 0–255 and silently discards anything larger while it
  holds the device (`KEY_CONTROLPANEL` is `0x243`), so the key presses produced
  nothing and did not even show up in `keyd monitor`. The hwdb therefore maps the
  scancode to `KEY_DASHBOARD` (keyd name `dashboard`) and keyd assigns the actions
  (`Super+S` bare, `F12` with `Search`).
- `Ctrl+Alt+Backspace` → `Ctrl+Alt+Delete` (kept as-is).

## Caveats & not-mapped items

- **Overview = screenshot**: a deliberate divergence from ChromeOS (where bare Overview
  opens the Desks picker and `Ctrl+Overview` screenshots). Per owner decision it maps
  straight to the screenshot UI and does not keep the original key.
- **Right Alt**: kept as keyd's default (`rightalt = layer(altgr)`), so it emits
  `RIGHTALT` (not `LEFTALT`). An `[altgr]` layer mirrors the `[alt]` layer so
  right-Alt combos (`Alt+Backspace`, `Alt+brightness`, `Alt+Search`) behave
  identically to left-Alt ones, matching ChromeOS.
- **Not mapped** (out of scope for a keyboard layer): right-click (`Search`/`Alt` + tap —
  touchpad/Wayland), clipboard history (`Search+V`), emoji panel, and the accessibility
  combos (`Search+Ctrl+h`, magnifier). Use two-finger tap and the OS UI instead.
- **Device-verify before trusting**: run `sudo keyd check` after editing. The raw
  arrive-as codes (Assistant scancode `0xd8` → `f23`, Three-lines scancode `0x5d`
  → `dashboard`) are confirmed with `sudo evtest` or
  `sudo systemd-hwdb query "evdev:atkbd:dmi:bvn*:bvr*:bd*:svnGoogle:pnEve:pvr*"`;
  `keyd monitor` shows keyd's *output* after mapping, not scancodes. Also confirm
  the top row arrives as media codes (`back`, `zoom`, `scale`, …) as this
  config assumes. Some firmware/distro states report the top row as `f1`–`f10`
  instead; if `keyd monitor` shows that, the left-hand key names above (`zoom`,
  `scale`, and the `back`…`volumeup` list under Search) must be rewritten to the
  `f1`–`f10` codes. The Three-lines key is *not* a sleep key — earlier revisions
  wrongly assumed it arrived as `sleep`; it is scancode `0x5d` (atkbd default
  keycode `F13`; ChromeOS calls it `KEY_CONTROLPANEL`), remapped to `dashboard`
  by the hwdb because keyd cannot see `KEY_CONTROLPANEL` at all (code > 255).
- **Updated hosts keep `/etc`**: the image build compiles the hwdb with
  `systemd-hwdb update --usr` (only the compiled `hwdb.bin` is read at runtime).
  bootc updates preserve `/etc`, so devices from earlier images may still carry
  the old `/etc/udev/hwdb.d/61-pixel-keyboard.hwdb`. The new `70-` filename
  deliberately sorts after the legacy `61-` record (all hwdb files merge in
  lexicographic order and later files win per property), but if a key stops
  behaving after an update, clean up once:
  `sudo rm -f /etc/udev/hwdb.d/61-pixel-keyboard.hwdb /etc/udev/hwdb.bin && sudo systemd-hwdb update && sudo udevadm trigger --subsystem-match=input && sudo systemctl restart keyd`.

# Modifications to the Upstream

- Add audio support from [WeirdTreeThing](https://github.com/WeirdTreeThing).
- Add keymap via [keyd](https://github.com/rvaiya/keyd).
- Add RPM Fusion (free + nonfree) plus full multimedia codecs: complete `ffmpeg`, the GStreamer plugin set (including `bad-freeworld` and `ugly` from RPM Fusion), and full VA-API hardware video decoding. AAC decode is software (via `ffmpeg`/`gstreamer1-plugin-libav`); HEVC (8/10-bit) video decode is accelerated on the Pixelbook's Kaby Lake iGPU, with `vainfo` listing the video profiles (HEVC, VP9).
- Improve Pixelbook Pen support.
- Add experimental auto brightness using the Pixelbook (eve) vendor curve.
- Tablet-mode keyboard backlight (issue #72): folding the Pixelbook into tablet mode turns the keyboard backlight off and hides the quick-settings "Keyboard" item; unfolding restores the previous level (ChromeOS powerd semantics). Implemented by the vendored GNOME Shell extension in `system_files/usr/share/gnome-shell/extensions/tablet-kbd-backlight@zhchmmx.github.io/`, enabled through `enabled-extensions` in `system_files/usr/share/glib-2.0/schemas/zz0-pixel-blue-modifications.gschema.override`.
- Add [Waydroid](https://waydro.id/) (Android in a container) from the Fedora repos.
- Add a small GTK4 firmware updater backed by a D-Bus activated system service (see below).
- Replace GNOME Software with Flatpaks (issue #87): the `gnome-software` and `gnome-software-rpm-ostree` RPMs are dropped from the image, and [Bazaar](https://flathub.org/apps/io.github.kolunmi.Bazaar) (the store, pinned in the dash where GNOME Software used to be) is preinstalled from Flathub via `flatpak_refs/`. The Firmware flatpak is no longer preinstalled: it only covers LVFS devices, which does not include the MrChromebox Full ROM; install it from Flathub if you also want LVFS updates for peripherals. The `fwupd` RPM stays in the image (it is part of the base). OS updates: with GNOME Software gone there is no store-provided update UI (including the power-menu "Update and Restart"); `bootc upgrade` / `rpm-ostree upgrade` remain the path until upstream settings integration lands.

# UEFI Firmware Update (MrChromebox)

Pixel Blue runs on the [MrChromebox](https://mrchromebox.org/) UEFI Full ROM, whose releases are **not** published on LVFS — fwupd and GNOME Firmware cannot see them. The updater is therefore split into a small front-end and a privileged service:

- `pixel-blue-updater` is a GTK4 application that runs as the user. It has no privileges: no root, no `/dev/mem`, no SPI access, no writes to root-owned files.
- `pixel-blue-firmware-daemon` is a D-Bus activated root service (`tech.pixelblue.Firmware1`, object `/tech/pixelblue/Firmware1`; the shared interface description is in `system_files/usr/share/dbus-1/interfaces/tech.pixelblue.Firmware1.xml`). It starts on demand, never at boot. `StartUpdate` runs the polkit action `tech.pixelblue.firmware.update` with `auth_admin`, so every flash needs an explicit administrator password; `ExportBackup` uses `tech.pixelblue.firmware.export-backup` (`auth_self`). Because the D-Bus interface is the whole boundary, the GUI can later ship as a Flatpak without changing the daemon.
- The dangerous steps are not reimplemented: the daemon sources the vendored, unmodified upstream `functions.sh`/`sources.sh` in a bash child and calls `clear_software_wp` and `flashrom_write_image` with the environment they expect. `flashrom` is the one packaged in the image; the only thing downloaded at runtime is the ROM from the MrChromebox CDN.

A weekly timer (`firmware-update-check.timer`) runs `/usr/libexec/firmware-update-check.sh`. It reads the vendored `sources.sh` (it does not fetch scripts from the network), compares the published release date with the installed BIOS date (`/sys/class/dmi/id/bios_date`), and when a newer release exists downloads the ROM plus its upstream `.sha1`, verifies both SHA-1 and SHA-256, and atomically publishes `/var/lib/pixel-blue/firmware/check.json`. Superseded dated ROMs (~16 MB each) are removed automatically. The timer only reads and downloads: **it never flashes**.

An update started from the updater runs `preflight -> backup -> clear software WP -> write -> verify`. Preflight checks UEFI boot, board `eve`, a non-`Google_*` BIOS version, Secure Boot off, BIOS Lock/SMM write protection off, Intel PR0/GPR0 ranges not covering the BIOS region, hardware write protection off, and AC power (or at least 50% battery). It then reads the current flash to `backup.rom` before writing anything, creates the failure marker, takes a logind `shutdown:sleep` inhibitor, clears software write protection through upstream `clear_software_wp`, re-verifies the downloaded SHA-256, writes the BIOS region via upstream `flashrom_write_image`, and finally verifies the flash with `flashrom -v`. Success writes `needs-reboot`; only after a reboot does DMI report the new date.

Everything lives in `/var/lib/pixel-blue/firmware/`: `check.json`, `cache/`, `backup.rom`, `backup.json`, `export/`, `attempts`, `last-success`, `last-failure`, `needs-reboot`, `flash-failed` and `mrchromebox.log`. The directory is 0755 and the metadata files are 0644 so the user-session notifier can read them; `backup.rom` is 0600 root. The updater's **导出恢复备份** (export recovery backup) button asks the daemon to copy the backup to `export/`, then copies it into your own `~/Downloads` with your own credentials — root never writes into a user's home. The former `/var/lib/pixel-blue/fwupd/` directory from the fwupd-plugin era is unused; leftovers across a bootc upgrade are harmless and can be deleted.

## When an update fails: class A vs class B

The service keeps a marker at `/var/lib/pixel-blue/firmware/flash-failed` (`class=A|B`, `state=inprogress|failed`, `attempts=N`). It is created as `class=B state=inprogress` immediately before software write protection is cleared, flipped to `state=failed` by every failure path (downgraded to `class=A` when the failure happened before any write), and deleted on success. The resident user-session notifier (`pixel-blue-firmware-alert.service`, enabled globally) watches the marker and the update metadata through `Gio.FileMonitor`, turns problems into a desktop notification, and grades the advice by class because the two cases demand opposite behaviour:

- **class A — the flash was never touched.** The write had not started yet (preflight, write protection, power, backup, checksum or staging failed). Rebooting or shutting down is completely safe; the notification says so. Click **Retry** in the updater; metadata and ROM stay local, so retrying does not need the network.
- **class B — the write started or was interrupted mid-write.** The flash content may be incomplete and *restarting now can leave the machine unbootable*. Do not reboot; open the updater and click **Retry**. While a class B alert is live the notifier holds a `systemd-inhibit --what=shutdown:sleep --mode=block` lock (the daemon holds its own inhibitor while it is flashing), so the normal "Power off / Restart" menu entries show the standard *"an app is blocking shutdown"* dialog instead of silently rebooting into a brick; bypassing that is an explicit, authenticated override. class A alerts never block shutdown.

The notifier's **打开更新器** (open updater) action activates the running updater through `org.gtk.Actions` (falling back to launching `pixel-blue-updater`), and a stale `state=inprogress` marker — the daemon died without running any error path — is reported the same way, graded by its class. A failure that never reaches the daemon (a download or checksum error during the weekly check) leaves no marker and none is needed: it cannot touch the flash. To stop the notifications manually:

``` bash
sudo rm /var/lib/pixel-blue/firmware/flash-failed
```

## "Cancel" does not stop a flash in progress

The updater has no cancel for a running flash. `StartUpdate` is fire-and-forget: `flashrom` writes and verifies to completion either way, and interrupting it is exactly the class B case. If you close the window or press power-off, keep the machine powered until the write finishes (about a minute), then reopen the updater. Never pull power during the write regardless of what the UI says.

## Worst case: power lost during a write

Before every write the daemon saves the current firmware to `backup.rom`. It always reads the BIOS region (`flashrom --ifd -i bios`) because the write needs that layout, and when the chip allows it also reads the whole 16 MB chip so the backup can be restored with an external programmer. On a retry after a failure the existing backup is reused when its recorded version still matches DMI and its SHA-256 is intact — a class B retry never overwrites the known-good image with the possibly half-written current flash. The updater shows which scope was captured. If power is lost mid-write and the machine no longer boots, the OS is unreachable and no software on the machine itself can rewrite the SPI chip. Recovery is hardware work:

1. Export the backup from the updater before flashing if you want it on hand (it lands in `~/Downloads`), or read it from the SSD with another Linux machine. The same-version `.rom` is also available from <https://www.mrchromebox.tech/files/firmware/full_rom/>.
2. From a second computer, reflash the SPI chip directly with a SOIC-8 test clip on the board's flash chip and a USB SPI programmer (e.g. CH341A):
   - full-chip backup: `sudo flashrom -p ch341a_spi -w backup.rom -v`
   - BIOS-region backup: `sudo flashrom -p ch341a_spi --ifd -i bios -w backup.rom -v`
   The clip needs stable 3.3 V — power the laptop through its charger or use a powered clip adapter; the official [coreboot flashrom guide](https://www.coreboot.org/Flashrom) and MrChromebox's bricking-recovery notes cover the wiring.
3. After a successful rewrite the old firmware boots again; reinstall the update with AC power connected.

Do **not** follow the ChromeOS SuzyQ/CCD recovery procedure: this machine runs the MrChromebox UEFI Full ROM, and a ChromeOS recovery image does not apply.

Everything before this point is deliberately stacked against it: the write is gated on AC power or ≥50% battery, the daemon holds a logind `shutdown:sleep` inhibitor (an open file descriptor, so a daemon crash releases it rather than leaving the machine un-shutdownable), and a class B failure re-blocks shutdown from the user session until a retry succeeds.

## Why the image vendors the MrChromebox scripts

`functions.sh` and `sources.sh` are upstream [Unlicense](https://unlicense.org/) code, installed **verbatim** from a pinned commit. `vendor/mrchromebox/VENDOR_COMMIT` records the 40-character commit and `manifest.sha256` the file hashes; `build_files/modules/42-firmware.sh` copies them to `/usr/lib/pixel-blue/mrchromebox/`. The CI `vendor` job recomputes the manifest, runs `bash -n`, and asserts that the functions and variables the daemon calls still exist and that the flashrom flags the parsing depends on are still present. Upgrading is `just vendor-mrchromebox <commit>`, reviewing the diff and committing; nothing is patched locally. `download-fullrom.sh` is installed too, but only as a manual/rescue tool — the automatic path never invokes it, because it fetches upstream metadata at runtime.

`flashrom` is not pinned: it comes from the Fedora base and moves with bootc updates. The daemon probes the installed binary for `--ifd`, `-N/--noverify-all` and `--use-first-chip` instead of assuming a version, and preflight fails closed if the chip cannot be probed.

Manual flashing remains possible at any time with the vendored downloader and the upstream tool. The downloader is the unmodified upstream script: outside a git checkout it fetches its own `sources.sh`/`device-db.sh` metadata from GitHub and writes the ROM to the chosen output directory. The automatic weekly path never invokes it.

``` bash
/usr/lib/pixel-blue/mrchromebox/download-fullrom.sh --list-versions
/usr/lib/pixel-blue/mrchromebox/download-fullrom.sh -o ~/Downloads eve
curl -LO https://mrchromebox.tech/firmware-util.sh && sudo bash firmware-util.sh
```

This feature only checks and flashes; it never touches the firmware automatically — updates are user-initiated only.

# Waydroid First-Time Setup

The Waydroid packages are baked into the image, but the Android system image is not pre-downloaded. On first use, run:

``` bash
sudo waydroid init
```

This downloads the LineageOS-based Android system image (choose the variant you want when prompted). Afterwards start the container and launch it:

``` bash
sudo systemctl start waydroid-container
waydroid session start
waydroid show-full-ui
```

# Learn More

For more information, please email me at [1055773545@qq.com](mailto:1055773545@qq.com).

You can also refer to the template I use, which contains more information about what each file in this repo does.
