#ifndef HPS_BASE_VERSION_H
#define HPS_BASE_VERSION_H

/**
 * @file hps/base/version.h
 * @brief Framework version constants and compile-time comparison.
 */

/* Initial development API; no stable ABI is promised before 1.0. */
#define HPS_VERSION_MAJOR 0
#define HPS_VERSION_MINOR 1
#define HPS_VERSION_PATCH 0
#define HPS_VERSION_STRING "0.1.0"
#define HPS_VERSION_AT_LEAST(major, minor, patch) \
    ((HPS_VERSION_MAJOR > (major)) || \
     (HPS_VERSION_MAJOR == (major) && HPS_VERSION_MINOR > (minor)) || \
     (HPS_VERSION_MAJOR == (major) && HPS_VERSION_MINOR == (minor) && \
      HPS_VERSION_PATCH >= (patch)))

#endif /* HPS_BASE_VERSION_H */
