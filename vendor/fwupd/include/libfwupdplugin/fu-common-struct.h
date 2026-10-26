/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fwupd.h>
/**
 * FuCpuVendor:
 *
 * The CPU vendor.
 */
typedef enum {
    /**
     * FU_CPU_VENDOR_UNKNOWN:
     */
    FU_CPU_VENDOR_UNKNOWN,
    /**
     * FU_CPU_VENDOR_INTEL:
     */
    FU_CPU_VENDOR_INTEL,
    /**
     * FU_CPU_VENDOR_AMD:
     */
    FU_CPU_VENDOR_AMD,
}  FuCpuVendor;
#define FU_CPU_VENDOR_LAST 3
/**
 * FuPowerState:
 *
 * The system power state.
 * This does not have to be exactly what the battery is doing, but is supposed to represent the
 * 40,000ft view of the system power state.
 * For example, it is perfectly correct to set %FU_POWER_STATE_AC if the system is connected to
 * AC power, but the battery cells are discharging for health or for other performance reasons.
 */
typedef enum {
    /**
     * FU_POWER_STATE_UNKNOWN:
     */
    FU_POWER_STATE_UNKNOWN,
    /**
     * FU_POWER_STATE_AC:
     *
     * On AC power
     */
    FU_POWER_STATE_AC,
    /**
     * FU_POWER_STATE_BATTERY:
     *
     * On system battery
     */
    FU_POWER_STATE_BATTERY,
}  FuPowerState;
#define FU_POWER_STATE_LAST 3
const gchar *fu_power_state_to_string(FuPowerState val) G_GNUC_CONST;
/**
 * FuLidState:
 *
 * The device lid state.
 */
typedef enum {
    /**
     * FU_LID_STATE_UNKNOWN:
     */
    FU_LID_STATE_UNKNOWN,
    /**
     * FU_LID_STATE_OPEN:
     */
    FU_LID_STATE_OPEN,
    /**
     * FU_LID_STATE_CLOSED:
     */
    FU_LID_STATE_CLOSED,
}  FuLidState;
#define FU_LID_STATE_LAST 3
const gchar *fu_lid_state_to_string(FuLidState val) G_GNUC_CONST;
/**
 * FuDisplayState:
 *
 * The device display state.
 */
typedef enum {
    /**
     * FU_DISPLAY_STATE_UNKNOWN:
     */
    FU_DISPLAY_STATE_UNKNOWN,
    /**
     * FU_DISPLAY_STATE_CONNECTED:
     */
    FU_DISPLAY_STATE_CONNECTED,
    /**
     * FU_DISPLAY_STATE_DISCONNECTED:
     */
    FU_DISPLAY_STATE_DISCONNECTED,
}  FuDisplayState;
#define FU_DISPLAY_STATE_LAST 3
const gchar *fu_display_state_to_string(FuDisplayState val) G_GNUC_CONST;
FuDisplayState fu_display_state_from_string(const gchar *val) G_GNUC_CONST G_GNUC_NON_NULL(1);
