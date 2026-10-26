/*
 * This file is auto-generated, do not modify!
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once
#include <fwupd.h>
/**
 * FuProgressFlags:
 */
typedef enum {
    /**
     * FU_PROGRESS_FLAG_NONE:
     */
    FU_PROGRESS_FLAG_NONE = 0,
    /**
     * FU_PROGRESS_FLAG_GUESSED:
     */
    FU_PROGRESS_FLAG_GUESSED = 1 << 0,
    /**
     * FU_PROGRESS_FLAG_NO_PROFILE:
     */
    FU_PROGRESS_FLAG_NO_PROFILE = 1 << 1,
    /**
     * FU_PROGRESS_FLAG_CHILD_FINISHED:
     */
    FU_PROGRESS_FLAG_CHILD_FINISHED = 1 << 2,
    /**
     * FU_PROGRESS_FLAG_NO_TRACEBACK:
     */
    FU_PROGRESS_FLAG_NO_TRACEBACK = 1 << 3,
    /**
     * FU_PROGRESS_FLAG_NO_SENDER:
     */
    FU_PROGRESS_FLAG_NO_SENDER = 1 << 4,
    /**
     * FU_PROGRESS_FLAG_UNKNOWN:
     */
    FU_PROGRESS_FLAG_UNKNOWN = G_MAXUINT64,
} G_GNUC_FLAG_ENUM FuProgressFlags;
