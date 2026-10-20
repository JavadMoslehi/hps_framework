#ifndef HPS_RESULT_CHECK_H
#define HPS_RESULT_CHECK_H

/**
 * @file hps/result/check.h
 * @brief Zero-result predicates without error types, codes or registration.
 */

#include "hps/base/hps_stdint.h"
#include "hps/base/attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return exactly 0 or 1. These O(1) predicates inspect an int32_t once and
 * acquire no storage or ownership. The originating API defines each result's
 * meaning: HPS error-returning modules use zero success and negative failures.
 * Positive values are also non-success here; these helpers do not validate or
 * translate an API's error domain, and do not apply to count/pointer results.
 */
static HPS_INLINE uint8_t hps_result_is_success(const int32_t result)
{
    return (result == INT32_C(0)) ? UINT8_C(1) : UINT8_C(0);
}

static HPS_INLINE uint8_t hps_result_is_failure(const int32_t result)
{
    return (result != INT32_C(0)) ? UINT8_C(1) : UINT8_C(0);
}

#ifdef __cplusplus
}
#endif

#endif /* HPS_RESULT_CHECK_H */
