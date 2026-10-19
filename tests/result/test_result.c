#include "../test_suites.h"
#include "hps/result/check.h"
#include <assert.h>

/**
 * @file tests/result/test_result.c
 * @brief Zero-result conventions, range extremes and single evaluation.
 */

#ifdef NDEBUG
# error "Result tests require enabled assertions"
#endif

int32_t test_result(void)
{
    assert(hps_result_is_success(INT32_C(0)) == UINT8_C(1));
    assert(hps_result_is_failure(INT32_C(0)) == UINT8_C(0));
    assert(hps_result_is_success(-INT32_C(1)) == UINT8_C(0));
    assert(hps_result_is_failure(-INT32_C(1)) == UINT8_C(1));
    assert(hps_result_is_success(INT32_C(1)) == UINT8_C(0));
    assert(hps_result_is_failure(INT32_C(1)) == UINT8_C(1));
    assert(hps_result_is_success(INT32_MIN) == UINT8_C(0));
    assert(hps_result_is_failure(INT32_MIN) == UINT8_C(1));
    assert(hps_result_is_success(INT32_MAX) == UINT8_C(0));
    assert(hps_result_is_failure(INT32_MAX) == UINT8_C(1));

    int32_t value = -INT32_C(1);
    uint8_t success = hps_result_is_success(++value);
    assert(success == UINT8_C(1));
    assert(value == INT32_C(0));
    uint8_t failure = hps_result_is_failure(++value);
    assert(failure == UINT8_C(1));
    assert(value == INT32_C(1));
    return INT32_C(0);
}
