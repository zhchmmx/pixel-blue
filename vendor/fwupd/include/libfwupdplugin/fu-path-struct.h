/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fwupd.h>
/**
 * FuPathKind:
 *
 * Path types to use when dynamically determining a path at runtime.
 */
typedef enum {
    /**
     * FU_PATH_KIND_CACHEDIR_PKG:
     *
     * The cache directory (/var/cache/fwupd)
     */
    FU_PATH_KIND_CACHEDIR_PKG,
    /**
     * FU_PATH_KIND_DATADIR_PKG:
     *
     * The non-volatile data store (/usr/share/fwupd)
     */
    FU_PATH_KIND_DATADIR_PKG,
    /**
     * FU_PATH_KIND_EFIAPPDIR:
     *
     * The location to store EFI apps before install (/usr/libexec/fwupd/efi)
     */
    FU_PATH_KIND_EFIAPPDIR,
    /**
     * FU_PATH_KIND_LOCALSTATEDIR:
     *
     * The local state directory (/var)
     */
    FU_PATH_KIND_LOCALSTATEDIR,
    /**
     * FU_PATH_KIND_LOCALSTATEDIR_PKG:
     *
     * The local state directory for the package (/var/lib/fwupd)
     */
    FU_PATH_KIND_LOCALSTATEDIR_PKG,
    /**
     * FU_PATH_KIND_LIBDIR_PKG:
     *
     * The location to look for plugins for package (/usr/lib/[triplet]/fwupd-plugins-3)
     */
    FU_PATH_KIND_LIBDIR_PKG,
    /**
     * FU_PATH_KIND_SYSCONFDIR:
     *
     * The configuration location (/etc)
     */
    FU_PATH_KIND_SYSCONFDIR,
    /**
     * FU_PATH_KIND_SYSCONFDIR_PKG:
     *
     * The package configuration location (/etc/fwupd)
     */
    FU_PATH_KIND_SYSCONFDIR_PKG,
    /**
     * FU_PATH_KIND_SYSFSDIR:
     *
     * The sysfs base location (/sys)
     */
    FU_PATH_KIND_SYSFSDIR,
    /**
     * FU_PATH_KIND_SYSFSDIR_FW:
     *
     * The sysfs firmware location (/sys/firmware)
     */
    FU_PATH_KIND_SYSFSDIR_FW,
    /**
     * FU_PATH_KIND_SYSFSDIR_DRIVERS:
     *
     * The platform sysfs directory (/sys/bus/platform/drivers)
     */
    FU_PATH_KIND_SYSFSDIR_DRIVERS,
    /**
     * FU_PATH_KIND_SYSFSDIR_TPM:
     *
     * The TPM sysfs directory (/sys/class/tpm)
     */
    FU_PATH_KIND_SYSFSDIR_TPM,
    /**
     * FU_PATH_KIND_PROCFS:
     *
     * The procfs location (/proc)
     */
    FU_PATH_KIND_PROCFS,
    /**
     * FU_PATH_KIND_SYSFSDIR_SECURITY:
     *
     * The sysfs security location (/sys/kernel/security)
     */
    FU_PATH_KIND_SYSFSDIR_SECURITY,
    /**
     * FU_PATH_KIND_ACPI_TABLES:
     *
     * The location of the ACPI tables
     */
    FU_PATH_KIND_ACPI_TABLES,
    /**
     * FU_PATH_KIND_RUNDIR:
     *
     * The runtime directory (/run)
     */
    FU_PATH_KIND_RUNDIR,
    /**
     * FU_PATH_KIND_LOCKDIR:
     *
     * The lock directory (/run/lock)
     */
    FU_PATH_KIND_LOCKDIR,
    /**
     * FU_PATH_KIND_SYSFSDIR_FW_ATTRIB:
     *
     * The firmware attributes directory (/sys/class/firmware-attributes)
     */
    FU_PATH_KIND_SYSFSDIR_FW_ATTRIB,
    /**
     * FU_PATH_KIND_FIRMWARE_SEARCH:
     *
     * The path to configure the kernel policy for runtime loading other than /lib/firmware (/sys/module/firmware_class/parameters/path)
     */
    FU_PATH_KIND_FIRMWARE_SEARCH,
    /**
     * FU_PATH_KIND_DATADIR_QUIRKS:
     *
     * The quirks data store (/usr/share/fwupd/quirks.d)
     */
    FU_PATH_KIND_DATADIR_QUIRKS,
    /**
     * FU_PATH_KIND_LOCALSTATEDIR_QUIRKS:
     *
     * The local state directory for quirks (/var/lib/fwupd/quirks.d)
     */
    FU_PATH_KIND_LOCALSTATEDIR_QUIRKS,
    /**
     * FU_PATH_KIND_LOCALSTATEDIR_METADATA:
     *
     * The local state directory for metadata (/var/lib/fwupd/metadata)
     */
    FU_PATH_KIND_LOCALSTATEDIR_METADATA,
    /**
     * FU_PATH_KIND_LOCALSTATEDIR_REMOTES:
     *
     * The local state directory for remotes (/var/lib/fwupd/remotes.d)
     */
    FU_PATH_KIND_LOCALSTATEDIR_REMOTES,
    /**
     * FU_PATH_KIND_WIN32_BASEDIR:
     *
     * The root of the install directory on Windows
     */
    FU_PATH_KIND_WIN32_BASEDIR,
    /**
     * FU_PATH_KIND_LOCALCONFDIR_PKG:
     *
     * The package configuration override (/var/etc/fwupd)
     */
    FU_PATH_KIND_LOCALCONFDIR_PKG,
    /**
     * FU_PATH_KIND_SYSFSDIR_DMI:
     *
     * The sysfs DMI location, (/sys/class/dmi/id)
     */
    FU_PATH_KIND_SYSFSDIR_DMI,
    /**
     * FU_PATH_KIND_HOSTFS_ROOT:
     *
     * The root of the host filesystem (/)
     */
    FU_PATH_KIND_HOSTFS_ROOT,
    /**
     * FU_PATH_KIND_HOSTFS_BOOT:
     *
     * The host boot directory, (/boot)
     */
    FU_PATH_KIND_HOSTFS_BOOT,
    /**
     * FU_PATH_KIND_DEVFS:
     *
     * The host dev directory, (/dev)
     */
    FU_PATH_KIND_DEVFS,
    /**
     * FU_PATH_KIND_LOCALTIME:
     *
     * The timezone symlink (/etc/localtime)
     */
    FU_PATH_KIND_LOCALTIME,
    /**
     * FU_PATH_KIND_LIBEXECDIR:
     *
     * The directory to launch executables
     */
    FU_PATH_KIND_LIBEXECDIR,
    /**
     * FU_PATH_KIND_LIBEXECDIR_PKG:
     *
     * The directory launch executables packaged with daemon
     */
    FU_PATH_KIND_LIBEXECDIR_PKG,
    /**
     * FU_PATH_KIND_DATADIR_VENDOR_IDS:
     *
     * The vendor ID store (/usr/share/hwdata)
     */
    FU_PATH_KIND_DATADIR_VENDOR_IDS,
    /**
     * FU_PATH_KIND_DEBUGFSDIR:
     *
     * The debugfs directory (/sys/kernel/debug)
     */
    FU_PATH_KIND_DEBUGFSDIR,
    /**
     * FU_PATH_KIND_UEFI_ESP:
     *
     * The UEFI ESP, typically autodetected using udisks
     */
    FU_PATH_KIND_UEFI_ESP,
}  FuPathKind;
#define FU_PATH_KIND_LAST 35
const gchar *fu_path_kind_to_string(FuPathKind val) G_GNUC_CONST;
