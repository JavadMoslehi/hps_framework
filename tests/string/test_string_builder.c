#include "../test_suites.h"
#include "hps/string/hps_builder.h"
#include "hps/string/hps_fixed.h"
#include "hps/string/hps_policy.h"
#include "policy_expectations.h"
#include <assert.h>
#include <string.h>

/**
 * @file tests/string/test_string_builder.c
 * @brief Bounded editing, alias staging, JSON escaping and typed fixed storage.
 */

HPS_STRING_FIXED_DEFINE(test_fixed, 16U)

static void test_fixed_and_lifecycle(void)
{
    test_fixed_t storage = HPS_STRING_FIXED_STATIC_INIT;
    hps_string_builder builder = {NULL, 0U, 0U};
    hps_string_view view = {NULL, 0U};
    assert(test_fixed_capacity() == 16U);
    assert(test_fixed_bind(&storage, &builder) == HPS_STRING_OK);
    assert(builder.data == storage.data && builder.length == 0U && builder.capacity == 16U);
    const hps_string_view text = {"abc", 3U};
    assert(hps_string_builder_assign_view(&builder, text) == HPS_STRING_OK);
    assert(test_fixed_adopt(&storage, &builder) == HPS_STRING_OK && builder.length == 3U);
    const hps_string_view digits = {"123", 3U};
    assert(hps_string_policy_assign(&builder, digits, HPS_STRING_POLICY_DIGIT) == HPS_STRING_OK);
    assert(hps_string_policy_append(&builder, text, HPS_STRING_POLICY_DIGIT) == HPS_STRING_INVALID_ARGUMENT && strcmp(storage.data, "123") == 0);
    assert(hps_string_policy_append(&builder, digits, HPS_STRING_POLICY_DIGIT) == HPS_STRING_OK && strcmp(storage.data, "123123") == 0);
    assert(hps_string_policy_assign(&builder, text, UINT8_MAX) == HPS_STRING_INVALID_ARGUMENT && strcmp(storage.data, "123123") == 0);
    assert(hps_string_builder_assign_view(&builder, text) == HPS_STRING_OK);
    assert(test_fixed_bind(NULL, &builder) == HPS_STRING_INVALID_ARGUMENT && builder.length == 3U);
    assert(test_fixed_adopt(&storage, NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_builder_as_view(&builder, &view) == HPS_STRING_OK && view.data == storage.data && view.length == 3U);
    assert(hps_string_builder_reserve(&builder, 15U) == HPS_STRING_OK);
    assert(hps_string_builder_reserve(&builder, 16U) == HPS_STRING_NO_SPACE);
    assert(hps_string_builder_reserve(&builder, SIZE_MAX) == HPS_STRING_NO_SPACE);
    assert(hps_string_builder_append_char(&builder, 33U) == HPS_STRING_OK);
    assert(hps_string_builder_append_repeat_char(&builder, 120U, 2U) == HPS_STRING_OK);
    assert(strcmp(storage.data, "abc!xx") == 0);
    assert(hps_string_builder_resize(&builder, 8U, 121U) == HPS_STRING_OK && strcmp(storage.data, "abc!xxyy") == 0);
    assert(hps_string_builder_resize(&builder, 2U, 121U) == HPS_STRING_OK && strcmp(storage.data, "ab") == 0);
    assert(hps_string_builder_resize(&builder, 0U, 0U) == HPS_STRING_INVALID_ARGUMENT && strcmp(storage.data, "ab") == 0);
    assert(hps_string_builder_append_char(&builder, 128U) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_builder_append_repeat_char(&builder, 120U, SIZE_MAX) == HPS_STRING_NO_SPACE);
    assert(hps_string_builder_append_newline(&builder) == HPS_STRING_OK && strcmp(storage.data, "ab\n") == 0);
    char other_storage[8];
    hps_string_builder other = {NULL, 0U, 0U};
    assert(hps_string_builder_init(&other, other_storage, sizeof(other_storage)) == HPS_STRING_OK);
    assert(hps_string_builder_append_view(&other, text) == HPS_STRING_OK);
    assert(hps_string_builder_swap(&builder, &other) == HPS_STRING_OK);
    assert(builder.data == other_storage && other.data == storage.data);
    assert(hps_string_builder_swap(&builder, &builder) == HPS_STRING_OK);
    assert(hps_string_builder_clear(&builder) == HPS_STRING_OK && other_storage[0] == '\0');
    assert(hps_string_builder_swap(&builder, NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_policy_accept(HPS_STRING_POLICY_EMAIL_CHAR, 64U) == 1U);
    assert(hps_string_policy_accept(HPS_STRING_POLICY_IPV6_CHAR, 58U) == 1U);
    assert(hps_string_policy_accept(HPS_STRING_POLICY_BASE64_URL, 95U) == 1U);
    assert(hps_string_policy_accept(HPS_STRING_POLICY_NONE, 65U) == 0U);
    assert(hps_string_policy_accept(HPS_STRING_POLICY_ANY, UINT8_MAX) == 1U);
    assert(hps_string_policy_accept(UINT8_MAX, 65U) == 0U);
}

static void test_edits(void)
{
    char storage[64] = "abc";
    char scratch[64] = "untouched";
    hps_string_builder builder = {storage, 3U, sizeof(storage)};
    const hps_string_view inserted = {"XY", 2U};
    assert(hps_string_builder_insert_view(&builder, 1U, inserted, scratch, sizeof(scratch)) == HPS_STRING_OK);
    assert(strcmp(storage, "aXYbc") == 0);
    const hps_string_view aliased = {storage + 1U, 2U};
    assert(hps_string_builder_prepend_view(&builder, aliased, scratch, sizeof(scratch)) == HPS_STRING_OK);
    assert(strcmp(storage, "XYaXYbc") == 0);
    assert(hps_string_builder_erase(&builder, 0U, 2U) == HPS_STRING_OK && strcmp(storage, "aXYbc") == 0);
    const hps_string_view empty = {NULL, 0U};
    assert(hps_string_builder_replace_range_view(&builder, 1U, 2U, empty, scratch, sizeof(scratch)) == HPS_STRING_OK);
    assert(strcmp(storage, "abc") == 0);
    const hps_string_view pattern = {"b", 1U};
    uint8_t changed = 97U;
    assert(hps_string_builder_replace_first(&builder, pattern, inserted, scratch, sizeof(scratch), &changed) == HPS_STRING_OK);
    assert(changed == 1U && strcmp(storage, "aXYc") == 0);
    assert(hps_string_builder_replace_first(&builder, pattern, inserted, NULL, 0U, &changed) == HPS_STRING_OK && changed == 0U);
    const hps_string_view repeated = {"aaaa", 4U};
    assert(hps_string_builder_assign_view(&builder, repeated) == HPS_STRING_OK);
    const hps_string_view aa = {storage, 2U};
    const hps_string_view a = {storage, 1U};
    size_t count = 97U;
    assert(hps_string_builder_replace_all(&builder, aa, a, scratch, sizeof(scratch), &count) == HPS_STRING_OK);
    assert(count == 2U && strcmp(storage, "aa") == 0);
    assert(hps_string_builder_replace_char_all(&builder, 97U, 98U, &count) == HPS_STRING_OK && count == 2U && strcmp(storage, "bb") == 0);
    const hps_string_view padded = {" \tAbC \n", 7U};
    assert(hps_string_builder_assign_view(&builder, padded) == HPS_STRING_OK);
    assert(hps_string_builder_trim_ascii_whitespace(&builder) == HPS_STRING_OK && strcmp(storage, "AbC") == 0);
    assert(hps_string_builder_to_lower_ascii(&builder) == HPS_STRING_OK && strcmp(storage, "abc") == 0);
    assert(hps_string_builder_to_upper_ascii(&builder) == HPS_STRING_OK && strcmp(storage, "ABC") == 0);
    const hps_string_view edges = {"xABxx", 5U};
    assert(hps_string_builder_assign_view(&builder, edges) == HPS_STRING_OK);
    assert(hps_string_builder_trim_char(&builder, 120U) == HPS_STRING_OK && strcmp(storage, "AB") == 0);
}

static void test_composition(void)
{
    char storage[128] = "p:";
    char scratch[128];
    hps_string_builder builder = {storage, 2U, sizeof(storage)};
    const hps_string_view plain = {"abc", 3U};
    assert(hps_string_builder_append_line(&builder, plain, scratch, sizeof(scratch)) == HPS_STRING_OK);
    assert(strcmp(storage, "p:abc\n") == 0);
    assert(hps_string_builder_clear(&builder) == HPS_STRING_OK);
    assert(hps_string_builder_append_quoted(&builder, plain, scratch, sizeof(scratch)) == HPS_STRING_OK && strcmp(storage, "\"abc\"") == 0);
    assert(hps_string_builder_clear(&builder) == HPS_STRING_OK);
    const hps_string_view controls = {"\"\\\b\t\n\f\r\x01\0z", 10U};
    const char *expected = "\\\"\\\\\\b\\t\\n\\f\\r\\u0001\\u0000z";
    assert(hps_string_builder_append_json_escaped(&builder, controls, scratch, sizeof(scratch)) == HPS_STRING_OK);
    assert(strcmp(storage, expected) == 0);
    assert(hps_string_builder_clear(&builder) == HPS_STRING_OK);
    assert(hps_string_builder_append_json_quoted(&builder, controls, scratch, sizeof(scratch)) == HPS_STRING_OK);
    assert(storage[0] == '"' && storage[builder.length - 1U] == '"');
    assert(memcmp(storage + 1U, expected, strlen(expected)) == 0);
    assert(hps_string_builder_clear(&builder) == HPS_STRING_OK);
    assert(hps_string_builder_append_view(&builder, plain) == HPS_STRING_OK);
    const hps_string_view aliased = {storage, 3U};
    assert(hps_string_builder_append_json_quoted(&builder, aliased, scratch, sizeof(scratch)) == HPS_STRING_OK && strcmp(storage, "abc\"abc\"") == 0);
}

static void test_error_preservation(void)
{
    char storage[8] = "keep###";
    storage[4] = '\0';
    char scratch[8] = "scratch";
    char before[8];
    (void)memcpy(before, storage, sizeof(storage));
    hps_string_builder builder = {storage, 4U, sizeof(storage)};
    const hps_string_view text = {"abc", 3U};
    const hps_string_view malformed = {NULL, 1U};
    const hps_string_view empty = {NULL, 0U};
    const hps_string_view nul = {"a\0b", 3U};
    size_t count = 97U;
    uint8_t changed = 97U;
    assert(hps_string_builder_insert_view(&builder, SIZE_MAX, text, scratch, sizeof(scratch)) == HPS_STRING_OUT_OF_RANGE);
    assert(hps_string_builder_replace_range_view(&builder, 1U, SIZE_MAX, text, scratch, sizeof(scratch)) == HPS_STRING_OUT_OF_RANGE);
    assert(hps_string_builder_insert_view(&builder, 0U, text, scratch, 1U) == HPS_STRING_NO_SPACE);
    assert(hps_string_builder_insert_view(&builder, 0U, malformed, scratch, sizeof(scratch)) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_builder_insert_view(&builder, 0U, nul, scratch, sizeof(scratch)) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_builder_replace_first(&builder, empty, text, scratch, sizeof(scratch), &changed) == HPS_STRING_INVALID_ARGUMENT && changed == 97U);
    assert(hps_string_builder_replace_all(&builder, empty, text, scratch, sizeof(scratch), &count) == HPS_STRING_INVALID_ARGUMENT && count == 97U);
    const hps_string_view e = {"e", 1U};
    assert(hps_string_builder_replace_all(&builder, e, text, scratch, 1U, &count) == HPS_STRING_NO_SPACE && count == 97U);
    assert(hps_string_builder_replace_char_all(&builder, 101U, 0U, &count) == HPS_STRING_INVALID_ARGUMENT && count == 97U);
    assert(hps_string_builder_append_line(&builder, text, scratch, sizeof(scratch)) == HPS_STRING_NO_SPACE);
    assert(hps_string_builder_append_quoted(&builder, nul, scratch, sizeof(scratch)) == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(storage, before, sizeof(storage)) == 0 && builder.length == 4U);
    assert(memcmp(scratch, "scratch", sizeof(scratch)) == 0);
    assert(hps_string_builder_append_repeat_char(&builder, 120U, SIZE_MAX) == HPS_STRING_NO_SPACE);
    assert(hps_string_builder_replace_all(NULL, e, text, scratch, sizeof(scratch), &count) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_builder_replace_first(&builder, e, text, scratch, sizeof(scratch), NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_builder_insert_view(&builder, 0U, text, NULL, 8U) == HPS_STRING_INVALID_ARGUMENT);
    builder.length = SIZE_MAX;
    assert(hps_string_builder_clear(&builder) == HPS_STRING_INVALID_STATE);
}

int32_t test_string_builder(void)
{
    uint8_t policy;
    uint16_t value;
    for (policy = 0U; policy < 59U; policy += 1U) {
        for (value = 0U; value < 256U; value += 1U) {
            const uint8_t expected = (uint8_t)((policy_expected[policy][value / 8U] >>
                (value % 8U)) & 1U);
            assert(hps_string_policy_accept(policy, (uint8_t)value) == expected);
        }
    }
    test_fixed_and_lifecycle();
    test_edits();
    test_composition();
    test_error_preservation();
    return INT32_C(0);
}
