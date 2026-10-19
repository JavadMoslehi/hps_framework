#include "../test_suites.h"
#include "hps/platform/hosted/debug.h"
#include <assert.h>

/**
 * @file tests/platform/test_hosted_debug.c
 * @brief Successful assertions and nonfatal zero-result adapter behavior.
 */

#ifdef NDEBUG
# error "Hosted debug tests require enabled assertions"
#endif

int32_t test_hosted_debug(void)
{
    uint32_t evaluations = UINT32_C(0);
    HPS_DEBUG_ASSERT(++evaluations == UINT32_C(1));
    assert(evaluations == UINT32_C(1));
    HPS_ASSERT_OR_PANIC(++evaluations == UINT32_C(2));
    assert(evaluations == UINT32_C(2));

    /* Zero must not inspect strings even when their lengths are unusable. */
    hps_result_report(INT32_C(0), NULL, SIZE_MAX, NULL, SIZE_MAX);
    hps_panic_on_result(INT32_C(0), NULL, SIZE_MAX, NULL, SIZE_MAX);
    return INT32_C(0);
}
