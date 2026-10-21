#ifndef HPS_PLATFORM_PLATFORM_H
#define HPS_PLATFORM_PLATFORM_H

/**
 * @file hps/platform/platform.h
 * @brief Optional compile-time OS, C-library and POSIX detection umbrella.
 */

/* Library/SDK evidence must be visible before this first inclusion if needed. */
#include "hps/platform/hps_os.h"
#include "hps/platform/hps_libc.h"
#include "hps/platform/hps_posix.h"

#endif /* HPS_PLATFORM_PLATFORM_H */
