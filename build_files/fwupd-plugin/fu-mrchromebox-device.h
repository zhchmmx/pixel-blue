/*
 * fwupd plugin for MrChromebox UEFI Full ROM on the Google Pixelbook (eve).
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include <libfwupdplugin/fu-device.h>

#define FU_TYPE_MRCHROMEBOX_DEVICE (fu_mrchromebox_device_get_type())
G_DECLARE_FINAL_TYPE(FuMrchromeboxDevice, fu_mrchromebox_device, FU, MRCHROMEBOX_DEVICE, FuDevice)

FuDevice *fu_mrchromebox_device_new(FuContext *ctx);
