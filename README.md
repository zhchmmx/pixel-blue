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

Pixel Blue is published in two channels. Pick the one you want and use the matching command.

### Latest channel (default)

`latest` follows the upstream Universal Blue `latest` image and always tracks the newest Fedora.

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:latest --enforce-container-sigpolicy
```

### Stable channel

`stable` builds on the current Fedora GA release, which is the version you would download from the official Fedora website. It only moves to a newer Fedora after that release has been declared stable.

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:stable --enforce-container-sigpolicy
```

### Pinning to a specific build

Every build is also published with a commit-based tag, so you can freeze to an exact build:

``` bash
sudo bootc switch ghcr.io/zhchmmx/pixel-blue:latest-<commit> --enforce-container-sigpolicy
```

After that, you may reboot your device to see the changes. Users on a moving channel (`stable` or `latest`) are updated automatically; users pinned to a commit tag will not receive updates until they switch back to a channel.

> For maintainers: the `stable` channel builds on the Fedora version set by `STABLE_FEDORA_VERSION` in `image-template.env`. Bump it to the new Fedora GA version to roll `stable` forward.

# Learn More

For more information, please email me at [1055773545@qq.com](mailto:1055773545@qq.com).

You can also refer to the template I use, which contains more information about what each file in this repo does.
