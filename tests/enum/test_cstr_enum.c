#include "../test_suites.h"
#include "hps/enum/cstr.h"
#include "hps/enum/cstr.h"
#include <assert.h>
#include "hps/base/hps_stdint.h"
#include <string.h>

/**
 * @file tests/enum/test_cstr_enum.c
 * @brief String switch generation, bounded parsing and reverse name lookup.
 */

#define TEST_KEYS(ENTRY_, PREFIX_) \
    ENTRY_(PREFIX_, PORT, "port") \
    ENTRY_(PREFIX_, MODE, "mode") \
    ENTRY_(PREFIX_, EMPTY, "") \
    ENTRY_(PREFIX_, DUPLICATE, "port")

HPS_CSTR_ENUM_DEFINE_STATIC(test_key_t, TEST_KEY, TEST_KEYS)

int32_t test_cstr_enum(void)
{
    const char missing_nul[4] = {'p','o','r','t'};
    assert(test_key_t_from_cstr("port", 5U) == TEST_KEY_PORT);
    assert(test_key_t_from_cstr("mode", 5U) == TEST_KEY_MODE);
    assert(test_key_t_from_cstr("", 1U) == TEST_KEY_EMPTY);
    assert(test_key_t_from_cstr("PORT", 5U) == TEST_KEY_UNKNOWN);
    assert(test_key_t_from_cstr("port!", 6U) == TEST_KEY_UNKNOWN);
    assert(test_key_t_from_cstr(NULL, 5U) == TEST_KEY_UNKNOWN);
    assert(test_key_t_from_cstr("port", 0U) == TEST_KEY_UNKNOWN);
    assert(test_key_t_from_cstr(missing_nul, sizeof(missing_nul)) == TEST_KEY_UNKNOWN);
    assert(strcmp(test_key_t_to_cstr(TEST_KEY_MODE), "mode") == 0);
    assert(strcmp(test_key_t_to_cstr(TEST_KEY_DUPLICATE), "port") == 0);
    assert(strcmp(test_key_t_to_cstr(TEST_KEY_UNKNOWN), "unknown") == 0);
    assert(strcmp(test_key_t_to_cstr(TEST_KEY_COUNT), "unknown") == 0);
    assert(strcmp(test_key_t_to_cstr((test_key_t)INT32_C(99)), "unknown") == 0);
    switch (test_key_t_from_cstr("port", 5U)) {
        case TEST_KEY_PORT:
            break;
        default:
            assert(0);
            break;
    }
    return INT32_C(0);
}
