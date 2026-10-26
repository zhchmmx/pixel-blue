/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fwupd.h>
/**
 * FuContextFlags:
 */
typedef enum {
    /**
     * FU_CONTEXT_FLAG_NONE:
     */
    FU_CONTEXT_FLAG_NONE = 0,
    /**
     * FU_CONTEXT_FLAG_SAVE_EVENTS:
     *
     * so that they can be replayed to emulate devices
     */
    FU_CONTEXT_FLAG_SAVE_EVENTS = 1 << 0,
    /**
     * FU_CONTEXT_FLAG_SYSTEM_INHIBIT:
     *
     * all devices are not updatable
     */
    FU_CONTEXT_FLAG_SYSTEM_INHIBIT = 1 << 1,
    /**
     * FU_CONTEXT_FLAG_LOADED_HWINFO:
     *
     * has called fu_context_load()
     */
    FU_CONTEXT_FLAG_LOADED_HWINFO = 1 << 2,
    /**
     * FU_CONTEXT_FLAG_INHIBIT_VOLUME_MOUNT:
     *
     * usually for self tests
     */
    FU_CONTEXT_FLAG_INHIBIT_VOLUME_MOUNT = 1 << 3,
    /**
     * FU_CONTEXT_FLAG_FDE_BITLOCKER:
     *
     * full disk encryption
     */
    FU_CONTEXT_FLAG_FDE_BITLOCKER = 1 << 4,
    /**
     * FU_CONTEXT_FLAG_FDE_SNAPD:
     *
     * full disk encryption
     */
    FU_CONTEXT_FLAG_FDE_SNAPD = 1 << 5,
    /**
     * FU_CONTEXT_FLAG_IGNORE_EFIVARS_FREE_SPACE:
     *
     * ignore the free space requirement for db, dbx, etc
     */
    FU_CONTEXT_FLAG_IGNORE_EFIVARS_FREE_SPACE = 1 << 6,
    /**
     * FU_CONTEXT_FLAG_NO_IDLE_SOURCES:
     */
    FU_CONTEXT_FLAG_NO_IDLE_SOURCES = 1 << 7,
    /**
     * FU_CONTEXT_FLAG_INSECURE_UEFI:
     */
    FU_CONTEXT_FLAG_INSECURE_UEFI = 1 << 8,
    /**
     * FU_CONTEXT_FLAG_IS_HYPERVISOR:
     */
    FU_CONTEXT_FLAG_IS_HYPERVISOR = 1 << 9,
    /**
     * FU_CONTEXT_FLAG_IS_HYPERVISOR_PRIVILEGED:
     *
     * privileged xen can access most hardware
     */
    FU_CONTEXT_FLAG_IS_HYPERVISOR_PRIVILEGED = 1 << 10,
    /**
     * FU_CONTEXT_FLAG_IS_CONTAINER:
     */
    FU_CONTEXT_FLAG_IS_CONTAINER = 1 << 11,
    /**
     * FU_CONTEXT_FLAG_SMBIOS_UEFI_ENABLED:
     */
    FU_CONTEXT_FLAG_SMBIOS_UEFI_ENABLED = 1 << 12,
    /**
     * FU_CONTEXT_FLAG_NO_QUIRKS:
     */
    FU_CONTEXT_FLAG_NO_QUIRKS = 1 << 13,
    /**
     * FU_CONTEXT_FLAG_DUMMY_EFIVARS:
     */
    FU_CONTEXT_FLAG_DUMMY_EFIVARS = 1 << 14,
    /**
     * FU_CONTEXT_FLAG_IS_SERVER:
     */
    FU_CONTEXT_FLAG_IS_SERVER = 1 << 15,
    /**
     * FU_CONTEXT_FLAG_READONLY_FS:
     *
     * ignore readonly filesystem errors
     */
    FU_CONTEXT_FLAG_READONLY_FS = 1 << 16,
    /**
     * FU_CONTEXT_FLAG_NO_CACHE:
     *
     * do not save to a persistent cache
     */
    FU_CONTEXT_FLAG_NO_CACHE = 1 << 17,
    /**
     * FU_CONTEXT_FLAG_DUAL_BOOT_WINDOWS:
     */
    FU_CONTEXT_FLAG_DUAL_BOOT_WINDOWS = 1 << 18,
    /**
     * FU_CONTEXT_FLAG_FDE_SYSTEMD_PCRLOCK:
     *
     * full disk encryption
     */
    FU_CONTEXT_FLAG_FDE_SYSTEMD_PCRLOCK = 1 << 19,
}  FuContextFlags;
FuContextFlags fu_context_flag_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
/**
 * FuContextLoadFlags:
 */
typedef enum {
    /**
     * FU_CONTEXT_LOAD_FLAG_NONE:
     */
    FU_CONTEXT_LOAD_FLAG_NONE = 0,
    /**
     * FU_CONTEXT_LOAD_FLAG_HWID_CONFIG:
     */
    FU_CONTEXT_LOAD_FLAG_HWID_CONFIG = 1 << 0,
    /**
     * FU_CONTEXT_LOAD_FLAG_HWID_SMBIOS:
     */
    FU_CONTEXT_LOAD_FLAG_HWID_SMBIOS = 1 << 1,
    /**
     * FU_CONTEXT_LOAD_FLAG_HWID_FDT:
     */
    FU_CONTEXT_LOAD_FLAG_HWID_FDT = 1 << 2,
    /**
     * FU_CONTEXT_LOAD_FLAG_HWID_DMI:
     */
    FU_CONTEXT_LOAD_FLAG_HWID_DMI = 1 << 3,
    /**
     * FU_CONTEXT_LOAD_FLAG_HWID_KENV:
     */
    FU_CONTEXT_LOAD_FLAG_HWID_KENV = 1 << 4,
    /**
     * FU_CONTEXT_LOAD_FLAG_HWID_DARWIN:
     */
    FU_CONTEXT_LOAD_FLAG_HWID_DARWIN = 1 << 5,
    /**
     * FU_CONTEXT_LOAD_FLAG_WATCH_FILES:
     */
    FU_CONTEXT_LOAD_FLAG_WATCH_FILES = 1 << 6,
    /**
     * FU_CONTEXT_LOAD_FLAG_FIX_PERMISSIONS:
     */
    FU_CONTEXT_LOAD_FLAG_FIX_PERMISSIONS = 1 << 7,
    /**
     * FU_CONTEXT_LOAD_FLAG_PATH_STORE_DEFAULTS:
     */
    FU_CONTEXT_LOAD_FLAG_PATH_STORE_DEFAULTS = 1 << 8,
    /**
     * FU_CONTEXT_LOAD_FLAG_PATH_STORE_ENV:
     */
    FU_CONTEXT_LOAD_FLAG_PATH_STORE_ENV = 1 << 9,
} G_GNUC_FLAG_ENUM FuContextLoadFlags;
/**
 * FuContextEspFileFlags:
 */
typedef enum {
    /**
     * FU_CONTEXT_ESP_FILE_FLAG_NONE:
     */
    FU_CONTEXT_ESP_FILE_FLAG_NONE = 0,
    /**
     * FU_CONTEXT_ESP_FILE_FLAG_INCLUDE_FIRST_STAGE:
     *
     * e.g. shim
     */
    FU_CONTEXT_ESP_FILE_FLAG_INCLUDE_FIRST_STAGE = 1 << 0,
    /**
     * FU_CONTEXT_ESP_FILE_FLAG_INCLUDE_SECOND_STAGE:
     *
     * e.g. grub
     */
    FU_CONTEXT_ESP_FILE_FLAG_INCLUDE_SECOND_STAGE = 1 << 1,
    /**
     * FU_CONTEXT_ESP_FILE_FLAG_INCLUDE_REVOCATIONS:
     *
     * e.g. the `revocations.efi` file used by shim
     */
    FU_CONTEXT_ESP_FILE_FLAG_INCLUDE_REVOCATIONS = 1 << 2,
} G_GNUC_FLAG_ENUM FuContextEspFileFlags;
/**
 * FuContextQuirkSource:
 */
typedef enum {
    /**
     * FU_CONTEXT_QUIRK_SOURCE_DEVICE:
     *
     * perhaps a USB descriptor
     */
    FU_CONTEXT_QUIRK_SOURCE_DEVICE,
    /**
     * FU_CONTEXT_QUIRK_SOURCE_FILE:
     *
     * an internal .quirk file.
     */
    FU_CONTEXT_QUIRK_SOURCE_FILE,
    /**
     * FU_CONTEXT_QUIRK_SOURCE_DB:
     *
     * populated from usb.ids and pci.ids
     */
    FU_CONTEXT_QUIRK_SOURCE_DB,
    /**
     * FU_CONTEXT_QUIRK_SOURCE_FALLBACK:
     *
     * perhaps from the PCI class information
     */
    FU_CONTEXT_QUIRK_SOURCE_FALLBACK,
}  FuContextQuirkSource;
#define FU_CONTEXT_QUIRK_SOURCE_LAST 4
