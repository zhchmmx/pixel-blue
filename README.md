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

Every build is also published with a commit-based tag, so you can freeze to an exact build:

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:latest-<commit>
```

After that, you may reboot your device to see the changes. Users on a moving channel (`latest` or `testing`) are updated automatically; users pinned to a commit tag will not receive updates until they switch back to a channel.

> For maintainers: `latest` is built from the `main` branch and `testing` from the `testing` branch. To roll a change out, commit it on `testing`, verify the published `testing` image, then merge `testing` into `main` to promote it to `latest`.

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
- Improve Pixelbook Pen support.
- Add experimental auto brightness using the Pixelbook (eve) vendor curve.

# Learn More

For more information, please email me at [1055773545@qq.com](mailto:1055773545@qq.com).

You can also refer to the template I use, which contains more information about what each file in this repo does.
