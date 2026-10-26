/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fu-input-stream.h>
/**
 * FuSmbiosStructureType:
 */
typedef enum {
    /**
     * FU_SMBIOS_STRUCTURE_TYPE_BIOS:
     */
    FU_SMBIOS_STRUCTURE_TYPE_BIOS,
    /**
     * FU_SMBIOS_STRUCTURE_TYPE_SYSTEM:
     */
    FU_SMBIOS_STRUCTURE_TYPE_SYSTEM,
    /**
     * FU_SMBIOS_STRUCTURE_TYPE_BASEBOARD:
     *
     * aka motherboard
     */
    FU_SMBIOS_STRUCTURE_TYPE_BASEBOARD,
    /**
     * FU_SMBIOS_STRUCTURE_TYPE_CHASSIS:
     */
    FU_SMBIOS_STRUCTURE_TYPE_CHASSIS,
}  FuSmbiosStructureType;
#define FU_SMBIOS_STRUCTURE_TYPE_LAST 4
/**
 * FuSmbiosChassisKind:
 */
typedef enum {
    /**
     * FU_SMBIOS_CHASSIS_KIND_UNSET:
     *
     * inferred
     */
    FU_SMBIOS_CHASSIS_KIND_UNSET,
    /**
     * FU_SMBIOS_CHASSIS_KIND_OTHER:
     */
    FU_SMBIOS_CHASSIS_KIND_OTHER,
    /**
     * FU_SMBIOS_CHASSIS_KIND_UNKNOWN:
     */
    FU_SMBIOS_CHASSIS_KIND_UNKNOWN,
    /**
     * FU_SMBIOS_CHASSIS_KIND_DESKTOP:
     */
    FU_SMBIOS_CHASSIS_KIND_DESKTOP,
    /**
     * FU_SMBIOS_CHASSIS_KIND_LOW_PROFILE_DESKTOP:
     */
    FU_SMBIOS_CHASSIS_KIND_LOW_PROFILE_DESKTOP,
    /**
     * FU_SMBIOS_CHASSIS_KIND_PIZZA_BOX:
     */
    FU_SMBIOS_CHASSIS_KIND_PIZZA_BOX,
    /**
     * FU_SMBIOS_CHASSIS_KIND_MINI_TOWER:
     */
    FU_SMBIOS_CHASSIS_KIND_MINI_TOWER,
    /**
     * FU_SMBIOS_CHASSIS_KIND_TOWER:
     */
    FU_SMBIOS_CHASSIS_KIND_TOWER,
    /**
     * FU_SMBIOS_CHASSIS_KIND_PORTABLE:
     */
    FU_SMBIOS_CHASSIS_KIND_PORTABLE,
    /**
     * FU_SMBIOS_CHASSIS_KIND_LAPTOP:
     */
    FU_SMBIOS_CHASSIS_KIND_LAPTOP,
    /**
     * FU_SMBIOS_CHASSIS_KIND_NOTEBOOK:
     */
    FU_SMBIOS_CHASSIS_KIND_NOTEBOOK,
    /**
     * FU_SMBIOS_CHASSIS_KIND_HAND_HELD:
     */
    FU_SMBIOS_CHASSIS_KIND_HAND_HELD,
    /**
     * FU_SMBIOS_CHASSIS_KIND_DOCKING_STATION:
     */
    FU_SMBIOS_CHASSIS_KIND_DOCKING_STATION,
    /**
     * FU_SMBIOS_CHASSIS_KIND_ALL_IN_ONE:
     */
    FU_SMBIOS_CHASSIS_KIND_ALL_IN_ONE,
    /**
     * FU_SMBIOS_CHASSIS_KIND_SUB_NOTEBOOK:
     */
    FU_SMBIOS_CHASSIS_KIND_SUB_NOTEBOOK,
    /**
     * FU_SMBIOS_CHASSIS_KIND_SPACE_SAVING:
     */
    FU_SMBIOS_CHASSIS_KIND_SPACE_SAVING,
    /**
     * FU_SMBIOS_CHASSIS_KIND_LUNCH_BOX:
     */
    FU_SMBIOS_CHASSIS_KIND_LUNCH_BOX,
    /**
     * FU_SMBIOS_CHASSIS_KIND_MAIN_SERVER:
     */
    FU_SMBIOS_CHASSIS_KIND_MAIN_SERVER,
    /**
     * FU_SMBIOS_CHASSIS_KIND_EXPANSION:
     */
    FU_SMBIOS_CHASSIS_KIND_EXPANSION,
    /**
     * FU_SMBIOS_CHASSIS_KIND_SUBCHASSIS:
     */
    FU_SMBIOS_CHASSIS_KIND_SUBCHASSIS,
    /**
     * FU_SMBIOS_CHASSIS_KIND_BUS_EXPANSION:
     */
    FU_SMBIOS_CHASSIS_KIND_BUS_EXPANSION,
    /**
     * FU_SMBIOS_CHASSIS_KIND_PERIPHERAL:
     */
    FU_SMBIOS_CHASSIS_KIND_PERIPHERAL,
    /**
     * FU_SMBIOS_CHASSIS_KIND_RAID:
     */
    FU_SMBIOS_CHASSIS_KIND_RAID,
    /**
     * FU_SMBIOS_CHASSIS_KIND_RACK_MOUNT:
     */
    FU_SMBIOS_CHASSIS_KIND_RACK_MOUNT,
    /**
     * FU_SMBIOS_CHASSIS_KIND_SEALED_CASE_PC:
     */
    FU_SMBIOS_CHASSIS_KIND_SEALED_CASE_PC,
    /**
     * FU_SMBIOS_CHASSIS_KIND_MULTI_SYSTEM:
     */
    FU_SMBIOS_CHASSIS_KIND_MULTI_SYSTEM,
    /**
     * FU_SMBIOS_CHASSIS_KIND_COMPACT_PCI:
     */
    FU_SMBIOS_CHASSIS_KIND_COMPACT_PCI,
    /**
     * FU_SMBIOS_CHASSIS_KIND_ADVANCED_TCA:
     */
    FU_SMBIOS_CHASSIS_KIND_ADVANCED_TCA,
    /**
     * FU_SMBIOS_CHASSIS_KIND_BLADE:
     */
    FU_SMBIOS_CHASSIS_KIND_BLADE,
    /**
     * FU_SMBIOS_CHASSIS_KIND_RESERVED:
     *
     * 0x1D is missing!
     */
    FU_SMBIOS_CHASSIS_KIND_RESERVED,
    /**
     * FU_SMBIOS_CHASSIS_KIND_TABLET:
     */
    FU_SMBIOS_CHASSIS_KIND_TABLET,
    /**
     * FU_SMBIOS_CHASSIS_KIND_CONVERTIBLE:
     */
    FU_SMBIOS_CHASSIS_KIND_CONVERTIBLE,
    /**
     * FU_SMBIOS_CHASSIS_KIND_DETACHABLE:
     */
    FU_SMBIOS_CHASSIS_KIND_DETACHABLE,
    /**
     * FU_SMBIOS_CHASSIS_KIND_IOT_GATEWAY:
     */
    FU_SMBIOS_CHASSIS_KIND_IOT_GATEWAY,
    /**
     * FU_SMBIOS_CHASSIS_KIND_EMBEDDED_PC:
     */
    FU_SMBIOS_CHASSIS_KIND_EMBEDDED_PC,
    /**
     * FU_SMBIOS_CHASSIS_KIND_MINI_PC:
     */
    FU_SMBIOS_CHASSIS_KIND_MINI_PC,
    /**
     * FU_SMBIOS_CHASSIS_KIND_STICK_PC:
     */
    FU_SMBIOS_CHASSIS_KIND_STICK_PC,
}  FuSmbiosChassisKind;
#define FU_SMBIOS_CHASSIS_KIND_LAST 37
const gchar *fu_smbios_chassis_kind_to_string(FuSmbiosChassisKind val) G_GNUC_CONST;
/**
 * FuSmbiosBiosCharacteristics:
 */
typedef enum {
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS___NONE:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS___NONE = 1 << 0,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS___RESERVED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS___RESERVED = 1 << 1,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS___UNKNOWN:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS___UNKNOWN = 1 << 2,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_BIOS_CHARACTERISTICS_NOT_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_BIOS_CHARACTERISTICS_NOT_SUPPORTED = 1 << 3,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_ISA_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_ISA_SUPPORTED = 1 << 4,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_MCA_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_MCA_SUPPORTED = 1 << 5,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EISA_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EISA_SUPPORTED = 1 << 6,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_PCI_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_PCI_SUPPORTED = 1 << 7,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_PCCARD_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_PCCARD_SUPPORTED = 1 << 8,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_PLUG_AND_PLAY_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_PLUG_AND_PLAY_SUPPORTED = 1 << 9,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_APM_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_APM_SUPPORTED = 1 << 10,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_BIOS_IS_UPGRADEABLE:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_BIOS_IS_UPGRADEABLE = 1 << 11,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_BIOS_SHADOWING_ALLOWED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_BIOS_SHADOWING_ALLOWED = 1 << 12,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_VLVESA_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_VLVESA_SUPPORTED = 1 << 13,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_ESCD_SUPPORT_AVAILABLE:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_ESCD_SUPPORT_AVAILABLE = 1 << 14,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_BOOT_FROM_CD_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_BOOT_FROM_CD_SUPPORTED = 1 << 15,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_SELECTABLE_BOOT_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_SELECTABLE_BOOT_SUPPORTED = 1 << 16,
} G_GNUC_FLAG_ENUM FuSmbiosBiosCharacteristics;
/**
 * FuSmbiosBiosCharacteristicsExt:
 */
typedef enum {
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_ACPI_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_ACPI_SUPPORTED = 1 << 0,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_USB_LEGACY_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_USB_LEGACY_SUPPORTED = 1 << 1,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_AGP_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_AGP_SUPPORTED = 1 << 2,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_I2O_BOOT_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_I2O_BOOT_SUPPORTED = 1 << 3,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_LS120_SUPER_DISK_BOOT_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_LS120_SUPER_DISK_BOOT_SUPPORTED = 1 << 4,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_ATAPI_ZIP_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_ATAPI_ZIP_SUPPORTED = 1 << 5,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_1394_BOOT_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_1394_BOOT_SUPPORTED = 1 << 6,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_SMART_BATTERY_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_SMART_BATTERY_SUPPORTED = 1 << 7,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_BIOS_BOOT_SPECIFICATION_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_BIOS_BOOT_SPECIFICATION_SUPPORTED = 1 << 8,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_FUNCTION_KEY_NETWORK_BOOT_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_FUNCTION_KEY_NETWORK_BOOT_SUPPORTED = 1 << 9,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_ENABLE_TARGETED_CONTENT_DISTRIBUTION:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_ENABLE_TARGETED_CONTENT_DISTRIBUTION = 1 << 10,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_UEFI_SPECIFICATION_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_UEFI_SPECIFICATION_SUPPORTED = 1 << 11,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_IS_VIRTUAL_MACHINE:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_IS_VIRTUAL_MACHINE = 1 << 12,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_MANUFACTURING_MODE_SUPPORTED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_MANUFACTURING_MODE_SUPPORTED = 1 << 13,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_MANUFACTURING_MODE_ENABLED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT_MANUFACTURING_MODE_ENABLED = 1 << 14,
    /**
     * FU_SMBIOS_BIOS_CHARACTERISTICS_EXT___RESERVED:
     */
    FU_SMBIOS_BIOS_CHARACTERISTICS_EXT___RESERVED = 1 << 15,
} G_GNUC_FLAG_ENUM FuSmbiosBiosCharacteristicsExt;
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructSmbiosEp32;

FuStructSmbiosEp32 *fu_struct_smbios_ep32_ref(FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_unref(FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructSmbiosEp32, fu_struct_smbios_ep32_unref)
FuStructSmbiosEp32 *fu_struct_smbios_ep32_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructSmbiosEp32 *fu_struct_smbios_ep32_parse(const guint8 *buf, gsize bufsz, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gchar *fu_struct_smbios_ep32_get_anchor_str(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep32_get_entry_point_csum(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep32_get_entry_point_len(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep32_get_smbios_major_ver(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep32_get_smbios_minor_ver(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_smbios_ep32_get_max_structure_sz(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep32_get_entry_point_rev(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gchar *fu_struct_smbios_ep32_get_intermediate_anchor_str(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep32_get_intermediate_csum(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_smbios_ep32_get_structure_table_len(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_smbios_ep32_get_structure_table_addr(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_smbios_ep32_get_number_smbios_structs(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep32_get_smbios_bcd_rev(const FuStructSmbiosEp32 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gboolean fu_struct_smbios_ep32_set_anchor_str(FuStructSmbiosEp32 *st, const gchar *value, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_smbios_ep32_set_entry_point_csum(FuStructSmbiosEp32 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_entry_point_len(FuStructSmbiosEp32 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_smbios_major_ver(FuStructSmbiosEp32 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_smbios_minor_ver(FuStructSmbiosEp32 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_max_structure_sz(FuStructSmbiosEp32 *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_entry_point_rev(FuStructSmbiosEp32 *st, guint8 value) G_GNUC_NON_NULL(1);
gboolean fu_struct_smbios_ep32_set_intermediate_anchor_str(FuStructSmbiosEp32 *st, const gchar *value, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_smbios_ep32_set_intermediate_csum(FuStructSmbiosEp32 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_structure_table_len(FuStructSmbiosEp32 *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_structure_table_addr(FuStructSmbiosEp32 *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_number_smbios_structs(FuStructSmbiosEp32 *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep32_set_smbios_bcd_rev(FuStructSmbiosEp32 *st, guint8 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_EP32_OFFSET_ANCHOR_STR 0x0
#define FU_STRUCT_SMBIOS_EP32_OFFSET_ENTRY_POINT_CSUM 0x4
#define FU_STRUCT_SMBIOS_EP32_OFFSET_ENTRY_POINT_LEN 0x5
#define FU_STRUCT_SMBIOS_EP32_OFFSET_SMBIOS_MAJOR_VER 0x6
#define FU_STRUCT_SMBIOS_EP32_OFFSET_SMBIOS_MINOR_VER 0x7
#define FU_STRUCT_SMBIOS_EP32_OFFSET_MAX_STRUCTURE_SZ 0x8
#define FU_STRUCT_SMBIOS_EP32_OFFSET_ENTRY_POINT_REV 0xA
#define FU_STRUCT_SMBIOS_EP32_OFFSET_INTERMEDIATE_ANCHOR_STR 0x10
#define FU_STRUCT_SMBIOS_EP32_OFFSET_INTERMEDIATE_CSUM 0x15
#define FU_STRUCT_SMBIOS_EP32_OFFSET_STRUCTURE_TABLE_LEN 0x16
#define FU_STRUCT_SMBIOS_EP32_OFFSET_STRUCTURE_TABLE_ADDR 0x18
#define FU_STRUCT_SMBIOS_EP32_OFFSET_NUMBER_SMBIOS_STRUCTS 0x1C
#define FU_STRUCT_SMBIOS_EP32_OFFSET_SMBIOS_BCD_REV 0x1E
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_EP32_SIZE_ANCHOR_STR 0x4
#define FU_STRUCT_SMBIOS_EP32_N_ELEMENTS_ANCHOR_STR 4
#define FU_STRUCT_SMBIOS_EP32_SIZE_INTERMEDIATE_ANCHOR_STR 0x5
#define FU_STRUCT_SMBIOS_EP32_N_ELEMENTS_INTERMEDIATE_ANCHOR_STR 5
#define FU_STRUCT_SMBIOS_EP32_SIZE 0x1F
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructSmbiosEp64;

FuStructSmbiosEp64 *fu_struct_smbios_ep64_ref(FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_unref(FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructSmbiosEp64, fu_struct_smbios_ep64_unref)
FuStructSmbiosEp64 *fu_struct_smbios_ep64_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructSmbiosEp64 *fu_struct_smbios_ep64_parse(const guint8 *buf, gsize bufsz, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gchar *fu_struct_smbios_ep64_get_anchor_str(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep64_get_entry_point_csum(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep64_get_entry_point_len(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep64_get_smbios_major_ver(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep64_get_smbios_minor_ver(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep64_get_smbios_docrev(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep64_get_entry_point_rev(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_ep64_get_reserved0(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_smbios_ep64_get_structure_table_len(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint64 fu_struct_smbios_ep64_get_structure_table_addr(const FuStructSmbiosEp64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gboolean fu_struct_smbios_ep64_set_anchor_str(FuStructSmbiosEp64 *st, const gchar *value, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_smbios_ep64_set_entry_point_csum(FuStructSmbiosEp64 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_set_entry_point_len(FuStructSmbiosEp64 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_set_smbios_major_ver(FuStructSmbiosEp64 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_set_smbios_minor_ver(FuStructSmbiosEp64 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_set_smbios_docrev(FuStructSmbiosEp64 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_set_entry_point_rev(FuStructSmbiosEp64 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_set_reserved0(FuStructSmbiosEp64 *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_set_structure_table_len(FuStructSmbiosEp64 *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_ep64_set_structure_table_addr(FuStructSmbiosEp64 *st, guint64 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_EP64_OFFSET_ANCHOR_STR 0x0
#define FU_STRUCT_SMBIOS_EP64_OFFSET_ENTRY_POINT_CSUM 0x5
#define FU_STRUCT_SMBIOS_EP64_OFFSET_ENTRY_POINT_LEN 0x6
#define FU_STRUCT_SMBIOS_EP64_OFFSET_SMBIOS_MAJOR_VER 0x7
#define FU_STRUCT_SMBIOS_EP64_OFFSET_SMBIOS_MINOR_VER 0x8
#define FU_STRUCT_SMBIOS_EP64_OFFSET_SMBIOS_DOCREV 0x9
#define FU_STRUCT_SMBIOS_EP64_OFFSET_ENTRY_POINT_REV 0xA
#define FU_STRUCT_SMBIOS_EP64_OFFSET_RESERVED0 0xB
#define FU_STRUCT_SMBIOS_EP64_OFFSET_STRUCTURE_TABLE_LEN 0xC
#define FU_STRUCT_SMBIOS_EP64_OFFSET_STRUCTURE_TABLE_ADDR 0x10
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_EP64_SIZE_ANCHOR_STR 0x5
#define FU_STRUCT_SMBIOS_EP64_N_ELEMENTS_ANCHOR_STR 5
#define FU_STRUCT_SMBIOS_EP64_SIZE 0x18
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructSmbiosStructure;

FuStructSmbiosStructure *fu_struct_smbios_structure_ref(FuStructSmbiosStructure *st) G_GNUC_NON_NULL(1);
void fu_struct_smbios_structure_unref(FuStructSmbiosStructure *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructSmbiosStructure, fu_struct_smbios_structure_unref)
FuStructSmbiosStructure *fu_struct_smbios_structure_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructSmbiosStructure *fu_struct_smbios_structure_parse(const guint8 *buf, gsize bufsz, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuStructSmbiosStructure *fu_struct_smbios_structure_parse_bytes(GBytes *blob, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_structure_get_type(const FuStructSmbiosStructure *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_structure_get_length(const FuStructSmbiosStructure *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_smbios_structure_get_handle(const FuStructSmbiosStructure *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_smbios_structure_set_type(FuStructSmbiosStructure *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_structure_set_length(FuStructSmbiosStructure *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_smbios_structure_set_handle(FuStructSmbiosStructure *st, guint16 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_STRUCTURE_OFFSET_TYPE 0x0
#define FU_STRUCT_SMBIOS_STRUCTURE_OFFSET_LENGTH 0x1
#define FU_STRUCT_SMBIOS_STRUCTURE_OFFSET_HANDLE 0x2
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_STRUCTURE_SIZE 0x4
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructSmbiosBiosInformation;

FuStructSmbiosBiosInformation *fu_struct_smbios_bios_information_ref(FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1);
void fu_struct_smbios_bios_information_unref(FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructSmbiosBiosInformation, fu_struct_smbios_bios_information_unref)
FuStructSmbiosBiosInformation *fu_struct_smbios_bios_information_parse_bytes(GBytes *blob, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_bios_information_get_length(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_smbios_bios_information_get_handle(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_bios_information_get_vendor(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_bios_information_get_version(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_smbios_bios_information_get_starting_addr_segment(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_bios_information_get_release_date(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_smbios_bios_information_get_rom_size(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuSmbiosBiosCharacteristics fu_struct_smbios_bios_information_get_characteristics(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuSmbiosBiosCharacteristicsExt fu_struct_smbios_bios_information_get_characteristics_ext(const FuStructSmbiosBiosInformation *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_TYPE 0x0
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_LENGTH 0x1
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_HANDLE 0x2
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_VENDOR 0x4
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_VERSION 0x5
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_STARTING_ADDR_SEGMENT 0x6
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_RELEASE_DATE 0x8
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_ROM_SIZE 0x9
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_CHARACTERISTICS 0xA
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_OFFSET_CHARACTERISTICS_EXT 0x12
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_SIZE 0x14
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_SMBIOS_BIOS_INFORMATION_DEFAULT_TYPE FU_SMBIOS_STRUCTURE_TYPE_BIOS
#endif
