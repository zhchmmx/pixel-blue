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

# Experimental automatic brightness

The image ships an experimental ambient-light auto-brightness daemon built
around the Pixelbook (eve) vendor curve taken from ChromeOS `power_manager`. It
is enabled by default and follows GNOME's automatic brightness toggle; while it
is active, GNOME's own auto-brightness is kept out of the way.

## What it does

- seven brightness steps with the vendor down/up lux thresholds (hysteresis),
  so brightness does not flap around a single lux value
- exponential smoothing of the sensor readings
- the ChromeOS non-linear percent-to-backlight curve
- a short ramp between steps instead of an instant jump
- if you change the brightness yourself, the daemon yields; it takes control
  back once the ambient light changes by more than 100 lux

## Enabled by default

`als-brightness.service` is a per-user service enabled for all login sessions.
It stays resident and reads `org.gnome.settings-daemon.plugins.power
ambient-enabled` from inside its main loop (every 5 s); when the toggle is off
it simply pauses, when it is on it runs the curve. Because the gsettings key
defaults to true, automatic brightness is on out of the box — no extra watcher
process needed.

## Disable

Turn it off (survives logins):

``` bash
gsettings set org.gnome.settings-daemon.plugins.power ambient-enabled false
```

Or remove the feature entirely:

``` bash
systemctl --user disable --now als-brightness.service
```

## Tuning the curve

The seven steps and their lux thresholds live in
`/usr/lib/pixel-blue/als-steps.conf`. Because `/usr` is read-only, put an
override at `/etc/pixel-blue/als-steps.conf` with the same format:

```
# ui_percent  brightness_percent  down_lux  up_lux
36.14 36.14 -1 90
47.62 47.62 40 250
```

`-1` means "no transition in that direction": the first row has no down
threshold and the last row has no up threshold. Restart the daemon after
editing:

``` bash
systemctl --user restart als-brightness.service
```

## Caveats

- The sensor is intentionally still published by `iio-sensor-proxy` so that
  GNOME Settings keeps showing the **Automatic Brightness** switch. A D-Bus
  policy (`/usr/share/dbus-1/system.d/zz-pixel-blue-als.conf`) denies
  `ClaimLight()` instead, which stops gnome-settings-daemon from reading the
  sensor and competing for the backlight. Because polling in
  `iio-sensor-proxy` only runs while a client holds a claim, gsd gets no light
  updates and leaves brightness to the daemon.
- Transitions are stepwise, like ChromeOS, not continuous.
- The sensor lux scale may differ slightly from ChromeOS; adjust the thresholds
  in the override file if steps trigger at the wrong ambient light.
- Experimental: check `journalctl --user -u als-brightness.service` if
  something does not work.

# Modifications to the Upstream

- Add audio support from [WeirdTreeThing](https://github.com/WeirdTreeThing).
- Add keymap via [keyd](https://github.com/rvaiya/keyd).
- Improve Pixelbook Pen support.
- Add experimental auto brightness using the Pixelbook (eve) vendor curve. (Opt-in, see below)

# Learn More

For more information, please email me at [1055773545@qq.com](mailto:1055773545@qq.com).

You can also refer to the template I use, which contains more information about what each file in this repo does.
