#include "hps/status.h"
#include "hps/bytes/view.h"
#include "hps/memory/pool.h"

/**
 * @file src/status.c
 * @brief Static module-range classification and status message registry.
 */

/* Fixed allocation of namespaces, not runtime configuration. Comparisons avoid
 * arithmetic on arbitrary caller statuses, including the signed minimum.
 * Module headers supply only constants/types; no module function is called.
 */
#define HPS_FRAMEWORK_ERROR_MIN (-INT64_C(65535))
#define HPS_FRAMEWORK_ERROR_MAX (-INT64_C(1))
#define HPS_MEMORY_ERROR_MIN (-INT64_C(131071))
#define HPS_MEMORY_ERROR_MAX (-INT64_C(65537))
#define HPS_BYTES_ERROR_MIN (-INT64_C(196607))
#define HPS_BYTES_ERROR_MAX (-INT64_C(131073))

uint16_t hps_status_module(const hps_status_t status)
{
    uint16_t module = HPS_STATUS_MODULE_UNKNOWN;
    if ((status == HPS_STATUS_OK) ||
        ((status >= HPS_FRAMEWORK_ERROR_MIN) &&
         (status <= HPS_FRAMEWORK_ERROR_MAX))) {
        module = HPS_STATUS_MODULE_FRAMEWORK;
    } else if ((status >= HPS_MEMORY_ERROR_MIN) &&
               (status <= HPS_MEMORY_ERROR_MAX)) {
        module = HPS_STATUS_MODULE_MEMORY;
    } else if ((status >= HPS_BYTES_ERROR_MIN) &&
               (status <= HPS_BYTES_ERROR_MAX)) {
        module = HPS_STATUS_MODULE_BYTES;
    } else {
        /* Unknown/unallocated values retain UNKNOWN. */
    }
    return module;
}

const char *hps_status_string(const hps_status_t status)
{
    const char *message = "unknown status";
    switch (status) {
        case HPS_STATUS_OK:
            message = "ok";
            break;
        case HPS_STATUS_INVALID_ARGUMENT:
            message = "framework: invalid argument";
            break;
        case HPS_STATUS_OUT_OF_RANGE:
            message = "framework: out of range";
            break;
        case HPS_STATUS_NO_SPACE:
            message = "framework: no space";
            break;
        case HPS_STATUS_INVALID_STATE:
            message = "framework: invalid state";
            break;
        case HPS_POOL_INVALID_ARGUMENT:
            message = "memory: invalid argument";
            break;
        case HPS_POOL_EXHAUSTED:
            message = "memory: exhausted";
            break;
        case HPS_POOL_INVALID_POINTER:
            message = "memory: invalid pointer";
            break;
        case HPS_POOL_DOUBLE_FREE:
            message = "memory: double free";
            break;
        case HPS_POOL_NOT_INITIALIZED:
            message = "memory: not initialized";
            break;
        case HPS_POOL_ALREADY_INITIALIZED:
            message = "memory: already initialized";
            break;
        case HPS_POOL_CORRUPT:
            message = "memory: corrupt";
            break;
        case HPS_BYTE_INVALID_ARGUMENT:
            message = "bytes: invalid argument";
            break;
        case HPS_BYTE_OUT_OF_RANGE:
            message = "bytes: out of range";
            break;
        case HPS_BYTE_NO_SPACE:
            message = "bytes: no space";
            break;
        case HPS_BYTE_INVALID_STATE:
            message = "bytes: invalid state";
            break;
        default:
            /* Unregistered codes retain the immutable fallback. */
            break;
    }
    return message;
}
