/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fu-input-stream.h>
/**
 * FuEfiStatus:
 *
 * EFI_STATUS is u64le on 64-bit and u32le on 32-bit -- but be pragmatic
 */
typedef enum {
    /**
     * FU_EFI_STATUS_SUCCESS:
     */
    FU_EFI_STATUS_SUCCESS = 0,
    /**
     * FU_EFI_STATUS_LOAD_ERROR:
     */
    FU_EFI_STATUS_LOAD_ERROR = 1,
    /**
     * FU_EFI_STATUS_INVALID_PARAMETER:
     */
    FU_EFI_STATUS_INVALID_PARAMETER = 2,
    /**
     * FU_EFI_STATUS_UNSUPPORTED:
     */
    FU_EFI_STATUS_UNSUPPORTED = 3,
    /**
     * FU_EFI_STATUS_BAD_BUFFER_SIZE:
     */
    FU_EFI_STATUS_BAD_BUFFER_SIZE = 4,
    /**
     * FU_EFI_STATUS_BUFFER_TOO_SMALL:
     */
    FU_EFI_STATUS_BUFFER_TOO_SMALL = 5,
    /**
     * FU_EFI_STATUS_NOT_READY:
     */
    FU_EFI_STATUS_NOT_READY = 6,
    /**
     * FU_EFI_STATUS_DEVICE_ERROR:
     */
    FU_EFI_STATUS_DEVICE_ERROR = 7,
    /**
     * FU_EFI_STATUS_WRITE_PROTECTED:
     */
    FU_EFI_STATUS_WRITE_PROTECTED = 8,
    /**
     * FU_EFI_STATUS_OUT_OF_RESOURCES:
     */
    FU_EFI_STATUS_OUT_OF_RESOURCES = 9,
    /**
     * FU_EFI_STATUS_VOLUME_CORRUPTED:
     */
    FU_EFI_STATUS_VOLUME_CORRUPTED = 10,
    /**
     * FU_EFI_STATUS_VOLUME_FULL:
     */
    FU_EFI_STATUS_VOLUME_FULL = 11,
    /**
     * FU_EFI_STATUS_NO_MEDIA:
     */
    FU_EFI_STATUS_NO_MEDIA = 12,
    /**
     * FU_EFI_STATUS_MEDIA_CHANGED:
     */
    FU_EFI_STATUS_MEDIA_CHANGED = 13,
    /**
     * FU_EFI_STATUS_NOT_FOUND:
     */
    FU_EFI_STATUS_NOT_FOUND = 14,
    /**
     * FU_EFI_STATUS_ACCESS_DENIED:
     */
    FU_EFI_STATUS_ACCESS_DENIED = 15,
    /**
     * FU_EFI_STATUS_NO_RESPONSE:
     */
    FU_EFI_STATUS_NO_RESPONSE = 16,
    /**
     * FU_EFI_STATUS_NO_MAPPING:
     */
    FU_EFI_STATUS_NO_MAPPING = 17,
    /**
     * FU_EFI_STATUS_TIMEOUT:
     */
    FU_EFI_STATUS_TIMEOUT = 18,
    /**
     * FU_EFI_STATUS_NOT_STARTED:
     */
    FU_EFI_STATUS_NOT_STARTED = 19,
    /**
     * FU_EFI_STATUS_ALREADY_STARTED:
     */
    FU_EFI_STATUS_ALREADY_STARTED = 20,
    /**
     * FU_EFI_STATUS_ABORTED:
     */
    FU_EFI_STATUS_ABORTED = 21,
    /**
     * FU_EFI_STATUS_ICMP_ERROR:
     */
    FU_EFI_STATUS_ICMP_ERROR = 22,
    /**
     * FU_EFI_STATUS_TFTP_ERROR:
     */
    FU_EFI_STATUS_TFTP_ERROR = 23,
    /**
     * FU_EFI_STATUS_PROTOCOL_ERROR:
     */
    FU_EFI_STATUS_PROTOCOL_ERROR = 24,
    /**
     * FU_EFI_STATUS_INCOMPATIBLE_VERSION:
     */
    FU_EFI_STATUS_INCOMPATIBLE_VERSION = 25,
    /**
     * FU_EFI_STATUS_SECURITY_VIOLATION:
     */
    FU_EFI_STATUS_SECURITY_VIOLATION = 26,
    /**
     * FU_EFI_STATUS_CRC_ERROR:
     */
    FU_EFI_STATUS_CRC_ERROR = 27,
    /**
     * FU_EFI_STATUS_END_OF_MEDIA:
     */
    FU_EFI_STATUS_END_OF_MEDIA = 28,
    /**
     * FU_EFI_STATUS_END_OF_FILE:
     */
    FU_EFI_STATUS_END_OF_FILE = 31,
    /**
     * FU_EFI_STATUS_INVALID_LANGUAGE:
     */
    FU_EFI_STATUS_INVALID_LANGUAGE = 32,
    /**
     * FU_EFI_STATUS_COMPROMISED_DATA:
     */
    FU_EFI_STATUS_COMPROMISED_DATA = 33,
    /**
     * FU_EFI_STATUS_IP_ADDRESS_CONFLICT:
     */
    FU_EFI_STATUS_IP_ADDRESS_CONFLICT = 34,
    /**
     * FU_EFI_STATUS_HTTP_ERROR:
     */
    FU_EFI_STATUS_HTTP_ERROR = 35,
}  FuEfiStatus;
const gchar *fu_efi_status_to_string(FuEfiStatus val) G_GNUC_CONST;
/**
 * FuEfiSignatureKind:
 */
typedef enum {
    /**
     * FU_EFI_SIGNATURE_KIND_UNKNOWN:
     */
    FU_EFI_SIGNATURE_KIND_UNKNOWN,
    /**
     * FU_EFI_SIGNATURE_KIND_SHA256:
     */
    FU_EFI_SIGNATURE_KIND_SHA256,
    /**
     * FU_EFI_SIGNATURE_KIND_X509:
     */
    FU_EFI_SIGNATURE_KIND_X509,
    /**
     * FU_EFI_SIGNATURE_KIND_EXTERNAL:
     */
    FU_EFI_SIGNATURE_KIND_EXTERNAL,
}  FuEfiSignatureKind;
#define FU_EFI_SIGNATURE_KIND_LAST 4
const gchar *fu_efi_signature_kind_to_string(FuEfiSignatureKind val) G_GNUC_CONST;
FuEfiSignatureKind fu_efi_signature_kind_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
/**
 * FuEfiFileAttrib:
 */
typedef enum {
    /**
     * FU_EFI_FILE_ATTRIB_NONE:
     */
    FU_EFI_FILE_ATTRIB_NONE = 0x00,
    /**
     * FU_EFI_FILE_ATTRIB_LARGE_FILE:
     */
    FU_EFI_FILE_ATTRIB_LARGE_FILE = 0x01,
    /**
     * FU_EFI_FILE_ATTRIB_DATA_ALIGNMENT2:
     */
    FU_EFI_FILE_ATTRIB_DATA_ALIGNMENT2 = 0x02,
    /**
     * FU_EFI_FILE_ATTRIB_FIXED:
     */
    FU_EFI_FILE_ATTRIB_FIXED = 0x04,
    /**
     * FU_EFI_FILE_ATTRIB_DATA_ALIGNMENT:
     */
    FU_EFI_FILE_ATTRIB_DATA_ALIGNMENT = 0x38,
    /**
     * FU_EFI_FILE_ATTRIB_CHECKSUM:
     */
    FU_EFI_FILE_ATTRIB_CHECKSUM = 0x40,
}  FuEfiFileAttrib;
/**
 * FuEfiFileType:
 */
typedef enum {
    /**
     * FU_EFI_FILE_TYPE_ALL:
     */
    FU_EFI_FILE_TYPE_ALL = 0x00,
    /**
     * FU_EFI_FILE_TYPE_RAW:
     */
    FU_EFI_FILE_TYPE_RAW = 0x01,
    /**
     * FU_EFI_FILE_TYPE_FREEFORM:
     */
    FU_EFI_FILE_TYPE_FREEFORM = 0x02,
    /**
     * FU_EFI_FILE_TYPE_SECURITY_CORE:
     */
    FU_EFI_FILE_TYPE_SECURITY_CORE = 0x03,
    /**
     * FU_EFI_FILE_TYPE_PEI_CORE:
     */
    FU_EFI_FILE_TYPE_PEI_CORE = 0x04,
    /**
     * FU_EFI_FILE_TYPE_DXE_CORE:
     */
    FU_EFI_FILE_TYPE_DXE_CORE = 0x05,
    /**
     * FU_EFI_FILE_TYPE_PEIM:
     */
    FU_EFI_FILE_TYPE_PEIM = 0x06,
    /**
     * FU_EFI_FILE_TYPE_DRIVER:
     */
    FU_EFI_FILE_TYPE_DRIVER = 0x07,
    /**
     * FU_EFI_FILE_TYPE_COMBINED_PEIM_DRIVER:
     */
    FU_EFI_FILE_TYPE_COMBINED_PEIM_DRIVER = 0x08,
    /**
     * FU_EFI_FILE_TYPE_APPLICATION:
     */
    FU_EFI_FILE_TYPE_APPLICATION = 0x09,
    /**
     * FU_EFI_FILE_TYPE_MM:
     */
    FU_EFI_FILE_TYPE_MM = 0x0A,
    /**
     * FU_EFI_FILE_TYPE_FIRMWARE_VOLUME_IMAGE:
     */
    FU_EFI_FILE_TYPE_FIRMWARE_VOLUME_IMAGE = 0x0B,
    /**
     * FU_EFI_FILE_TYPE_COMBINED_MM_DXE:
     */
    FU_EFI_FILE_TYPE_COMBINED_MM_DXE = 0x0C,
    /**
     * FU_EFI_FILE_TYPE_MM_CORE:
     */
    FU_EFI_FILE_TYPE_MM_CORE = 0x0D,
    /**
     * FU_EFI_FILE_TYPE_MM_STANDALONE:
     */
    FU_EFI_FILE_TYPE_MM_STANDALONE = 0x0E,
    /**
     * FU_EFI_FILE_TYPE_MM_CORE_STANDALONE:
     */
    FU_EFI_FILE_TYPE_MM_CORE_STANDALONE = 0x0F,
    /**
     * FU_EFI_FILE_TYPE_FFS_PAD:
     */
    FU_EFI_FILE_TYPE_FFS_PAD = 0xF0,
}  FuEfiFileType;
const gchar *fu_efi_file_type_to_string(FuEfiFileType val) G_GNUC_CONST;
/**
 * FuEfiCompressionType:
 */
typedef enum {
    /**
     * FU_EFI_COMPRESSION_TYPE_NOT_COMPRESSED:
     */
    FU_EFI_COMPRESSION_TYPE_NOT_COMPRESSED = 0x00,
    /**
     * FU_EFI_COMPRESSION_TYPE_STANDARD_COMPRESSION:
     */
    FU_EFI_COMPRESSION_TYPE_STANDARD_COMPRESSION = 0x01,
}  FuEfiCompressionType;
/**
 * FuEfiLz77DecompressorVersion:
 */
typedef enum {
    /**
     * FU_EFI_LZ77_DECOMPRESSOR_VERSION_NONE:
     */
    FU_EFI_LZ77_DECOMPRESSOR_VERSION_NONE,
    /**
     * FU_EFI_LZ77_DECOMPRESSOR_VERSION_LEGACY:
     */
    FU_EFI_LZ77_DECOMPRESSOR_VERSION_LEGACY,
    /**
     * FU_EFI_LZ77_DECOMPRESSOR_VERSION_TIANO:
     */
    FU_EFI_LZ77_DECOMPRESSOR_VERSION_TIANO,
}  FuEfiLz77DecompressorVersion;
#define FU_EFI_LZ77_DECOMPRESSOR_VERSION_LAST 3
const gchar *fu_efi_lz77_decompressor_version_to_string(FuEfiLz77DecompressorVersion val) G_GNUC_CONST;
/**
 * FuEfiSectionType:
 */
typedef enum {
    /**
     * FU_EFI_SECTION_TYPE_COMPRESSION:
     *
     * encapsulation section type values
     */
    FU_EFI_SECTION_TYPE_COMPRESSION = 0x01,
    /**
     * FU_EFI_SECTION_TYPE_GUID_DEFINED:
     */
    FU_EFI_SECTION_TYPE_GUID_DEFINED = 0x02,
    /**
     * FU_EFI_SECTION_TYPE_DISPOSABLE:
     */
    FU_EFI_SECTION_TYPE_DISPOSABLE = 0x03,
    /**
     * FU_EFI_SECTION_TYPE_PE32:
     *
     * leaf section type values
     */
    FU_EFI_SECTION_TYPE_PE32 = 0x10,
    /**
     * FU_EFI_SECTION_TYPE_PIC:
     */
    FU_EFI_SECTION_TYPE_PIC = 0x11,
    /**
     * FU_EFI_SECTION_TYPE_TE:
     */
    FU_EFI_SECTION_TYPE_TE = 0x12,
    /**
     * FU_EFI_SECTION_TYPE_DXE_DEPEX:
     */
    FU_EFI_SECTION_TYPE_DXE_DEPEX = 0x13,
    /**
     * FU_EFI_SECTION_TYPE_VERSION:
     */
    FU_EFI_SECTION_TYPE_VERSION = 0x14,
    /**
     * FU_EFI_SECTION_TYPE_USER_INTERFACE:
     */
    FU_EFI_SECTION_TYPE_USER_INTERFACE = 0x15,
    /**
     * FU_EFI_SECTION_TYPE_COMPATIBILITY16:
     */
    FU_EFI_SECTION_TYPE_COMPATIBILITY16 = 0x16,
    /**
     * FU_EFI_SECTION_TYPE_VOLUME_IMAGE:
     */
    FU_EFI_SECTION_TYPE_VOLUME_IMAGE = 0x17,
    /**
     * FU_EFI_SECTION_TYPE_FREEFORM_SUBTYPE_GUID:
     */
    FU_EFI_SECTION_TYPE_FREEFORM_SUBTYPE_GUID = 0x18,
    /**
     * FU_EFI_SECTION_TYPE_RAW:
     */
    FU_EFI_SECTION_TYPE_RAW = 0x19,
    /**
     * FU_EFI_SECTION_TYPE_PEI_DEPEX:
     */
    FU_EFI_SECTION_TYPE_PEI_DEPEX = 0x1B,
    /**
     * FU_EFI_SECTION_TYPE_MM_DEPEX:
     */
    FU_EFI_SECTION_TYPE_MM_DEPEX = 0x1C,
    /**
     * FU_EFI_SECTION_TYPE_PHOENIX_SECTION_POSTCODE:
     *
     * vendor-specific
     * Phoenix SCT
     */
    FU_EFI_SECTION_TYPE_PHOENIX_SECTION_POSTCODE = 0xF0,
    /**
     * FU_EFI_SECTION_TYPE_INSYDE_SECTION_POSTCODE:
     *
     * Insyde H2O
     */
    FU_EFI_SECTION_TYPE_INSYDE_SECTION_POSTCODE = 0x20,
}  FuEfiSectionType;
const gchar *fu_efi_section_type_to_string(FuEfiSectionType val) G_GNUC_CONST;
/**
 * FuEfiVolumeExtEntryType:
 */
typedef enum {
    /**
     * FU_EFI_VOLUME_EXT_ENTRY_TYPE_OEM:
     */
    FU_EFI_VOLUME_EXT_ENTRY_TYPE_OEM = 0x01,
    /**
     * FU_EFI_VOLUME_EXT_ENTRY_TYPE_GUID:
     */
    FU_EFI_VOLUME_EXT_ENTRY_TYPE_GUID = 0x02,
    /**
     * FU_EFI_VOLUME_EXT_ENTRY_TYPE_SIZE:
     */
    FU_EFI_VOLUME_EXT_ENTRY_TYPE_SIZE = 0x03,
}  FuEfiVolumeExtEntryType;
const gchar *fu_efi_volume_ext_entry_type_to_string(FuEfiVolumeExtEntryType val) G_GNUC_CONST;
/**
 * FuEfiLoadOptionAttrs:
 */
typedef enum {
    /**
     * FU_EFI_LOAD_OPTION_ATTR_ACTIVE:
     */
    FU_EFI_LOAD_OPTION_ATTR_ACTIVE = 0x1,
    /**
     * FU_EFI_LOAD_OPTION_ATTR_FORCE_RECONNECT:
     */
    FU_EFI_LOAD_OPTION_ATTR_FORCE_RECONNECT = 0x2,
    /**
     * FU_EFI_LOAD_OPTION_ATTR_HIDDEN:
     */
    FU_EFI_LOAD_OPTION_ATTR_HIDDEN = 0x8,
    /**
     * FU_EFI_LOAD_OPTION_ATTR_CATEGORY:
     */
    FU_EFI_LOAD_OPTION_ATTR_CATEGORY = 0x1F00,
    /**
     * FU_EFI_LOAD_OPTION_ATTR_CATEGORY_BOOT:
     */
    FU_EFI_LOAD_OPTION_ATTR_CATEGORY_BOOT = 0x0,
    /**
     * FU_EFI_LOAD_OPTION_ATTR_CATEGORY_AP:
     */
    FU_EFI_LOAD_OPTION_ATTR_CATEGORY_AP = 0x100,
}  FuEfiLoadOptionAttrs;
/**
 * FuEfiLoadOptionKind:
 */
typedef enum {
    /**
     * FU_EFI_LOAD_OPTION_KIND_UNKNOWN:
     */
    FU_EFI_LOAD_OPTION_KIND_UNKNOWN,
    /**
     * FU_EFI_LOAD_OPTION_KIND_PATH:
     */
    FU_EFI_LOAD_OPTION_KIND_PATH,
    /**
     * FU_EFI_LOAD_OPTION_KIND_HIVE:
     */
    FU_EFI_LOAD_OPTION_KIND_HIVE,
    /**
     * FU_EFI_LOAD_OPTION_KIND_DATA:
     */
    FU_EFI_LOAD_OPTION_KIND_DATA,
}  FuEfiLoadOptionKind;
#define FU_EFI_LOAD_OPTION_KIND_LAST 4
const gchar *fu_efi_load_option_kind_to_string(FuEfiLoadOptionKind val) G_GNUC_CONST;
FuEfiLoadOptionKind fu_efi_load_option_kind_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
/**
 * FuEfiDevicePathType:
 */
typedef enum {
    /**
     * FU_EFI_DEVICE_PATH_TYPE_HARDWARE:
     */
    FU_EFI_DEVICE_PATH_TYPE_HARDWARE = 0x01,
    /**
     * FU_EFI_DEVICE_PATH_TYPE_ACPI:
     */
    FU_EFI_DEVICE_PATH_TYPE_ACPI,
    /**
     * FU_EFI_DEVICE_PATH_TYPE_MESSAGE:
     */
    FU_EFI_DEVICE_PATH_TYPE_MESSAGE,
    /**
     * FU_EFI_DEVICE_PATH_TYPE_MEDIA:
     */
    FU_EFI_DEVICE_PATH_TYPE_MEDIA,
    /**
     * FU_EFI_DEVICE_PATH_TYPE_BIOS_BOOT:
     */
    FU_EFI_DEVICE_PATH_TYPE_BIOS_BOOT,
    /**
     * FU_EFI_DEVICE_PATH_TYPE_END:
     */
    FU_EFI_DEVICE_PATH_TYPE_END = 0x7F,
}  FuEfiDevicePathType;
/**
 * FuEfiHardDriveDevicePathSubtype:
 */
typedef enum {
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_HARD_DRIVE:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_HARD_DRIVE = 0x01,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_CDROM:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_CDROM = 0x02,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_VENDOR:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_VENDOR = 0x03,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_FILE_PATH:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_FILE_PATH = 0x04,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_MEDIA_PROTOCOL:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_MEDIA_PROTOCOL = 0x05,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_PIWG_FIRMWARE_FILE:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_PIWG_FIRMWARE_FILE = 0x06,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_PIWG_FIRMWARE_VOLUME:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_PIWG_FIRMWARE_VOLUME = 0x07,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_RELATIVE_OFFSET_RANGE:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_RELATIVE_OFFSET_RANGE = 0x08,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_RAM_DISK_DEVICE_PATH:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_RAM_DISK_DEVICE_PATH = 0x09,
}  FuEfiHardDriveDevicePathSubtype;
/**
 * FuEfiHardDriveDevicePathPartitionFormat:
 */
typedef enum {
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_PARTITION_FORMAT_LEGACY_MBR:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_PARTITION_FORMAT_LEGACY_MBR = 0x01,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_PARTITION_FORMAT_GUID_PARTITION_TABLE:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_PARTITION_FORMAT_GUID_PARTITION_TABLE = 0x02,
}  FuEfiHardDriveDevicePathPartitionFormat;
const gchar *fu_efi_hard_drive_device_path_partition_format_to_string(FuEfiHardDriveDevicePathPartitionFormat val) G_GNUC_CONST;
FuEfiHardDriveDevicePathPartitionFormat fu_efi_hard_drive_device_path_partition_format_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
/**
 * FuEfiHardDriveDevicePathSignatureType:
 */
typedef enum {
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SIGNATURE_TYPE_NONE:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SIGNATURE_TYPE_NONE,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SIGNATURE_TYPE_ADDR1B8:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SIGNATURE_TYPE_ADDR1B8,
    /**
     * FU_EFI_HARD_DRIVE_DEVICE_PATH_SIGNATURE_TYPE_GUID:
     */
    FU_EFI_HARD_DRIVE_DEVICE_PATH_SIGNATURE_TYPE_GUID,
}  FuEfiHardDriveDevicePathSignatureType;
#define FU_EFI_HARD_DRIVE_DEVICE_PATH_SIGNATURE_TYPE_LAST 3
const gchar *fu_efi_hard_drive_device_path_signature_type_to_string(FuEfiHardDriveDevicePathSignatureType val) G_GNUC_CONST;
FuEfiHardDriveDevicePathSignatureType fu_efi_hard_drive_device_path_signature_type_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
/**
 * FuEfiVariableStoreState:
 */
typedef enum {
    /**
     * FU_EFI_VARIABLE_STORE_STATE_UNSET:
     */
    FU_EFI_VARIABLE_STORE_STATE_UNSET = 0x00,
    /**
     * FU_EFI_VARIABLE_STORE_STATE_HEALTHY:
     */
    FU_EFI_VARIABLE_STORE_STATE_HEALTHY = 0xFE,
    /**
     * FU_EFI_VARIABLE_STORE_STATE_EMPTY:
     */
    FU_EFI_VARIABLE_STORE_STATE_EMPTY = 0xFF,
}  FuEfiVariableStoreState;
const gchar *fu_efi_variable_store_state_to_string(FuEfiVariableStoreState val) G_GNUC_CONST;
FuEfiVariableStoreState fu_efi_variable_store_state_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
/**
 * FuEfiVariableStoreFormat:
 */
typedef enum {
    /**
     * FU_EFI_VARIABLE_STORE_FORMAT_FORMATTED:
     */
    FU_EFI_VARIABLE_STORE_FORMAT_FORMATTED = 0x5A,
}  FuEfiVariableStoreFormat;
/**
 * FuEfiVariableState:
 */
typedef enum {
    /**
     * FU_EFI_VARIABLE_STATE_UNSET:
     */
    FU_EFI_VARIABLE_STATE_UNSET = 0x00,
    /**
     * FU_EFI_VARIABLE_STATE_VARIABLE_IN_DELETED_TRANSITION:
     */
    FU_EFI_VARIABLE_STATE_VARIABLE_IN_DELETED_TRANSITION = 0xFE,
    /**
     * FU_EFI_VARIABLE_STATE_VARIABLE_DELETED:
     */
    FU_EFI_VARIABLE_STATE_VARIABLE_DELETED = 0xFD,
    /**
     * FU_EFI_VARIABLE_STATE_VARIABLE_HEADER_VALID:
     */
    FU_EFI_VARIABLE_STATE_VARIABLE_HEADER_VALID = 0x7F,
    /**
     * FU_EFI_VARIABLE_STATE_VARIABLE_ADDED:
     */
    FU_EFI_VARIABLE_STATE_VARIABLE_ADDED = 0x3F,
    /**
     * FU_EFI_VARIABLE_STATE_INTEL_VARIABLE_VALID:
     */
    FU_EFI_VARIABLE_STATE_INTEL_VARIABLE_VALID = 0xFC,
    /**
     * FU_EFI_VARIABLE_STATE_INTEL_VARIABLE_INVALID:
     */
    FU_EFI_VARIABLE_STATE_INTEL_VARIABLE_INVALID = 0xF8,
}  FuEfiVariableState;
const gchar *fu_efi_variable_state_to_string(FuEfiVariableState val) G_GNUC_CONST;
FuEfiVariableState fu_efi_variable_state_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
/**
 * FuEfiVariableAttrs:
 */
typedef enum {
    /**
     * FU_EFI_VARIABLE_ATTR_NONE:
     */
    FU_EFI_VARIABLE_ATTR_NONE = 0x00000000,
    /**
     * FU_EFI_VARIABLE_ATTR_NON_VOLATILE:
     */
    FU_EFI_VARIABLE_ATTR_NON_VOLATILE = 0x00000001,
    /**
     * FU_EFI_VARIABLE_ATTR_BOOTSERVICE_ACCESS:
     */
    FU_EFI_VARIABLE_ATTR_BOOTSERVICE_ACCESS = 0x00000002,
    /**
     * FU_EFI_VARIABLE_ATTR_RUNTIME_ACCESS:
     */
    FU_EFI_VARIABLE_ATTR_RUNTIME_ACCESS = 0x00000004,
    /**
     * FU_EFI_VARIABLE_ATTR_HARDWARE_ERROR_RECORD:
     */
    FU_EFI_VARIABLE_ATTR_HARDWARE_ERROR_RECORD = 0x00000008,
    /**
     * FU_EFI_VARIABLE_ATTR_AUTHENTICATED_WRITE_ACCESS:
     */
    FU_EFI_VARIABLE_ATTR_AUTHENTICATED_WRITE_ACCESS = 0x00000010,
    /**
     * FU_EFI_VARIABLE_ATTR_TIME_BASED_AUTHENTICATED_WRITE_ACCESS:
     */
    FU_EFI_VARIABLE_ATTR_TIME_BASED_AUTHENTICATED_WRITE_ACCESS = 0x00000020,
    /**
     * FU_EFI_VARIABLE_ATTR_APPEND_WRITE:
     */
    FU_EFI_VARIABLE_ATTR_APPEND_WRITE = 0x00000040,
} G_GNUC_FLAG_ENUM FuEfiVariableAttrs;
gchar *fu_efi_variable_attrs_to_string(FuEfiVariableAttrs val) G_GNUC_CONST;
FuEfiVariableAttrs fu_efi_variable_attrs_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
/**
 * FuEfiCapsuleHeaderFlags:
 */
typedef enum {
    /**
     * FU_EFI_CAPSULE_HEADER_FLAG_PERSIST_ACROSS_RESET:
     */
    FU_EFI_CAPSULE_HEADER_FLAG_PERSIST_ACROSS_RESET = 1 << 16,
    /**
     * FU_EFI_CAPSULE_HEADER_FLAG_POPULATE_SYSTEM_TABLE:
     */
    FU_EFI_CAPSULE_HEADER_FLAG_POPULATE_SYSTEM_TABLE = 1 << 17,
    /**
     * FU_EFI_CAPSULE_HEADER_FLAG_INITIATE_RESET:
     */
    FU_EFI_CAPSULE_HEADER_FLAG_INITIATE_RESET = 1 << 18,
} G_GNUC_FLAG_ENUM FuEfiCapsuleHeaderFlags;
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiFile;

FuStructEfiFile *fu_struct_efi_file_ref(FuStructEfiFile *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_file_unref(FuStructEfiFile *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiFile, fu_struct_efi_file_unref)
FuStructEfiFile *fu_struct_efi_file_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiFile *fu_struct_efi_file_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gboolean fu_struct_efi_file_validate(const guint8 *buf, gsize bufsz, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_file_get_name(const FuStructEfiFile *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_file_get_hdr_checksum(const FuStructEfiFile *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_file_get_data_checksum(const FuStructEfiFile *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiFileType fu_struct_efi_file_get_type(const FuStructEfiFile *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_file_get_attrs(const FuStructEfiFile *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_file_get_size(const FuStructEfiFile *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_file_get_state(const FuStructEfiFile *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_file_set_name(FuStructEfiFile *st, const fwupd_guid_t *value) G_GNUC_NON_NULL(1, 2);
void fu_struct_efi_file_set_hdr_checksum(FuStructEfiFile *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_file_set_data_checksum(FuStructEfiFile *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_file_set_type(FuStructEfiFile *st, FuEfiFileType value) G_GNUC_NON_NULL(1);
void fu_struct_efi_file_set_attrs(FuStructEfiFile *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_file_set_size(FuStructEfiFile *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_file_set_state(FuStructEfiFile *st, guint8 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_FILE_OFFSET_NAME 0x0
#define FU_STRUCT_EFI_FILE_OFFSET_HDR_CHECKSUM 0x10
#define FU_STRUCT_EFI_FILE_OFFSET_DATA_CHECKSUM 0x11
#define FU_STRUCT_EFI_FILE_OFFSET_TYPE 0x12
#define FU_STRUCT_EFI_FILE_OFFSET_ATTRS 0x13
#define FU_STRUCT_EFI_FILE_OFFSET_SIZE 0x14
#define FU_STRUCT_EFI_FILE_OFFSET_STATE 0x17
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_FILE_SIZE 0x18
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_FILE_DEFAULT_STATE 0xF8
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiFile2;

FuStructEfiFile2 *fu_struct_efi_file2_ref(FuStructEfiFile2 *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_file2_unref(FuStructEfiFile2 *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiFile2, fu_struct_efi_file2_unref)
FuStructEfiFile2 *fu_struct_efi_file2_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint64 fu_struct_efi_file2_get_extended_size(const FuStructEfiFile2 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_FILE2_OFFSET_EXTENDED_SIZE 0x18
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_FILE2_SIZE 0x20
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiSectionCompression;

FuStructEfiSectionCompression *fu_struct_efi_section_compression_ref(FuStructEfiSectionCompression *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_section_compression_unref(FuStructEfiSectionCompression *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiSectionCompression, fu_struct_efi_section_compression_unref)
FuStructEfiSectionCompression *fu_struct_efi_section_compression_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_section_compression_get_uncompressed_length(const FuStructEfiSectionCompression *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiCompressionType fu_struct_efi_section_compression_get_compression_type(const FuStructEfiSectionCompression *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION_COMPRESSION_OFFSET_UNCOMPRESSED_LENGTH 0x0
#define FU_STRUCT_EFI_SECTION_COMPRESSION_OFFSET_COMPRESSION_TYPE 0x4
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION_COMPRESSION_SIZE 0x5
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiLz77DecompressorHeader;

FuStructEfiLz77DecompressorHeader *fu_struct_efi_lz77_decompressor_header_ref(FuStructEfiLz77DecompressorHeader *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_lz77_decompressor_header_unref(FuStructEfiLz77DecompressorHeader *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiLz77DecompressorHeader, fu_struct_efi_lz77_decompressor_header_unref)
FuStructEfiLz77DecompressorHeader *fu_struct_efi_lz77_decompressor_header_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_lz77_decompressor_header_get_src_size(const FuStructEfiLz77DecompressorHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_lz77_decompressor_header_get_dst_size(const FuStructEfiLz77DecompressorHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_LZ77_DECOMPRESSOR_HEADER_OFFSET_SRC_SIZE 0x0
#define FU_STRUCT_EFI_LZ77_DECOMPRESSOR_HEADER_OFFSET_DST_SIZE 0x4
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_LZ77_DECOMPRESSOR_HEADER_SIZE 0x8
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiSection;

FuStructEfiSection *fu_struct_efi_section_ref(FuStructEfiSection *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_section_unref(FuStructEfiSection *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiSection, fu_struct_efi_section_unref)
FuStructEfiSection *fu_struct_efi_section_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiSection *fu_struct_efi_section_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_section_get_size(const FuStructEfiSection *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiSectionType fu_struct_efi_section_get_type(const FuStructEfiSection *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_section_set_size(FuStructEfiSection *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_section_set_type(FuStructEfiSection *st, FuEfiSectionType value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION_OFFSET_SIZE 0x0
#define FU_STRUCT_EFI_SECTION_OFFSET_TYPE 0x3
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION_SIZE 0x4
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiSection2;

FuStructEfiSection2 *fu_struct_efi_section2_ref(FuStructEfiSection2 *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_section2_unref(FuStructEfiSection2 *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiSection2, fu_struct_efi_section2_unref)
FuStructEfiSection2 *fu_struct_efi_section2_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiSectionType fu_struct_efi_section2_get_type(const FuStructEfiSection2 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_section2_get_extended_size(const FuStructEfiSection2 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION2_OFFSET_SIZE 0x0
#define FU_STRUCT_EFI_SECTION2_OFFSET_TYPE 0x3
#define FU_STRUCT_EFI_SECTION2_OFFSET_EXTENDED_SIZE 0x4
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION2_SIZE 0x8
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION2_DEFAULT_SIZE 0xFFFFFF
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiSectionFreeformSubtypeGuid;

FuStructEfiSectionFreeformSubtypeGuid *fu_struct_efi_section_freeform_subtype_guid_ref(FuStructEfiSectionFreeformSubtypeGuid *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_section_freeform_subtype_guid_unref(FuStructEfiSectionFreeformSubtypeGuid *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiSectionFreeformSubtypeGuid, fu_struct_efi_section_freeform_subtype_guid_unref)
FuStructEfiSectionFreeformSubtypeGuid *fu_struct_efi_section_freeform_subtype_guid_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_section_freeform_subtype_guid_get_guid(const FuStructEfiSectionFreeformSubtypeGuid *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION_FREEFORM_SUBTYPE_GUID_OFFSET_GUID 0x0
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION_FREEFORM_SUBTYPE_GUID_SIZE 0x10
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiSectionGuidDefined;

FuStructEfiSectionGuidDefined *fu_struct_efi_section_guid_defined_ref(FuStructEfiSectionGuidDefined *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_section_guid_defined_unref(FuStructEfiSectionGuidDefined *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiSectionGuidDefined, fu_struct_efi_section_guid_defined_unref)
FuStructEfiSectionGuidDefined *fu_struct_efi_section_guid_defined_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiSectionGuidDefined *fu_struct_efi_section_guid_defined_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_section_guid_defined_get_name(const FuStructEfiSectionGuidDefined *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_section_guid_defined_get_offset(const FuStructEfiSectionGuidDefined *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_section_guid_defined_get_attr(const FuStructEfiSectionGuidDefined *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_section_guid_defined_set_name(FuStructEfiSectionGuidDefined *st, const fwupd_guid_t *value) G_GNUC_NON_NULL(1, 2);
void fu_struct_efi_section_guid_defined_set_offset(FuStructEfiSectionGuidDefined *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_section_guid_defined_set_attr(FuStructEfiSectionGuidDefined *st, guint16 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION_GUID_DEFINED_OFFSET_NAME 0x0
#define FU_STRUCT_EFI_SECTION_GUID_DEFINED_OFFSET_OFFSET 0x10
#define FU_STRUCT_EFI_SECTION_GUID_DEFINED_OFFSET_ATTR 0x12
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SECTION_GUID_DEFINED_SIZE 0x14
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiVolume;

FuStructEfiVolume *fu_struct_efi_volume_ref(FuStructEfiVolume *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_unref(FuStructEfiVolume *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiVolume, fu_struct_efi_volume_unref)
FuStructEfiVolume *fu_struct_efi_volume_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiVolume *fu_struct_efi_volume_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gboolean fu_struct_efi_volume_validate_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_volume_get_zero_vector(const FuStructEfiVolume *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_volume_get_guid(const FuStructEfiVolume *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint64 fu_struct_efi_volume_get_length(const FuStructEfiVolume *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_volume_get_attrs(const FuStructEfiVolume *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_volume_get_hdr_len(const FuStructEfiVolume *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_volume_get_checksum(const FuStructEfiVolume *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_volume_get_ext_hdr(const FuStructEfiVolume *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_volume_set_zero_vector(FuStructEfiVolume *st, const fwupd_guid_t *value) G_GNUC_NON_NULL(1, 2);
void fu_struct_efi_volume_set_guid(FuStructEfiVolume *st, const fwupd_guid_t *value) G_GNUC_NON_NULL(1, 2);
void fu_struct_efi_volume_set_length(FuStructEfiVolume *st, guint64 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_set_attrs(FuStructEfiVolume *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_set_hdr_len(FuStructEfiVolume *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_set_checksum(FuStructEfiVolume *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_set_ext_hdr(FuStructEfiVolume *st, guint16 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_OFFSET_ZERO_VECTOR 0x0
#define FU_STRUCT_EFI_VOLUME_OFFSET_GUID 0x10
#define FU_STRUCT_EFI_VOLUME_OFFSET_LENGTH 0x20
#define FU_STRUCT_EFI_VOLUME_OFFSET_SIGNATURE 0x28
#define FU_STRUCT_EFI_VOLUME_OFFSET_ATTRS 0x2C
#define FU_STRUCT_EFI_VOLUME_OFFSET_HDR_LEN 0x30
#define FU_STRUCT_EFI_VOLUME_OFFSET_CHECKSUM 0x32
#define FU_STRUCT_EFI_VOLUME_OFFSET_EXT_HDR 0x34
#define FU_STRUCT_EFI_VOLUME_OFFSET_REVISION 0x37
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_SIZE 0x38
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_DEFAULT_SIGNATURE 0x4856465F
#define FU_STRUCT_EFI_VOLUME_DEFAULT_REVISION 0x02
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiVolumeExtHeader;

FuStructEfiVolumeExtHeader *fu_struct_efi_volume_ext_header_ref(FuStructEfiVolumeExtHeader *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_ext_header_unref(FuStructEfiVolumeExtHeader *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiVolumeExtHeader, fu_struct_efi_volume_ext_header_unref)
FuStructEfiVolumeExtHeader *fu_struct_efi_volume_ext_header_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_volume_ext_header_get_fv_name(const FuStructEfiVolumeExtHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_volume_ext_header_get_size(const FuStructEfiVolumeExtHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_EXT_HEADER_OFFSET_FV_NAME 0x0
#define FU_STRUCT_EFI_VOLUME_EXT_HEADER_OFFSET_SIZE 0x10
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_EXT_HEADER_SIZE 0x14
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiVolumeExtEntry;

FuStructEfiVolumeExtEntry *fu_struct_efi_volume_ext_entry_ref(FuStructEfiVolumeExtEntry *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_ext_entry_unref(FuStructEfiVolumeExtEntry *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiVolumeExtEntry, fu_struct_efi_volume_ext_entry_unref)
FuStructEfiVolumeExtEntry *fu_struct_efi_volume_ext_entry_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_volume_ext_entry_get_size(const FuStructEfiVolumeExtEntry *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiVolumeExtEntryType fu_struct_efi_volume_ext_entry_get_type(const FuStructEfiVolumeExtEntry *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_EXT_ENTRY_OFFSET_SIZE 0x0
#define FU_STRUCT_EFI_VOLUME_EXT_ENTRY_OFFSET_TYPE 0x2
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_EXT_ENTRY_SIZE 0x4
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiVolumeBlockMap;

FuStructEfiVolumeBlockMap *fu_struct_efi_volume_block_map_ref(FuStructEfiVolumeBlockMap *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_block_map_unref(FuStructEfiVolumeBlockMap *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiVolumeBlockMap, fu_struct_efi_volume_block_map_unref)
FuStructEfiVolumeBlockMap *fu_struct_efi_volume_block_map_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiVolumeBlockMap *fu_struct_efi_volume_block_map_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_volume_block_map_get_num_blocks(const FuStructEfiVolumeBlockMap *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_volume_block_map_get_length(const FuStructEfiVolumeBlockMap *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_volume_block_map_set_num_blocks(FuStructEfiVolumeBlockMap *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_volume_block_map_set_length(FuStructEfiVolumeBlockMap *st, guint32 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_BLOCK_MAP_OFFSET_NUM_BLOCKS 0x0
#define FU_STRUCT_EFI_VOLUME_BLOCK_MAP_OFFSET_LENGTH 0x4
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VOLUME_BLOCK_MAP_SIZE 0x8
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiTime;

FuStructEfiTime *fu_struct_efi_time_ref(FuStructEfiTime *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_unref(FuStructEfiTime *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiTime, fu_struct_efi_time_unref)
FuStructEfiTime *fu_struct_efi_time_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiTime *fu_struct_efi_time_new_internal(void) G_GNUC_WARN_UNUSED_RESULT;
gchar *fu_struct_efi_time_to_string(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_time_get_year(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_time_get_month(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_time_get_day(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_time_get_hour(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_time_get_minute(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_time_get_second(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_time_get_nanosecond(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_time_get_timezone(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_time_get_daylight(const FuStructEfiTime *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_time_set_year(FuStructEfiTime *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_set_month(FuStructEfiTime *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_set_day(FuStructEfiTime *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_set_hour(FuStructEfiTime *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_set_minute(FuStructEfiTime *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_set_second(FuStructEfiTime *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_set_nanosecond(FuStructEfiTime *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_set_timezone(FuStructEfiTime *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_time_set_daylight(FuStructEfiTime *st, guint8 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_TIME_OFFSET_YEAR 0x0
#define FU_STRUCT_EFI_TIME_OFFSET_MONTH 0x2
#define FU_STRUCT_EFI_TIME_OFFSET_DAY 0x3
#define FU_STRUCT_EFI_TIME_OFFSET_HOUR 0x4
#define FU_STRUCT_EFI_TIME_OFFSET_MINUTE 0x5
#define FU_STRUCT_EFI_TIME_OFFSET_SECOND 0x6
#define FU_STRUCT_EFI_TIME_OFFSET_NANOSECOND 0x8
#define FU_STRUCT_EFI_TIME_OFFSET_TIMEZONE 0xC
#define FU_STRUCT_EFI_TIME_OFFSET_DAYLIGHT 0xE
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_TIME_SIZE 0x10
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiWinCertificate;

FuStructEfiWinCertificate *fu_struct_efi_win_certificate_ref(FuStructEfiWinCertificate *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_win_certificate_unref(FuStructEfiWinCertificate *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiWinCertificate, fu_struct_efi_win_certificate_unref)
guint32 fu_struct_efi_win_certificate_get_length(const FuStructEfiWinCertificate *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_win_certificate_set_length(FuStructEfiWinCertificate *st, guint32 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_WIN_CERTIFICATE_OFFSET_LENGTH 0x0
#define FU_STRUCT_EFI_WIN_CERTIFICATE_OFFSET_REVISION 0x4
#define FU_STRUCT_EFI_WIN_CERTIFICATE_OFFSET_CERTIFICATE_TYPE 0x6
#define FU_STRUCT_EFI_WIN_CERTIFICATE_OFFSET_GUID 0x8
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_WIN_CERTIFICATE_SIZE 0x18
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_WIN_CERTIFICATE_DEFAULT_LENGTH 24
#define FU_STRUCT_EFI_WIN_CERTIFICATE_DEFAULT_REVISION 0x0200
#define FU_STRUCT_EFI_WIN_CERTIFICATE_DEFAULT_CERTIFICATE_TYPE 0x0EF1
#define FU_STRUCT_EFI_WIN_CERTIFICATE_DEFAULT_GUID "\x9d\xd2\xaf\x4a\xdf\x68\xee\x49\x8a\xa9\x34\x7d\x37\x56\x65\xa7"
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiVariableAuthentication2;

FuStructEfiVariableAuthentication2 *fu_struct_efi_variable_authentication2_ref(FuStructEfiVariableAuthentication2 *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_variable_authentication2_unref(FuStructEfiVariableAuthentication2 *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiVariableAuthentication2, fu_struct_efi_variable_authentication2_unref)
FuStructEfiVariableAuthentication2 *fu_struct_efi_variable_authentication2_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiVariableAuthentication2 *fu_struct_efi_variable_authentication2_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gboolean fu_struct_efi_variable_authentication2_validate_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiTime *fu_struct_efi_variable_authentication2_get_timestamp(const FuStructEfiVariableAuthentication2 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiWinCertificate *fu_struct_efi_variable_authentication2_get_auth_info(const FuStructEfiVariableAuthentication2 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gboolean
fu_struct_efi_variable_authentication2_set_timestamp(FuStructEfiVariableAuthentication2 *st, const FuStructEfiTime *st_donor, GError **error) G_GNUC_NON_NULL(1, 2) G_GNUC_WARN_UNUSED_RESULT;
gboolean
fu_struct_efi_variable_authentication2_set_auth_info(FuStructEfiVariableAuthentication2 *st, const FuStructEfiWinCertificate *st_donor, GError **error) G_GNUC_NON_NULL(1, 2) G_GNUC_WARN_UNUSED_RESULT;

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VARIABLE_AUTHENTICATION2_OFFSET_TIMESTAMP 0x0
#define FU_STRUCT_EFI_VARIABLE_AUTHENTICATION2_OFFSET_AUTH_INFO 0x10
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VARIABLE_AUTHENTICATION2_SIZE 0x28
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiSignatureList;

FuStructEfiSignatureList *fu_struct_efi_signature_list_ref(FuStructEfiSignatureList *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_signature_list_unref(FuStructEfiSignatureList *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiSignatureList, fu_struct_efi_signature_list_unref)
FuStructEfiSignatureList *fu_struct_efi_signature_list_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiSignatureList *fu_struct_efi_signature_list_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_signature_list_get_type(const FuStructEfiSignatureList *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_signature_list_get_list_size(const FuStructEfiSignatureList *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_signature_list_get_header_size(const FuStructEfiSignatureList *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_signature_list_get_size(const FuStructEfiSignatureList *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_signature_list_set_type(FuStructEfiSignatureList *st, const fwupd_guid_t *value) G_GNUC_NON_NULL(1, 2);
void fu_struct_efi_signature_list_set_list_size(FuStructEfiSignatureList *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_signature_list_set_header_size(FuStructEfiSignatureList *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_signature_list_set_size(FuStructEfiSignatureList *st, guint32 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SIGNATURE_LIST_OFFSET_TYPE 0x0
#define FU_STRUCT_EFI_SIGNATURE_LIST_OFFSET_LIST_SIZE 0x10
#define FU_STRUCT_EFI_SIGNATURE_LIST_OFFSET_HEADER_SIZE 0x14
#define FU_STRUCT_EFI_SIGNATURE_LIST_OFFSET_SIZE 0x18
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_SIGNATURE_LIST_SIZE 0x1C
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiLoadOption;

FuStructEfiLoadOption *fu_struct_efi_load_option_ref(FuStructEfiLoadOption *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_load_option_unref(FuStructEfiLoadOption *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiLoadOption, fu_struct_efi_load_option_unref)
FuStructEfiLoadOption *fu_struct_efi_load_option_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiLoadOption *fu_struct_efi_load_option_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiLoadOptionAttrs fu_struct_efi_load_option_get_attrs(const FuStructEfiLoadOption *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_load_option_get_dp_size(const FuStructEfiLoadOption *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_load_option_set_attrs(FuStructEfiLoadOption *st, FuEfiLoadOptionAttrs value) G_GNUC_NON_NULL(1);
void fu_struct_efi_load_option_set_dp_size(FuStructEfiLoadOption *st, guint16 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_LOAD_OPTION_OFFSET_ATTRS 0x0
#define FU_STRUCT_EFI_LOAD_OPTION_OFFSET_DP_SIZE 0x4
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_LOAD_OPTION_SIZE 0x6
#endif

#ifndef __GI_SCANNER__
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiDevicePath;

FuStructEfiDevicePath *fu_struct_efi_device_path_ref(FuStructEfiDevicePath *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_device_path_unref(FuStructEfiDevicePath *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiDevicePath, fu_struct_efi_device_path_unref)
FuStructEfiDevicePath *fu_struct_efi_device_path_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiDevicePath *fu_struct_efi_device_path_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiDevicePathType fu_struct_efi_device_path_get_type(const FuStructEfiDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_efi_device_path_get_subtype(const FuStructEfiDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_device_path_get_length(const FuStructEfiDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_device_path_set_type(FuStructEfiDevicePath *st, FuEfiDevicePathType value) G_GNUC_NON_NULL(1);
void fu_struct_efi_device_path_set_subtype(FuStructEfiDevicePath *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_device_path_set_length(FuStructEfiDevicePath *st, guint16 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_DEVICE_PATH_OFFSET_TYPE 0x0
#define FU_STRUCT_EFI_DEVICE_PATH_OFFSET_SUBTYPE 0x1
#define FU_STRUCT_EFI_DEVICE_PATH_OFFSET_LENGTH 0x2
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_DEVICE_PATH_SIZE 0x4
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_DEVICE_PATH_DEFAULT_SUBTYPE 0xFF
#define FU_STRUCT_EFI_DEVICE_PATH_DEFAULT_LENGTH 4
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiHardDriveDevicePath;

FuStructEfiHardDriveDevicePath *fu_struct_efi_hard_drive_device_path_ref(FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_hard_drive_device_path_unref(FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiHardDriveDevicePath, fu_struct_efi_hard_drive_device_path_unref)
FuStructEfiHardDriveDevicePath *fu_struct_efi_hard_drive_device_path_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiHardDriveDevicePath *fu_struct_efi_hard_drive_device_path_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiHardDriveDevicePathSubtype fu_struct_efi_hard_drive_device_path_get_subtype(const FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_hard_drive_device_path_get_partition_number(const FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint64 fu_struct_efi_hard_drive_device_path_get_partition_start(const FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint64 fu_struct_efi_hard_drive_device_path_get_partition_size(const FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_hard_drive_device_path_get_partition_signature(const FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiHardDriveDevicePathPartitionFormat fu_struct_efi_hard_drive_device_path_get_partition_format(const FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiHardDriveDevicePathSignatureType fu_struct_efi_hard_drive_device_path_get_signature_type(const FuStructEfiHardDriveDevicePath *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_hard_drive_device_path_set_subtype(FuStructEfiHardDriveDevicePath *st, FuEfiHardDriveDevicePathSubtype value) G_GNUC_NON_NULL(1);
void fu_struct_efi_hard_drive_device_path_set_partition_number(FuStructEfiHardDriveDevicePath *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_hard_drive_device_path_set_partition_start(FuStructEfiHardDriveDevicePath *st, guint64 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_hard_drive_device_path_set_partition_size(FuStructEfiHardDriveDevicePath *st, guint64 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_hard_drive_device_path_set_partition_signature(FuStructEfiHardDriveDevicePath *st, const fwupd_guid_t *value) G_GNUC_NON_NULL(1, 2);
void fu_struct_efi_hard_drive_device_path_set_partition_format(FuStructEfiHardDriveDevicePath *st, FuEfiHardDriveDevicePathPartitionFormat value) G_GNUC_NON_NULL(1);
void fu_struct_efi_hard_drive_device_path_set_signature_type(FuStructEfiHardDriveDevicePath *st, FuEfiHardDriveDevicePathSignatureType value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_TYPE 0x0
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_SUBTYPE 0x1
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_LENGTH 0x2
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_PARTITION_NUMBER 0x4
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_PARTITION_START 0x8
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_PARTITION_SIZE 0x10
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_PARTITION_SIGNATURE 0x18
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_PARTITION_FORMAT 0x28
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_OFFSET_SIGNATURE_TYPE 0x29
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_SIZE 0x2A
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_DEFAULT_TYPE FU_EFI_DEVICE_PATH_TYPE_MEDIA
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_DEFAULT_SUBTYPE FU_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE_HARD_DRIVE
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_DEFAULT_LENGTH 42
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_DEFAULT_PARTITION_FORMAT FU_EFI_HARD_DRIVE_DEVICE_PATH_PARTITION_FORMAT_GUID_PARTITION_TABLE
#define FU_STRUCT_EFI_HARD_DRIVE_DEVICE_PATH_DEFAULT_SIGNATURE_TYPE FU_EFI_HARD_DRIVE_DEVICE_PATH_SIGNATURE_TYPE_GUID
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiVss2VariableStoreHeader;

FuStructEfiVss2VariableStoreHeader *fu_struct_efi_vss2_variable_store_header_ref(FuStructEfiVss2VariableStoreHeader *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_vss2_variable_store_header_unref(FuStructEfiVss2VariableStoreHeader *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiVss2VariableStoreHeader, fu_struct_efi_vss2_variable_store_header_unref)
FuStructEfiVss2VariableStoreHeader *fu_struct_efi_vss2_variable_store_header_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiVss2VariableStoreHeader *fu_struct_efi_vss2_variable_store_header_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gboolean fu_struct_efi_vss2_variable_store_header_validate_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_vss2_variable_store_header_get_size(const FuStructEfiVss2VariableStoreHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_vss2_variable_store_header_set_size(FuStructEfiVss2VariableStoreHeader *st, guint32 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VSS2_VARIABLE_STORE_HEADER_OFFSET_SIGNATURE 0x0
#define FU_STRUCT_EFI_VSS2_VARIABLE_STORE_HEADER_OFFSET_SIZE 0x10
#define FU_STRUCT_EFI_VSS2_VARIABLE_STORE_HEADER_OFFSET_FORMAT 0x14
#define FU_STRUCT_EFI_VSS2_VARIABLE_STORE_HEADER_OFFSET_STATE 0x15
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VSS2_VARIABLE_STORE_HEADER_SIZE 0x1C
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VSS2_VARIABLE_STORE_HEADER_DEFAULT_SIGNATURE "\x78\x2c\xf3\xaa\x7b\x94\x9a\x43\xa1\x80\x2e\x14\x4e\xc3\x77\x92"
#define FU_STRUCT_EFI_VSS2_VARIABLE_STORE_HEADER_DEFAULT_FORMAT FU_EFI_VARIABLE_STORE_FORMAT_FORMATTED
#define FU_STRUCT_EFI_VSS2_VARIABLE_STORE_HEADER_DEFAULT_STATE FU_EFI_VARIABLE_STORE_STATE_HEALTHY
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiVssAuthVariableHeader;

FuStructEfiVssAuthVariableHeader *fu_struct_efi_vss_auth_variable_header_ref(FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_vss_auth_variable_header_unref(FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiVssAuthVariableHeader, fu_struct_efi_vss_auth_variable_header_unref)
FuStructEfiVssAuthVariableHeader *fu_struct_efi_vss_auth_variable_header_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiVssAuthVariableHeader *fu_struct_efi_vss_auth_variable_header_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint16 fu_struct_efi_vss_auth_variable_header_get_start_id(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiVariableState fu_struct_efi_vss_auth_variable_header_get_state(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiVariableAttrs fu_struct_efi_vss_auth_variable_header_get_attributes(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint64 fu_struct_efi_vss_auth_variable_header_get_monotonic_counter(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiTime *fu_struct_efi_vss_auth_variable_header_get_timestamp(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_vss_auth_variable_header_get_pubkey_index(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_vss_auth_variable_header_get_name_size(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_vss_auth_variable_header_get_data_size(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
const fwupd_guid_t *fu_struct_efi_vss_auth_variable_header_get_vendor_guid(const FuStructEfiVssAuthVariableHeader *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_vss_auth_variable_header_set_start_id(FuStructEfiVssAuthVariableHeader *st, guint16 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_vss_auth_variable_header_set_state(FuStructEfiVssAuthVariableHeader *st, FuEfiVariableState value) G_GNUC_NON_NULL(1);
void fu_struct_efi_vss_auth_variable_header_set_attributes(FuStructEfiVssAuthVariableHeader *st, FuEfiVariableAttrs value) G_GNUC_NON_NULL(1);
void fu_struct_efi_vss_auth_variable_header_set_monotonic_counter(FuStructEfiVssAuthVariableHeader *st, guint64 value) G_GNUC_NON_NULL(1);
gboolean
fu_struct_efi_vss_auth_variable_header_set_timestamp(FuStructEfiVssAuthVariableHeader *st, const FuStructEfiTime *st_donor, GError **error) G_GNUC_NON_NULL(1, 2) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_vss_auth_variable_header_set_pubkey_index(FuStructEfiVssAuthVariableHeader *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_vss_auth_variable_header_set_name_size(FuStructEfiVssAuthVariableHeader *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_vss_auth_variable_header_set_data_size(FuStructEfiVssAuthVariableHeader *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_vss_auth_variable_header_set_vendor_guid(FuStructEfiVssAuthVariableHeader *st, const fwupd_guid_t *value) G_GNUC_NON_NULL(1, 2);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_START_ID 0x0
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_STATE 0x2
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_ATTRIBUTES 0x4
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_MONOTONIC_COUNTER 0x8
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_TIMESTAMP 0x10
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_PUBKEY_INDEX 0x20
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_NAME_SIZE 0x24
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_DATA_SIZE 0x28
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_OFFSET_VENDOR_GUID 0x2C
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_SIZE 0x3C
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_VSS_AUTH_VARIABLE_HEADER_DEFAULT_START_ID 0x55AA
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructEfiFaultTolerantWorkingBlockHeader64;

FuStructEfiFaultTolerantWorkingBlockHeader64 *fu_struct_efi_fault_tolerant_working_block_header64_ref(FuStructEfiFaultTolerantWorkingBlockHeader64 *st) G_GNUC_NON_NULL(1);
void fu_struct_efi_fault_tolerant_working_block_header64_unref(FuStructEfiFaultTolerantWorkingBlockHeader64 *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructEfiFaultTolerantWorkingBlockHeader64, fu_struct_efi_fault_tolerant_working_block_header64_unref)
FuStructEfiFaultTolerantWorkingBlockHeader64 *fu_struct_efi_fault_tolerant_working_block_header64_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructEfiFaultTolerantWorkingBlockHeader64 *fu_struct_efi_fault_tolerant_working_block_header64_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
gboolean fu_struct_efi_fault_tolerant_working_block_header64_validate_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_efi_fault_tolerant_working_block_header64_get_crc(const FuStructEfiFaultTolerantWorkingBlockHeader64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
FuEfiVariableStoreState fu_struct_efi_fault_tolerant_working_block_header64_get_state(const FuStructEfiFaultTolerantWorkingBlockHeader64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint64 fu_struct_efi_fault_tolerant_working_block_header64_get_write_queue_size(const FuStructEfiFaultTolerantWorkingBlockHeader64 *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_efi_fault_tolerant_working_block_header64_set_crc(FuStructEfiFaultTolerantWorkingBlockHeader64 *st, guint32 value) G_GNUC_NON_NULL(1);
void fu_struct_efi_fault_tolerant_working_block_header64_set_state(FuStructEfiFaultTolerantWorkingBlockHeader64 *st, FuEfiVariableStoreState value) G_GNUC_NON_NULL(1);
void fu_struct_efi_fault_tolerant_working_block_header64_set_write_queue_size(FuStructEfiFaultTolerantWorkingBlockHeader64 *st, guint64 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER64_OFFSET_SIGNATURE 0x0
#define FU_STRUCT_EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER64_OFFSET_CRC 0x10
#define FU_STRUCT_EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER64_OFFSET_STATE 0x14
#define FU_STRUCT_EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER64_OFFSET_WRITE_QUEUE_SIZE 0x18
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER64_SIZE 0x20
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER64_DEFAULT_SIGNATURE "\x2b\x29\x58\x9e\x68\x7c\x7d\x49\xa0\xce\x65\x0\xfd\x9f\x1b\x95"
#define FU_STRUCT_EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER64_DEFAULT_CRC 0xFFFFFFFF
#define FU_STRUCT_EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER64_DEFAULT_STATE FU_EFI_VARIABLE_STORE_STATE_EMPTY
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructShimHive;

FuStructShimHive *fu_struct_shim_hive_ref(FuStructShimHive *st) G_GNUC_NON_NULL(1);
void fu_struct_shim_hive_unref(FuStructShimHive *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructShimHive, fu_struct_shim_hive_unref)
FuStructShimHive *fu_struct_shim_hive_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructShimHive *fu_struct_shim_hive_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_shim_hive_get_header_version(const FuStructShimHive *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_shim_hive_get_items_count(const FuStructShimHive *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_shim_hive_get_items_offset(const FuStructShimHive *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_shim_hive_get_crc32(const FuStructShimHive *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_shim_hive_set_header_version(FuStructShimHive *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_shim_hive_set_items_count(FuStructShimHive *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_shim_hive_set_items_offset(FuStructShimHive *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_shim_hive_set_crc32(FuStructShimHive *st, guint32 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_SHIM_HIVE_OFFSET_MAGIC 0x0
#define FU_STRUCT_SHIM_HIVE_OFFSET_HEADER_VERSION 0x4
#define FU_STRUCT_SHIM_HIVE_OFFSET_ITEMS_COUNT 0x5
#define FU_STRUCT_SHIM_HIVE_OFFSET_ITEMS_OFFSET 0x6
#define FU_STRUCT_SHIM_HIVE_OFFSET_CRC32 0x7
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_SHIM_HIVE_SIZE_MAGIC 0x4
#define FU_STRUCT_SHIM_HIVE_N_ELEMENTS_MAGIC 4
#define FU_STRUCT_SHIM_HIVE_SIZE 0xB
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_SHIM_HIVE_DEFAULT_MAGIC "HIVE"
#define FU_STRUCT_SHIM_HIVE_DEFAULT_HEADER_VERSION 0x1
#endif
typedef struct {
  GByteArray *buf;
  guint refcount;
} FuStructShimHiveItem;

FuStructShimHiveItem *fu_struct_shim_hive_item_ref(FuStructShimHiveItem *st) G_GNUC_NON_NULL(1);
void fu_struct_shim_hive_item_unref(FuStructShimHiveItem *st) G_GNUC_NON_NULL(1);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuStructShimHiveItem, fu_struct_shim_hive_item_unref)
FuStructShimHiveItem *fu_struct_shim_hive_item_new(void) G_GNUC_WARN_UNUSED_RESULT;
FuStructShimHiveItem *fu_struct_shim_hive_item_parse_stream(FuInputStream *stream, gsize offset, GError **error) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint8 fu_struct_shim_hive_item_get_key_length(const FuStructShimHiveItem *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
guint32 fu_struct_shim_hive_item_get_value_length(const FuStructShimHiveItem *st) G_GNUC_NON_NULL(1) G_GNUC_WARN_UNUSED_RESULT;
void fu_struct_shim_hive_item_set_key_length(FuStructShimHiveItem *st, guint8 value) G_GNUC_NON_NULL(1);
void fu_struct_shim_hive_item_set_value_length(FuStructShimHiveItem *st, guint32 value) G_GNUC_NON_NULL(1);

#ifndef __GI_SCANNER__
#define FU_STRUCT_SHIM_HIVE_ITEM_OFFSET_KEY_LENGTH 0x0
#define FU_STRUCT_SHIM_HIVE_ITEM_OFFSET_VALUE_LENGTH 0x1
#endif

#ifndef __GI_SCANNER__
#define FU_STRUCT_SHIM_HIVE_ITEM_SIZE 0x5
#endif

#ifndef __GI_SCANNER__
#endif
