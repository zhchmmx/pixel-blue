/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fwupd.h>
/**
 * FuFirmwareExportFlags:
 */
typedef enum {
    /**
     * FU_FIRMWARE_EXPORT_FLAG_NONE:
     */
    FU_FIRMWARE_EXPORT_FLAG_NONE = 0,
    /**
     * FU_FIRMWARE_EXPORT_FLAG_INCLUDE_DEBUG:
     */
    FU_FIRMWARE_EXPORT_FLAG_INCLUDE_DEBUG = 1 << 0,
    /**
     * FU_FIRMWARE_EXPORT_FLAG_ASCII_DATA:
     *
     * as UTF-8 strings
     */
    FU_FIRMWARE_EXPORT_FLAG_ASCII_DATA = 1 << 1,
    /**
     * FU_FIRMWARE_EXPORT_FLAG_SORTED:
     */
    FU_FIRMWARE_EXPORT_FLAG_SORTED = 1 << 2,
} G_GNUC_FLAG_ENUM FuFirmwareExportFlags;
/**
 * FuFirmwareParseFlags:
 */
typedef enum {
    /**
     * FU_FIRMWARE_PARSE_FLAG_NONE:
     */
    FU_FIRMWARE_PARSE_FLAG_NONE = 0,
    /**
     * FU_FIRMWARE_PARSE_FLAG_IGNORE_CHECKSUM:
     */
    FU_FIRMWARE_PARSE_FLAG_IGNORE_CHECKSUM = 1 << 6,
    /**
     * FU_FIRMWARE_PARSE_FLAG_IGNORE_VID_PID:
     */
    FU_FIRMWARE_PARSE_FLAG_IGNORE_VID_PID = 1 << 7,
    /**
     * FU_FIRMWARE_PARSE_FLAG_NO_SEARCH:
     *
     * no heuristics
     */
    FU_FIRMWARE_PARSE_FLAG_NO_SEARCH = 1 << 8,
    /**
     * FU_FIRMWARE_PARSE_FLAG_CACHE_STREAM:
     */
    FU_FIRMWARE_PARSE_FLAG_CACHE_STREAM = 1 << 10,
    /**
     * FU_FIRMWARE_PARSE_FLAG_CACHE_BLOB:
     */
    FU_FIRMWARE_PARSE_FLAG_CACHE_BLOB = 1 << 11,
    /**
     * FU_FIRMWARE_PARSE_FLAG_ONLY_TRUST_PQ_SIGNATURES:
     */
    FU_FIRMWARE_PARSE_FLAG_ONLY_TRUST_PQ_SIGNATURES = 1 << 12,
    /**
     * FU_FIRMWARE_PARSE_FLAG_ONLY_PARTITION_LAYOUT:
     */
    FU_FIRMWARE_PARSE_FLAG_ONLY_PARTITION_LAYOUT = 1 << 13,
    /**
     * FU_FIRMWARE_PARSE_FLAG_ONLY_BASENAME:
     */
    FU_FIRMWARE_PARSE_FLAG_ONLY_BASENAME = 1 << 14,
} G_GNUC_FLAG_ENUM FuFirmwareParseFlags;
/**
 * FuFirmwareBuilderFlags:
 */
typedef enum {
    /**
     * FU_FIRMWARE_BUILDER_FLAG_NONE:
     */
    FU_FIRMWARE_BUILDER_FLAG_NONE = 0,
    /**
     * FU_FIRMWARE_BUILDER_FLAG_NO_BINARY_COMPARE:
     */
    FU_FIRMWARE_BUILDER_FLAG_NO_BINARY_COMPARE = 1 << 0,
    /**
     * FU_FIRMWARE_BUILDER_FLAG_NO_WRITE:
     */
    FU_FIRMWARE_BUILDER_FLAG_NO_WRITE = 1 << 1,
} G_GNUC_FLAG_ENUM FuFirmwareBuilderFlags;
/**
 * FuFirmwareFlags:
 */
typedef enum {
    /**
     * FU_FIRMWARE_FLAG_NONE:
     */
    FU_FIRMWARE_FLAG_NONE = 0,
    /**
     * FU_FIRMWARE_FLAG_DEDUPE_ID:
     */
    FU_FIRMWARE_FLAG_DEDUPE_ID = 1 << 0,
    /**
     * FU_FIRMWARE_FLAG_DEDUPE_IDX:
     */
    FU_FIRMWARE_FLAG_DEDUPE_IDX = 1 << 1,
    /**
     * FU_FIRMWARE_FLAG_HAS_CHECKSUM:
     *
     * or CRC
     */
    FU_FIRMWARE_FLAG_HAS_CHECKSUM = 1 << 2,
    /**
     * FU_FIRMWARE_FLAG_HAS_VID_PID:
     */
    FU_FIRMWARE_FLAG_HAS_VID_PID = 1 << 3,
    /**
     * FU_FIRMWARE_FLAG_DONE_PARSE:
     */
    FU_FIRMWARE_FLAG_DONE_PARSE = 1 << 4,
    /**
     * FU_FIRMWARE_FLAG_HAS_STORED_SIZE:
     */
    FU_FIRMWARE_FLAG_HAS_STORED_SIZE = 1 << 5,
    /**
     * FU_FIRMWARE_FLAG_ALWAYS_SEARCH:
     *
     * useful has an *unparsed* variable-length header
     */
    FU_FIRMWARE_FLAG_ALWAYS_SEARCH = 1 << 6,
    /**
     * FU_FIRMWARE_FLAG_NO_AUTO_DETECTION:
     *
     * has no known header
     */
    FU_FIRMWARE_FLAG_NO_AUTO_DETECTION = 1 << 7,
    /**
     * FU_FIRMWARE_FLAG_HAS_CHECK_COMPATIBLE:
     */
    FU_FIRMWARE_FLAG_HAS_CHECK_COMPATIBLE = 1 << 8,
    /**
     * FU_FIRMWARE_FLAG_IS_LAST_IMAGE:
     *
     * use for FuLinearFirmware when padding is present
     */
    FU_FIRMWARE_FLAG_IS_LAST_IMAGE = 1 << 9,
    /**
     * FU_FIRMWARE_FLAG_ALLOW_LINEAR:
     *
     * parse as an array of firmwares
     */
    FU_FIRMWARE_FLAG_ALLOW_LINEAR = 1 << 10,
    /**
     * FU_FIRMWARE_FLAG_IS_ABSTRACT:
     *
     * cannot parse a blob directly
     */
    FU_FIRMWARE_FLAG_IS_ABSTRACT = 1 << 11,
    /**
     * FU_FIRMWARE_FLAG_NO_IMAGE_TYPE_CHECK:
     */
    FU_FIRMWARE_FLAG_NO_IMAGE_TYPE_CHECK = 1 << 12,
} G_GNUC_FLAG_ENUM FuFirmwareFlags;
gchar *fu_firmware_flags_to_string(FuFirmwareFlags val) G_GNUC_CONST;
/**
 * FuFirmwareAlignment:
 */
typedef enum {
    /**
     * FU_FIRMWARE_ALIGNMENT_1:
     */
    FU_FIRMWARE_ALIGNMENT_1,
    /**
     * FU_FIRMWARE_ALIGNMENT_2:
     */
    FU_FIRMWARE_ALIGNMENT_2,
    /**
     * FU_FIRMWARE_ALIGNMENT_4:
     */
    FU_FIRMWARE_ALIGNMENT_4,
    /**
     * FU_FIRMWARE_ALIGNMENT_8:
     */
    FU_FIRMWARE_ALIGNMENT_8,
    /**
     * FU_FIRMWARE_ALIGNMENT_16:
     */
    FU_FIRMWARE_ALIGNMENT_16,
    /**
     * FU_FIRMWARE_ALIGNMENT_32:
     */
    FU_FIRMWARE_ALIGNMENT_32,
    /**
     * FU_FIRMWARE_ALIGNMENT_64:
     */
    FU_FIRMWARE_ALIGNMENT_64,
    /**
     * FU_FIRMWARE_ALIGNMENT_128:
     */
    FU_FIRMWARE_ALIGNMENT_128,
    /**
     * FU_FIRMWARE_ALIGNMENT_256:
     */
    FU_FIRMWARE_ALIGNMENT_256,
    /**
     * FU_FIRMWARE_ALIGNMENT_512:
     */
    FU_FIRMWARE_ALIGNMENT_512,
    /**
     * FU_FIRMWARE_ALIGNMENT_1K:
     */
    FU_FIRMWARE_ALIGNMENT_1K,
    /**
     * FU_FIRMWARE_ALIGNMENT_2K:
     */
    FU_FIRMWARE_ALIGNMENT_2K,
    /**
     * FU_FIRMWARE_ALIGNMENT_4K:
     */
    FU_FIRMWARE_ALIGNMENT_4K,
    /**
     * FU_FIRMWARE_ALIGNMENT_8K:
     */
    FU_FIRMWARE_ALIGNMENT_8K,
    /**
     * FU_FIRMWARE_ALIGNMENT_16K:
     */
    FU_FIRMWARE_ALIGNMENT_16K,
    /**
     * FU_FIRMWARE_ALIGNMENT_32K:
     */
    FU_FIRMWARE_ALIGNMENT_32K,
    /**
     * FU_FIRMWARE_ALIGNMENT_64K:
     */
    FU_FIRMWARE_ALIGNMENT_64K,
    /**
     * FU_FIRMWARE_ALIGNMENT_128K:
     */
    FU_FIRMWARE_ALIGNMENT_128K,
    /**
     * FU_FIRMWARE_ALIGNMENT_256K:
     */
    FU_FIRMWARE_ALIGNMENT_256K,
    /**
     * FU_FIRMWARE_ALIGNMENT_512K:
     */
    FU_FIRMWARE_ALIGNMENT_512K,
    /**
     * FU_FIRMWARE_ALIGNMENT_1M:
     */
    FU_FIRMWARE_ALIGNMENT_1M,
    /**
     * FU_FIRMWARE_ALIGNMENT_2M:
     */
    FU_FIRMWARE_ALIGNMENT_2M,
    /**
     * FU_FIRMWARE_ALIGNMENT_4M:
     */
    FU_FIRMWARE_ALIGNMENT_4M,
    /**
     * FU_FIRMWARE_ALIGNMENT_8M:
     */
    FU_FIRMWARE_ALIGNMENT_8M,
    /**
     * FU_FIRMWARE_ALIGNMENT_16M:
     */
    FU_FIRMWARE_ALIGNMENT_16M,
    /**
     * FU_FIRMWARE_ALIGNMENT_32M:
     */
    FU_FIRMWARE_ALIGNMENT_32M,
    /**
     * FU_FIRMWARE_ALIGNMENT_64M:
     */
    FU_FIRMWARE_ALIGNMENT_64M,
    /**
     * FU_FIRMWARE_ALIGNMENT_128M:
     */
    FU_FIRMWARE_ALIGNMENT_128M,
    /**
     * FU_FIRMWARE_ALIGNMENT_256M:
     */
    FU_FIRMWARE_ALIGNMENT_256M,
    /**
     * FU_FIRMWARE_ALIGNMENT_512M:
     */
    FU_FIRMWARE_ALIGNMENT_512M,
    /**
     * FU_FIRMWARE_ALIGNMENT_1G:
     */
    FU_FIRMWARE_ALIGNMENT_1G,
    /**
     * FU_FIRMWARE_ALIGNMENT_2G:
     */
    FU_FIRMWARE_ALIGNMENT_2G,
    /**
     * FU_FIRMWARE_ALIGNMENT_4G:
     */
    FU_FIRMWARE_ALIGNMENT_4G,
}  FuFirmwareAlignment;
#define FU_FIRMWARE_ALIGNMENT_LAST 33
