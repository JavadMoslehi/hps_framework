#include "../test_suites.h"
#include "hps/base/hps_bool.h"
#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"

#include <limits.h>

/**
 * @file tests/base/test_standard_types.c
 * @brief Standard type boundaries, Boolean conversions and checked outputs.
 */

struct hps_standard_offset_fixture {
    uint8_t prefix;
    uint32_t value;
};

static int32_t bool_text_matches(const char *actual, const char *expected,
                                size_t extent)
{
    size_t index;
    for (index = (size_t)0; index < extent; ++index) {
        if (actual[index] != expected[index]) {
            return INT32_C(0);
        }
    }
    return INT32_C(1);
}

int32_t test_standard_types(void)
{
    uint16_t value;
    bool result = false;
    uint8_t values[3] = {UINT8_C(1), UINT8_C(2), UINT8_C(3)};
    ptrdiff_t difference = &values[2] - &values[0];
    size_t count = sizeof(values) / sizeof(values[0]);

    if ((sizeof(int8_t) * CHAR_BIT != 8U) ||
        (sizeof(uint8_t) * CHAR_BIT != 8U) ||
        (sizeof(int16_t) * CHAR_BIT != 16U) ||
        (sizeof(uint16_t) * CHAR_BIT != 16U) ||
        (sizeof(int32_t) * CHAR_BIT != 32U) ||
        (sizeof(uint32_t) * CHAR_BIT != 32U) ||
        (sizeof(int64_t) * CHAR_BIT != 64U) ||
        (sizeof(uint64_t) * CHAR_BIT != 64U)) {
        return INT32_C(1);
    }
    if ((difference != (ptrdiff_t)2) || (count != (size_t)3) ||
        (offsetof(struct hps_standard_offset_fixture, prefix) != (size_t)0) ||
        (offsetof(struct hps_standard_offset_fixture, value) < sizeof(uint8_t)) ||
        (offsetof(struct hps_standard_offset_fixture, value) >
         sizeof(struct hps_standard_offset_fixture) - sizeof(uint32_t))) {
        return INT32_C(2);
    }
    if (!bool_text_matches(hps_bool_to_cstr(false), "false", sizeof("false")) ||
        !bool_text_matches(hps_bool_to_cstr(true), "true", sizeof("true")) ||
        (hps_bool_to_char(false) != (uint8_t)'0') ||
        (hps_bool_to_char(true) != (uint8_t)'1') ||
        (hps_bool_to_u8(false) != UINT8_C(0)) ||
        (hps_bool_to_u8(true) != UINT8_C(1)) ||
        (hps_bool_to_i32(false) != INT32_C(0)) ||
        (hps_bool_to_i32(true) != INT32_C(1))) {
        return INT32_C(3);
    }
    /* The wider loop counter reaches 256 without wrapping uint8_t. */
    for (value = UINT16_C(0); value <= UINT16_C(255); ++value) {
        uint8_t octet = (uint8_t)value;
        bool expected = (value != UINT16_C(0));
        if ((hps_bool_from_u8(octet) != expected) ||
            (hps_bool_to_u8(hps_bool_from_u8(octet)) !=
             ((value == UINT16_C(0)) ? UINT8_C(0) : UINT8_C(1))) ||
            (hps_bool_is_valid_char(octet) !=
             ((octet == (uint8_t)'0') || (octet == (uint8_t)'1'))) ||
            (hps_bool_from_char(octet) != (octet == (uint8_t)'1'))) {
            return INT32_C(4);
        }
    }
    if (hps_bool_from_i32(INT32_C(0)) ||
        !hps_bool_from_i32(-INT32_C(1)) ||
        !hps_bool_from_i32(INT32_MIN) ||
        !hps_bool_from_i32(INT32_MAX)) {
        return INT32_C(5);
    }
    if (!hps_bool_from_char_checked((uint8_t)'1', &result) || !result) {
        return INT32_C(6);
    }
    if (!hps_bool_from_char_checked((uint8_t)'0', &result) || result) {
        return INT32_C(7);
    }
    result = true;
    if (hps_bool_from_char_checked((uint8_t)'x', &result) || result) {
        return INT32_C(8);
    }
    if (hps_bool_from_char_checked((uint8_t)'1', NULL) ||
        hps_bool_from_char_checked((uint8_t)'x', NULL)) {
        return INT32_C(9);
    }
    return INT32_C(0);
}
