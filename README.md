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
ChromeOS does it. The combos follow the official Google shortcut list, and the
keys keyd *emits* are chosen so they do not collide with GNOME's own defaults
(verified against upstream `gsettings-desktop-schemas`). The config lives in
`system_files/etc/keyd/cros.conf` (plus `system_files/etc/udev/hwdb.d/61-pixel-keyboard.hwdb`
for the Assistant key); this table is the human-readable reference.

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
| `Three-lines` (`sleep`) | System menu (Quick Settings) | `Super+S` → **Quick Settings** (GNOME 45+ native) |

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
| `Alt` + `↑` / `↓` | `Page Up` / `Page Down` | the official 6-pack page keys (see caveats) |
| `Alt` + `Backspace` | `Delete` | forward delete |
| `Alt` + `Search` | `Caps Lock` | Caps Lock |
| `Alt` + `Brightness −` / `+` | keyboard backlight | native pass-through |

`Ctrl+Alt+↑/↓` and `Ctrl+Alt+←/→` are **not** remapped — GNOME binds them to workspace
switching, and the owner chose to keep that. Bare `Alt+←/→` is likewise left untouched
(apps use it for back/forward), since ChromeOS does not use it for text navigation.

## Function keys & the Assistant key

- `F1`–`F10`: `Search` + top row. `F11`: `Fullscreen`. `F12`: `Search` + `Three-lines`.
- **Assistant** key (AT scancode `0xd8`, confirmed via `evtest`): mapped by the udev
  hwdb to `F13`. `F13` has no GNOME default, so bind it to any custom shortcut in
  Settings → Keyboard → Custom Shortcuts.
- `Ctrl+Alt+Backspace` → `Ctrl+Alt+Delete` (kept as-is).

## Caveats & not-mapped items

- **`Alt+↑/↓`**: GNOME has *no* system-level binding for these (schema-verified), but
  some **applications** do — most notably GNOME Files uses `Alt+↑` to jump to the
  parent folder (and `Alt+↓` to a child / history entry), and many text editors bind
  `Alt+↑/↓` to move the current line. Because keyd works below the app layer, this
  mapping hijacks them system-wide and Files' parent-folder shortcut is lost. To revert,
  delete the two `up = pageup` / `down = pagedown` lines in the `[alt]` section.
- **Overview = screenshot**: a deliberate divergence from ChromeOS (where bare Overview
  opens the Desks picker and `Ctrl+Overview` screenshots). Per owner decision it maps
  straight to the screenshot UI and does not keep the original key.
- **Right Alt**: mapped to behave as a plain `Alt` so the `Alt` combos work with either
  Alt. If a real ChromeOS device shows right Alt does *not* trigger Alt combos, remove
  `rightalt = layer(alt)`.
- **Not mapped** (out of scope for a keyboard layer): right-click (`Search`/`Alt` + tap —
  touchpad/Wayland), clipboard history (`Search+V`), emoji panel, and the accessibility
  combos (`Search+Ctrl+h`, magnifier). Use two-finger tap and the OS UI instead.
- **Device-verify before trusting**: run `sudo keyd check` after editing, and use
  `keyd monitor` to confirm (a) the Assistant key really reports scancode `0xd8`, and
  (b) the top row arrives as media codes (`back`, `zoom`, `scale`, `sleep`, …) as this
  config assumes. Some firmware/distro states report the top row as `f1`–`f10` instead;
  if `keyd monitor` shows that, the left-hand key names above (`zoom`, `scale`, `sleep`,
  and the `back`…`volumeup` list under Search) must be rewritten to the `f1`–`f13` codes.
  The Three-lines key is likewise assumed to arrive as `sleep`.

# Modifications to the Upstream

- Add audio support from [WeirdTreeThing](https://github.com/WeirdTreeThing).
- Add keymap via [keyd](https://github.com/rvaiya/keyd).
- Improve Pixelbook Pen support.
- Add experimental auto brightness using the Pixelbook (eve) vendor curve.

# Learn More

For more information, please email me at [1055773545@qq.com](mailto:1055773545@qq.com).

You can also refer to the template I use, which contains more information about what each file in this repo does.
