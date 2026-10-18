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

# Learn More

For more information, please email me at [1055773545@qq.com](mailto:1055773545@qq.com).

You can also refer to the template I use, which contains more information about what each file in this repo does.
