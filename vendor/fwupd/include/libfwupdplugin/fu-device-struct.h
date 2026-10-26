/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fwupd.h>
/**
 * FuDeviceInstanceFlags:
 */
typedef enum {
    /**
     * FU_DEVICE_INSTANCE_FLAG_NONE:
     */
    FU_DEVICE_INSTANCE_FLAG_NONE = 0,
    /**
     * FU_DEVICE_INSTANCE_FLAG_VISIBLE:
     */
    FU_DEVICE_INSTANCE_FLAG_VISIBLE = 1 << 0,
    /**
     * FU_DEVICE_INSTANCE_FLAG_QUIRKS:
     */
    FU_DEVICE_INSTANCE_FLAG_QUIRKS = 1 << 1,
    /**
     * FU_DEVICE_INSTANCE_FLAG_GENERIC:
     *
     * added by a baseclass
     */
    FU_DEVICE_INSTANCE_FLAG_GENERIC = 1 << 2,
    /**
     * FU_DEVICE_INSTANCE_FLAG_COUNTERPART:
     */
    FU_DEVICE_INSTANCE_FLAG_COUNTERPART = 1 << 3,
    /**
     * FU_DEVICE_INSTANCE_FLAG_DEPRECATED:
     */
    FU_DEVICE_INSTANCE_FLAG_DEPRECATED = 1 << 4,
} G_GNUC_FLAG_ENUM FuDeviceInstanceFlags;
gchar *fu_device_instance_flags_to_string(FuDeviceInstanceFlags val) G_GNUC_CONST;
/**
 * FuDeviceIncorporateFlags:
 *
 * Flags to use when incorporating a device instance.
 */
typedef enum {
    /**
     * FU_DEVICE_INCORPORATE_FLAG_BASECLASS:
     *
     * Set baseclass properties.
     */
    FU_DEVICE_INCORPORATE_FLAG_BASECLASS = 1 << 0,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_SUPERCLASS:
     *
     * Set superclass properties, implemented using `->incorporate()`.
     */
    FU_DEVICE_INCORPORATE_FLAG_SUPERCLASS = 1 << 1,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_VENDOR:
     *
     * Set vendor.
     */
    FU_DEVICE_INCORPORATE_FLAG_VENDOR = 1 << 2,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_VENDOR_IDS:
     *
     * Set vendor IDs.
     */
    FU_DEVICE_INCORPORATE_FLAG_VENDOR_IDS = 1 << 3,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_PHYSICAL_ID:
     *
     * Set the physical ID.
     */
    FU_DEVICE_INCORPORATE_FLAG_PHYSICAL_ID = 1 << 4,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_LOGICAL_ID:
     *
     * Set the logical ID.
     */
    FU_DEVICE_INCORPORATE_FLAG_LOGICAL_ID = 1 << 5,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_BACKEND_ID:
     *
     * Set the backend ID.
     */
    FU_DEVICE_INCORPORATE_FLAG_BACKEND_ID = 1 << 6,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_REMOVE_DELAY:
     *
     * Set the remove delay.
     */
    FU_DEVICE_INCORPORATE_FLAG_REMOVE_DELAY = 1 << 7,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_ACQUIESCE_DELAY:
     *
     * Set the acquiesce delay.
     */
    FU_DEVICE_INCORPORATE_FLAG_ACQUIESCE_DELAY = 1 << 8,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_ICONS:
     *
     * Set the icons.
     */
    FU_DEVICE_INCORPORATE_FLAG_ICONS = 1 << 9,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_UPDATE_ERROR:
     *
     * Set the update error.
     */
    FU_DEVICE_INCORPORATE_FLAG_UPDATE_ERROR = 1 << 10,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_UPDATE_STATE:
     *
     * Set the update state.
     */
    FU_DEVICE_INCORPORATE_FLAG_UPDATE_STATE = 1 << 11,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_VID:
     *
     * Set the vendor ID.
     */
    FU_DEVICE_INCORPORATE_FLAG_VID = 1 << 12,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_PID:
     *
     * Set the product ID.
     */
    FU_DEVICE_INCORPORATE_FLAG_PID = 1 << 13,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_UPDATE_MESSAGE:
     *
     * Set the update message.
     */
    FU_DEVICE_INCORPORATE_FLAG_UPDATE_MESSAGE = 1 << 14,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_UPDATE_IMAGE:
     *
     * Set the update image.
     */
    FU_DEVICE_INCORPORATE_FLAG_UPDATE_IMAGE = 1 << 15,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_EVENTS:
     *
     * Add the device events.
     */
    FU_DEVICE_INCORPORATE_FLAG_EVENTS = 1 << 16,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_INSTANCE_IDS:
     *
     * Set the instance IDs.
     */
    FU_DEVICE_INCORPORATE_FLAG_INSTANCE_IDS = 1 << 17,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_POSSIBLE_PLUGINS:
     *
     * Set the possible plugins.
     */
    FU_DEVICE_INCORPORATE_FLAG_POSSIBLE_PLUGINS = 1 << 18,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_GTYPE:
     *
     * Set the device GType.
     */
    FU_DEVICE_INCORPORATE_FLAG_GTYPE = 1 << 19,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_INSTANCE_KEYS:
     *
     * Set the device instance keys.
     */
    FU_DEVICE_INCORPORATE_FLAG_INSTANCE_KEYS = 1 << 20,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_PROXY:
     *
     * Set the proxy device.
     */
    FU_DEVICE_INCORPORATE_FLAG_PROXY = 1 << 21,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_PRIVATE_FLAGS:
     *
     * Set the private flags.
     */
    FU_DEVICE_INCORPORATE_FLAG_PRIVATE_FLAGS = 1 << 22,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_CREATED_MODIFIED:
     *
     * Sets the created and modified values.
     */
    FU_DEVICE_INCORPORATE_FLAG_CREATED_MODIFIED = 1 << 23,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_EQUIVALENT_ID:
     *
     * Sets the device equivalent ID.
     */
    FU_DEVICE_INCORPORATE_FLAG_EQUIVALENT_ID = 1 << 24,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_SET_QUIRK_KV:
     *
     * Call the set_quirk_kv() vfunc for the superclassed object.
     */
    FU_DEVICE_INCORPORATE_FLAG_SET_QUIRK_KV = 1 << 25,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_METADATA:
     *
     * Sets the metadata.
     */
    FU_DEVICE_INCORPORATE_FLAG_METADATA = 1 << 26,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_PARENT_IDS:
     *
     * Sets the various parent IDs.
     */
    FU_DEVICE_INCORPORATE_FLAG_PARENT_IDS = 1 << 27,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_REQUIRED_FREE:
     *
     * Sets the required free space.
     */
    FU_DEVICE_INCORPORATE_FLAG_REQUIRED_FREE = 1 << 28,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_UPDATE_REQUEST_ID:
     *
     * Sets the update request ID.
     */
    FU_DEVICE_INCORPORATE_FLAG_UPDATE_REQUEST_ID = 1 << 29,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_CUSTOM_FLAGS:
     *
     * Sets the custom flags.
     */
    FU_DEVICE_INCORPORATE_FLAG_CUSTOM_FLAGS = 1 << 30,
    /**
     * FU_DEVICE_INCORPORATE_FLAG_ALL:
     *
     * All flags
     */
    FU_DEVICE_INCORPORATE_FLAG_ALL = G_MAXUINT64,
} G_GNUC_FLAG_ENUM FuDeviceIncorporateFlags;
