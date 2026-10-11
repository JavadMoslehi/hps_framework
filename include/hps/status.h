#ifndef HPS_STATUS_H
#define HPS_STATUS_H

/**
 * @file hps/status.h
 * @brief Framework status type, module namespaces and immutable diagnostics.
 */

#include <stddef.h>
#include <stdint.h>

typedef int64_t hps_status_t;

#define HPS_STATUS_OK ((hps_status_t)INT64_C(0))
#define HPS_STATUS_INVALID_ARGUMENT ((hps_status_t)-INT64_C(1))
#define HPS_STATUS_OUT_OF_RANGE ((hps_status_t)-INT64_C(2))
#define HPS_STATUS_NO_SPACE ((hps_status_t)-INT64_C(3))
#define HPS_STATUS_INVALID_STATE ((hps_status_t)-INT64_C(4))

#define HPS_STATUS_MODULE_FRAMEWORK ((uint16_t)0U)
#define HPS_STATUS_MODULE_MEMORY ((uint16_t)1U)
#define HPS_STATUS_MODULE_BYTES ((uint16_t)2U)
#define HPS_STATUS_MODULE_UNKNOWN ((uint16_t)UINT16_MAX)
#define HPS_STATUS_MESSAGE_CAPACITY ((size_t)32U)

#ifdef __cplusplus
extern "C" {
#endif

/* Zero is the only success; registered errors are negative int64_t values.
 * Module m owns -(m*65536 + reason), reason 1..65535; reason zero is reserved.
 * IDs 0/1/2 are registered to framework/memory/bytes. UINT16_MAX is reserved for
 * UNKNOWN; future module IDs are 3..65534. New IDs/reasons require explicit static
 * registration; published values must not be reused.
 * Module status aliases have this exact type. Codes are diagnostics, not a wire
 * representation or authorization. No allocation, OS calls or mutable registry.
 */

/* Classifies an owned range even when the individual reason is unregistered.
 * OK maps to FRAMEWORK. Reserved zero-reason slots, positive values and values
 * outside the three registered ranges map to UNKNOWN. Every int64_t is valid
 * input, including INT64_MIN; no negation or absolute-value conversion. O(1).
 */
uint16_t hps_status_module(hps_status_t status);

/* Returns a non-NULL immutable NUL-terminated diagnostic C string with program
 * lifetime, occupying at most MESSAGE_CAPACITY bytes including its NUL. Codes have
 * module-qualified descriptions; OK is "ok", others are "unknown status".
 * Borrow the string: never write/free it. Text uses the execution character set,
 * not a stable wire schema, localized message or secret-bearing log. This lookup
 * does not change pool status.
 * Every int64_t is valid input. Bounded lookup over sixteen registered codes;
 * no heap, caller buffer, initialization, callbacks or mutable registry state.
 */
const char *hps_status_string(hps_status_t status);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_STATUS_H */
