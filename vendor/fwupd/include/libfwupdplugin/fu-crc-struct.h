/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fwupd.h>
/**
 * FuCrcKind:
 */
typedef enum {
    /**
     * FU_CRC_KIND_UNKNOWN:
     */
    FU_CRC_KIND_UNKNOWN,
    /**
     * FU_CRC_KIND_B32_STANDARD:
     */
    FU_CRC_KIND_B32_STANDARD,
    /**
     * FU_CRC_KIND_B32_BZIP2:
     */
    FU_CRC_KIND_B32_BZIP2,
    /**
     * FU_CRC_KIND_B32_JAMCRC:
     */
    FU_CRC_KIND_B32_JAMCRC,
    /**
     * FU_CRC_KIND_B32_MPEG2:
     */
    FU_CRC_KIND_B32_MPEG2,
    /**
     * FU_CRC_KIND_B32_POSIX:
     */
    FU_CRC_KIND_B32_POSIX,
    /**
     * FU_CRC_KIND_B32_SATA:
     */
    FU_CRC_KIND_B32_SATA,
    /**
     * FU_CRC_KIND_B32_XFER:
     */
    FU_CRC_KIND_B32_XFER,
    /**
     * FU_CRC_KIND_B32C:
     */
    FU_CRC_KIND_B32C,
    /**
     * FU_CRC_KIND_B32D:
     */
    FU_CRC_KIND_B32D,
    /**
     * FU_CRC_KIND_B32Q:
     */
    FU_CRC_KIND_B32Q,
    /**
     * FU_CRC_KIND_B16_XMODEM:
     */
    FU_CRC_KIND_B16_XMODEM,
    /**
     * FU_CRC_KIND_B16_KERMIT:
     */
    FU_CRC_KIND_B16_KERMIT,
    /**
     * FU_CRC_KIND_B16_USB:
     */
    FU_CRC_KIND_B16_USB,
    /**
     * FU_CRC_KIND_B16_UMTS:
     */
    FU_CRC_KIND_B16_UMTS,
    /**
     * FU_CRC_KIND_B16_TMS37157:
     */
    FU_CRC_KIND_B16_TMS37157,
    /**
     * FU_CRC_KIND_B16_BNR:
     */
    FU_CRC_KIND_B16_BNR,
    /**
     * FU_CRC_KIND_B8_WCDMA:
     */
    FU_CRC_KIND_B8_WCDMA,
    /**
     * FU_CRC_KIND_B8_TECH3250:
     */
    FU_CRC_KIND_B8_TECH3250,
    /**
     * FU_CRC_KIND_B8_STANDARD:
     */
    FU_CRC_KIND_B8_STANDARD,
    /**
     * FU_CRC_KIND_B8_SAE_J1850:
     */
    FU_CRC_KIND_B8_SAE_J1850,
    /**
     * FU_CRC_KIND_B8_ROHC:
     */
    FU_CRC_KIND_B8_ROHC,
    /**
     * FU_CRC_KIND_B8_OPENSAFETY:
     */
    FU_CRC_KIND_B8_OPENSAFETY,
    /**
     * FU_CRC_KIND_B8_NRSC5:
     */
    FU_CRC_KIND_B8_NRSC5,
    /**
     * FU_CRC_KIND_B8_MIFARE_MAD:
     */
    FU_CRC_KIND_B8_MIFARE_MAD,
    /**
     * FU_CRC_KIND_B8_MAXIM_DOW:
     */
    FU_CRC_KIND_B8_MAXIM_DOW,
    /**
     * FU_CRC_KIND_B8_LTE:
     */
    FU_CRC_KIND_B8_LTE,
    /**
     * FU_CRC_KIND_B8_I_CODE:
     */
    FU_CRC_KIND_B8_I_CODE,
    /**
     * FU_CRC_KIND_B8_ITU:
     */
    FU_CRC_KIND_B8_ITU,
    /**
     * FU_CRC_KIND_B8_HITAG:
     */
    FU_CRC_KIND_B8_HITAG,
    /**
     * FU_CRC_KIND_B8_GSM_B:
     */
    FU_CRC_KIND_B8_GSM_B,
    /**
     * FU_CRC_KIND_B8_GSM_A:
     */
    FU_CRC_KIND_B8_GSM_A,
    /**
     * FU_CRC_KIND_B8_DVB_S2:
     */
    FU_CRC_KIND_B8_DVB_S2,
    /**
     * FU_CRC_KIND_B8_DARC:
     */
    FU_CRC_KIND_B8_DARC,
    /**
     * FU_CRC_KIND_B8_CDMA2000:
     */
    FU_CRC_KIND_B8_CDMA2000,
    /**
     * FU_CRC_KIND_B8_BLUETOOTH:
     */
    FU_CRC_KIND_B8_BLUETOOTH,
    /**
     * FU_CRC_KIND_B8_AUTOSAR:
     */
    FU_CRC_KIND_B8_AUTOSAR,
}  FuCrcKind;
#define FU_CRC_KIND_LAST 37
const gchar *fu_crc_kind_to_string(FuCrcKind val) G_GNUC_CONST;
FuCrcKind fu_crc_kind_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
