#include "../test_suites.h"
#include "hps/cstr/hps_convert.h"
#include "hps/string/hps_fixed.h"

#include <assert.h>
#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include <string.h>

/**
 * @file tests/cstr/test_cstr_bool.c
 * @brief Legacy truthy spellings, bounded input and failure preservation.
 */

HPS_STRING_FIXED_DEFINE(hps_bool_fixture, 16U)

int32_t test_cstr_bool(void)
{
    const char *const truthy[] = {
        "true", "TrUe", "TRUE", "yes", "YeS", "ON", "on", "1",
        " true", "\tyes", "\non", "\r1", "\ftrue", "\vTRUE", " \t\r\n\f\vyes",
        "          true"
    };
    const char *const falsey[] = {
        "", " ", "false", "FALSE", "no", "off", "0", "2", "unknown",
        "true ", "yes\t", "on\n", "1\r", "truex", "t", "y", "01", "-1",
        "\xC2\xA0true", "\xFFtrue"
    };
    uint8_t result = 91U;
    for (size_t index = 0U; index < sizeof(truthy) / sizeof(truthy[0]); ++index) {
        assert(hps_cstr_parse_bool(truthy[index], strlen(truthy[index]) + 1U,
            &result) == HPS_STRING_OK);
        assert(result == 1U);
    }
    for (size_t index = 0U; index < sizeof(falsey) / sizeof(falsey[0]); ++index) {
        assert(hps_cstr_parse_bool(falsey[index], strlen(falsey[index]) + 1U,
            &result) == HPS_STRING_OK);
        assert(result == 0U);
    }
    result = 91U;
    assert(hps_cstr_parse_bool(NULL, 1U, &result) == HPS_STRING_INVALID_ARGUMENT);
    assert(result == 91U);
    assert(hps_cstr_parse_bool("true", 0U, &result) == HPS_STRING_INVALID_ARGUMENT);
    assert(result == 91U);
    assert(hps_cstr_parse_bool("true", 4U, &result) == HPS_STRING_NOT_TERMINATED);
    assert(result == 91U);
    assert(hps_cstr_parse_bool(NULL, 0U, NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_cstr_parse_bool("true", 5U, NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_cstr_parse_bool("true\0false", 11U, &result) == HPS_STRING_OK);
    assert(result == 1U);

    hps_bool_fixture_t fixture = HPS_STRING_FIXED_STATIC_INIT;
    hps_string_buffer buffer;
    assert(hps_bool_fixture_capacity() == sizeof(fixture.data));
    assert(hps_bool_fixture_adopt(&fixture, &buffer) == HPS_STRING_OK);
    assert(hps_bool_fixture_bind(&fixture, &buffer) == HPS_STRING_OK);
    (void)memset(fixture.data, 'x', sizeof(fixture.data));
    result = 91U;
    assert(hps_cstr_parse_bool(fixture.data, sizeof(fixture.data), &result) ==
        HPS_STRING_NOT_TERMINATED);
    assert(result == 91U);
    assert(fixture.data[0] == 'x' && fixture.data[15] == 'x');
    return INT32_C(0);
}
