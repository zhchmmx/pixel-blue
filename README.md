# Pixel Blue

This repository is based on Universal Blue's official template and built as a Fedora Silverblue image for Pixelbook.

This repository includes verified solutions from [WeirdTreeThing](https://github.com/WeirdTreeThing) and other respected contributors.

Please note that this is just an experimental project; use it at your own risk.

# How to Use

There are several ways for Pixelbook users to install this image on their device or just switch from another Universal Blue-based OS.

## Option 1: Install Pixel Blue (Not ready yet.)

For Chinese users, please use [this link](https://example.com) for faster downloads.

For users from other countries and regions, please check our GitHub Release page.

## Option 2: Switch to Pixel Blue

Pixel Blue is published in three channels. Pick the one you want and use the matching command.

### Latest channel (default)

`latest` follows the upstream Universal Blue `latest` image and always tracks the newest Fedora.

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:latest --enforce-container-sigpolicy
```

### Testing channel

`testing` is the preview channel for `latest`. It is built from the same upstream `latest` base, so it only differs by the changes that are being validated before they land in `latest`. Use it if you want to test upcoming changes early.

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:testing --enforce-container-sigpolicy
```

### Testing channel (preview of stable)

`testing` builds on the same Fedora GA release as `stable` and receives changes first, so it previews what `stable` will get next. It can be less stable than `stable`; use it if you want to try upcoming changes early.

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:testing --enforce-container-sigpolicy
```

### Pinning to a specific build

Every build is also published with a commit-based tag, so you can freeze to an exact build:

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:latest-<commit> --enforce-container-sigpolicy
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

## Verifying S3 is actually used

```bash
# Active selection is shown in square brackets, e.g. "[deep] s2idle":
cat /sys/power/mem_sleep

# After a suspend cycle, confirm the kernel took the S3 path:
journalctl -b | grep -i "suspend entry"    # expect "(deep)"

# Counters: success increments per cycle, failed should stay 0:
cat /sys/power/suspend_stats/success
cat /sys/power/suspend_stats/failed_suspend
```

The practical check: close the lid for an hour or two. S3 should drain very
little (typically under ~1%/hour) and resume back to the login screen in a couple
of seconds. If it wakes by rebooting instead, see the CR50 note below.

## CR50 TPM (resume) note

On eve, resume from suspend depends on the `tpm_tis_i2c_cr50` driver being
loaded. This image inherits the Fedora base kernel, which ships that support, and
it has been verified working on the Pixelbook — so no action is needed for normal
use. If a future kernel ever makes resume fail (sleeps fine but wakes to a
reboot), that module is the first thing to check.

## Optional fallback

On the rare machine where disabling Modern Standby still leaves the selection on
`s2idle`, you can force the deep path at boot with the kernel argument
`mem_sleep_default=deep` (via `rpm-ostree kargs`, or a
`/usr/lib/bootc/kargs.d` entry). This is intentionally not set in the image.

# Learn More

For more information, please email me at [1055773545@qq.com](mailto:1055773545@qq.com).

You can also refer to the template I use, which contains more information about what each file in this repo does.
