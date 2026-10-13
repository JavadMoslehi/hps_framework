#include "../test_suites.h"
#include "hps/string/view.h"
#include "hps/string/char.h"
#include <assert.h>
#include <string.h>

/**
 * @file tests/string/test_string_extended.c
 * @brief Character, split, bounded interop and extended view boundary checks.
 */

static void test_access_and_windows(void)
{
    const hps_string_view view = {"xabcx", 5U};
    const hps_string_view empty = {NULL, 0U};
    const hps_string_view bad = {NULL, 1U};
    hps_string_view out = {"keep", 4U};
    uint8_t byte = 99U;
    size_t length = 99U;
    const char *data = NULL;
    assert(hps_string_view_at(view, 1U, &byte) == HPS_STRING_OK && byte == 97U);
    assert(hps_string_view_front(view, &byte) == HPS_STRING_OK && byte == 120U);
    assert(hps_string_view_back(view, &byte) == HPS_STRING_OK && byte == 120U);
    assert(hps_string_view_at(view, SIZE_MAX, &byte) == HPS_STRING_OUT_OF_RANGE && byte == 120U);
    assert(hps_string_view_back(empty, &byte) == HPS_STRING_OUT_OF_RANGE && byte == 120U);
    assert(hps_string_view_at(bad, 0U, &byte) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_view_front(view, NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_view_data(view, &data) == HPS_STRING_OK && data == view.data);
    assert(hps_string_view_length(view, &length) == HPS_STRING_OK && length == 5U);
    assert(hps_string_view_is_empty(empty, &byte) == HPS_STRING_OK && byte == 1U);
    assert(hps_string_view_is_valid(bad) == 0U && hps_string_view_is_valid(empty) == 1U);
    assert(hps_string_view_left(view, 2U, &out) == HPS_STRING_OK && out.data == view.data && out.length == 2U);
    assert(hps_string_view_right(view, 2U, &out) == HPS_STRING_OK && out.data == view.data + 3U);
    assert(hps_string_view_right(view, SIZE_MAX, &out) == HPS_STRING_OUT_OF_RANGE && out.length == 2U);
    assert(hps_string_view_remove_prefix(&out, 1U) == HPS_STRING_OK && out.length == 1U);
    assert(hps_string_view_remove_suffix(&out, 1U) == HPS_STRING_OK && out.data == NULL && out.length == 0U);
    assert(hps_string_view_remove_suffix(&out, 1U) == HPS_STRING_OUT_OF_RANGE);
    out = view;
    assert(hps_string_view_trim_prefix_char(&out, 120U) == HPS_STRING_OK && out.length == 4U);
    assert(hps_string_view_trim_suffix_char(&out, 120U) == HPS_STRING_OK && out.length == 3U);
    assert(hps_string_view_trim_char(view, 120U, &out) == HPS_STRING_OK && out.length == 3U);
    assert(hps_string_view_trim_prefix_char(NULL, 120U) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_view_remove_prefix(NULL, 0U) == HPS_STRING_INVALID_ARGUMENT);
    char copy[6] = "#####";
    size_t copied = 99U;
    assert(hps_string_view_copy(view, copy, 3U, 3U, 1U, &copied) == HPS_STRING_OK);
    assert(copied == 3U && memcmp(copy, "abc##", sizeof(copy)) == 0);
    assert(hps_string_view_copy(view, copy, 2U, 3U, 1U, &copied) == HPS_STRING_NO_SPACE && copied == 3U);
    assert(hps_string_view_copy(view, copy, sizeof(copy), SIZE_MAX, 0U, &copied) == HPS_STRING_OUT_OF_RANGE);
    assert(hps_string_view_copy(empty, NULL, 0U, 0U, 0U, &copied) == HPS_STRING_OK && copied == 0U);
}

static void test_search_and_interop(void)
{
    const hps_string_view view = {"abaca", 5U};
    const hps_string_view set = {"ac", 2U};
    const hps_string_view empty = {NULL, 0U};
    size_t index = 99U;
    uint8_t flag = 99U;
    assert(hps_string_view_find_char(view, 97U, 1U, &index) == HPS_STRING_OK && index == 2U);
    assert(hps_string_view_rfind_char(view, 97U, HPS_STRING_NPOS, &index) == HPS_STRING_OK && index == 4U);
    assert(hps_string_view_find_char(view, 122U, 0U, &index) == HPS_STRING_OK && index == HPS_STRING_NPOS);
    assert(hps_string_view_find_char(empty, 97U, 1U, &index) == HPS_STRING_OUT_OF_RANGE);
    assert(hps_string_view_contains_char(view, 98U, &flag) == HPS_STRING_OK && flag == 1U);
    assert(hps_string_view_find_first_of(view, set, 1U, &index) == HPS_STRING_OK && index == 2U);
    assert(hps_string_view_find_first_not_of(view, set, 0U, &index) == HPS_STRING_OK && index == 1U);
    assert(hps_string_view_find_last_of(view, set, HPS_STRING_NPOS, &index) == HPS_STRING_OK && index == 4U);
    assert(hps_string_view_find_last_not_of(view, set, HPS_STRING_NPOS, &index) == HPS_STRING_OK && index == 1U);
    assert(hps_string_view_find_first_of(view, empty, 0U, &index) == HPS_STRING_OK && index == HPS_STRING_NPOS);
    assert(hps_string_view_find_last_not_of(view, empty, HPS_STRING_NPOS, &index) == HPS_STRING_OK && index == 4U);
    assert(hps_string_view_find_first_of(view, set, SIZE_MAX, &index) == HPS_STRING_OUT_OF_RANGE);
    assert(hps_string_view_equals_cstr(view, "abaca", 6U, &flag) == HPS_STRING_OK && flag == 1U);
    assert(hps_string_view_starts_with_cstr(view, "ab", 3U, &flag) == HPS_STRING_OK && flag == 1U);
    assert(hps_string_view_ends_with_cstr(view, "ca", 3U, &flag) == HPS_STRING_OK && flag == 1U);
    assert(hps_string_view_contains_cstr(view, "bac", 4U, &flag) == HPS_STRING_OK && flag == 1U);
    assert(hps_string_view_find_cstr(view, "a", 2U, 1U, &index) == HPS_STRING_OK && index == 2U);
    assert(hps_string_view_rfind_cstr(view, "a", 2U, HPS_STRING_NPOS, &index) == HPS_STRING_OK && index == 4U);
    flag = 99U;
    assert(hps_string_view_contains_cstr(view, "ab", 2U, &flag) == HPS_STRING_NOT_TERMINATED && flag == 99U);
}

static void test_split_atomicity(void)
{
    const hps_string_view view = {",a,,b,", 6U};
    const hps_string_view delimiter = {",", 1U};
    hps_string_view parts[5];
    size_t i;
    size_t count = 99U;
    for (i = 0U; i < 5U; ++i) { parts[i].data = "keep"; parts[i].length = 4U; }
    assert(hps_string_view_split(view, delimiter, parts, 4U, 1U, &count) == HPS_STRING_NO_SPACE);
    assert(count == 99U);
    for (i = 0U; i < 5U; ++i) { assert(parts[i].length == 4U && memcmp(parts[i].data, "keep", 4U) == 0); }
    assert(hps_string_view_split_char(view, 44U, parts, 5U, 1U, &count) == HPS_STRING_OK && count == 5U);
    assert(parts[0].data == NULL && parts[0].length == 0U);
    assert(parts[1].length == 1U && parts[1].data[0] == 'a');
    assert(parts[2].data == NULL && parts[4].data == NULL);
    assert(hps_string_view_split(view, delimiter, parts, 5U, 0U, &count) == HPS_STRING_OK && count == 2U);
    assert(parts[0].data[0] == 'a' && parts[1].data[0] == 'b');
    size_t cursor = 0U;
    uint8_t found = 99U;
    hps_string_view part = {"keep", 4U};
    assert(hps_string_view_split_next_char(view, 44U, &cursor, &part, 0U, &found) == HPS_STRING_OK && found == 1U);
    assert(part.length == 1U && part.data[0] == 'a');
    assert(hps_string_view_split_next(view, delimiter, &cursor, &part, 0U, &found) == HPS_STRING_OK && found == 1U);
    assert(part.data[0] == 'b');
    assert(hps_string_view_split_next(view, delimiter, &cursor, &part, 0U, &found) == HPS_STRING_OK && found == 0U);
    assert(cursor == HPS_STRING_NPOS && part.data[0] == 'b');
    cursor = 7U; found = 99U;
    assert(hps_string_view_split_next(view, delimiter, &cursor, &part, 0U, &found) == HPS_STRING_OUT_OF_RANGE);
    assert(cursor == 7U && found == 99U && part.data[0] == 'b');
    const hps_string_view empty = {NULL, 0U};
    assert(hps_string_view_split(view, empty, parts, 5U, 0U, &count) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_view_split(empty, delimiter, NULL, 0U, 0U, &count) == HPS_STRING_OK && count == 0U);
    assert(hps_string_view_split(empty, delimiter, parts, 5U, 1U, &count) == HPS_STRING_OK && count == 1U);
    assert(hps_string_view_split(view, delimiter, parts, 5U, 2U, &count) == HPS_STRING_INVALID_ARGUMENT);
    const hps_string_view doubled = {"::a::::b::", 10U};
    const hps_string_view double_delimiter = {"::", 2U};
    assert(hps_string_view_split(doubled, double_delimiter, parts, 5U, 1U, &count) == HPS_STRING_OK && count == 5U);
    assert(parts[1].data[0] == 'a' && parts[3].data[0] == 'b');
}

static void test_ascii_domain(void)
{
    uint16_t raw;
    for (raw = 0U; raw < 256U; ++raw) {
        const uint8_t value = (uint8_t)raw;
        assert(hps_char_is_binary(value) == (uint8_t)((raw == 48U) || (raw == 49U)));
        assert(hps_char_is_octal(value) == (uint8_t)((raw >= 48U) && (raw <= 55U)));
        assert(hps_char_is_decimal(value) == (uint8_t)((raw >= 48U) && (raw <= 57U)));
        assert(hps_char_is_hex_lower(value) == (uint8_t)((raw >= 97U) && (raw <= 102U)));
        assert(hps_char_is_hex_upper(value) == (uint8_t)((raw >= 65U) && (raw <= 70U)));
        assert(hps_char_is_alpha_lower(value) == (uint8_t)((raw >= 97U) && (raw <= 122U)));
        assert(hps_char_is_alpha_upper(value) == (uint8_t)((raw >= 65U) && (raw <= 90U)));
        assert(hps_char_is_whitespace(value) == (uint8_t)((raw == 32U) || (raw == 9U) || (raw == 10U) || (raw == 11U) || (raw == 12U) || (raw == 13U)));
        assert(hps_char_is_printable(value) == (uint8_t)((raw >= 32U) && (raw <= 126U)));
        assert(hps_char_is_control(value) == (uint8_t)((raw < 32U) || (raw == 127U)));
        assert(hps_char_is_sign(value) == (uint8_t)((raw == 43U) || (raw == 45U)));
        assert(hps_char_is_path_separator(value) == (uint8_t)((raw == 47U) || (raw == 92U)));
        assert(hps_char_is_line_ending(value) == (uint8_t)((raw == 10U) || (raw == 13U)));
        assert(hps_char_to_lower(value) == (uint8_t)(((raw >= 65U) && (raw <= 90U)) ? raw + 32U : raw));
        assert(hps_char_to_upper(value) == (uint8_t)(((raw >= 97U) && (raw <= 122U)) ? raw - 32U : raw));
        assert(hps_char_is_alpha(value) == (uint8_t)(((raw >= 65U) && (raw <= 90U)) || ((raw >= 97U) && (raw <= 122U))));
        assert(hps_char_is_alphanumeric(value) == (uint8_t)((hps_char_is_alpha(value) == 1U) || ((raw >= 48U) && (raw <= 57U))));
        assert(hps_char_is_hex(value) == (uint8_t)(((raw >= 48U) && (raw <= 57U)) || ((raw >= 65U) && (raw <= 70U)) || ((raw >= 97U) && (raw <= 102U))));
        const uint8_t nibble = (uint8_t)(raw % 16U);
        assert(hps_char_nibble_to_hex_lower(value) == (uint8_t)"0123456789abcdef"[nibble]);
        assert(hps_char_nibble_to_hex_upper(value) == (uint8_t)"0123456789ABCDEF"[nibble]);
        const int32_t expected = ((raw >= 48U) && (raw <= 57U)) ? (int32_t)(raw - 48U) :
            (((raw >= 65U) && (raw <= 70U)) ? (int32_t)(raw - 65U) + 10 :
            (((raw >= 97U) && (raw <= 102U)) ? (int32_t)(raw - 97U) + 10 : -1));
        assert(hps_char_hex_to_nibble(value) == expected);
    }
}

int32_t test_string_extended(void)
{
    test_access_and_windows();
    test_search_and_interop();
    test_split_atomicity();
    test_ascii_domain();
    return INT32_C(0);
}
