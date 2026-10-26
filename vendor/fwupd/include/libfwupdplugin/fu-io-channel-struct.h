/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fwupd.h>
/**
 * FuIoChannelOpenFlags:
 */
typedef enum {
    /**
     * FU_IO_CHANNEL_OPEN_FLAG_NONE:
     */
    FU_IO_CHANNEL_OPEN_FLAG_NONE = 0,
    /**
     * FU_IO_CHANNEL_OPEN_FLAG_READ:
     */
    FU_IO_CHANNEL_OPEN_FLAG_READ = 1 << 0,
    /**
     * FU_IO_CHANNEL_OPEN_FLAG_WRITE:
     */
    FU_IO_CHANNEL_OPEN_FLAG_WRITE = 1 << 1,
    /**
     * FU_IO_CHANNEL_OPEN_FLAG_NONBLOCK:
     */
    FU_IO_CHANNEL_OPEN_FLAG_NONBLOCK = 1 << 2,
    /**
     * FU_IO_CHANNEL_OPEN_FLAG_SYNC:
     */
    FU_IO_CHANNEL_OPEN_FLAG_SYNC = 1 << 3,
} G_GNUC_FLAG_ENUM FuIoChannelOpenFlags;
gchar *fu_io_channel_open_flags_to_string(FuIoChannelOpenFlags val) G_GNUC_CONST;
/**
 * FuIoChannelFlags:
 *
 * Flags used when reading data from the TTY.
 */
typedef enum {
    /**
     * FU_IO_CHANNEL_FLAG_NONE:
     *
     * No flags are set.
     */
    FU_IO_CHANNEL_FLAG_NONE = 0,
    /**
     * FU_IO_CHANNEL_FLAG_SINGLE_SHOT:
     *
     * Only one read or write is expected.
     */
    FU_IO_CHANNEL_FLAG_SINGLE_SHOT = 1 << 0,
    /**
     * FU_IO_CHANNEL_FLAG_FLUSH_INPUT:
     *
     * Flush pending input before writing.
     */
    FU_IO_CHANNEL_FLAG_FLUSH_INPUT = 1 << 1,
    /**
     * FU_IO_CHANNEL_FLAG_USE_BLOCKING_IO:
     *
     * Block waiting for the TTY.
     */
    FU_IO_CHANNEL_FLAG_USE_BLOCKING_IO = 1 << 2,
} G_GNUC_FLAG_ENUM FuIoChannelFlags;
