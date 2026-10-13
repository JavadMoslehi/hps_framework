#include "../test_suites.h"
#include "hps/cstr/format.h"
#include <assert.h>
#include <string.h>

/**
 * @file tests/cstr/test_cstr_format.c
 * @brief Typed formatting, fixed-decimal boundaries and atomic failures.
 */

static void test_numeric_boundaries(void)
{
    char storage[80] = "keep";
    hps_string_buffer buffer = {storage, 4U, sizeof(storage)};
    hps_string_status status = hps_cstr_render_decimal(&buffer, INT64_MIN, 0U);
    assert(status == HPS_STRING_OK && strcmp(storage, "-9223372036854775808") == 0);
    status = hps_cstr_render_decimal(&buffer, INT64_MAX, 18U);
    assert(status == HPS_STRING_OK && strcmp(storage, "9.223372036854775807") == 0);
    status = hps_cstr_render_decimal(&buffer, INT64_MIN, 18U);
    assert(status == HPS_STRING_OK && strcmp(storage, "-9.223372036854775808") == 0);
    status = hps_cstr_render_decimal(&buffer, -INT64_C(5), 2U);
    assert(status == HPS_STRING_OK && strcmp(storage, "-0.05") == 0);
    status = hps_cstr_render_decimal(&buffer, INT64_C(12345), 2U);
    assert(status == HPS_STRING_OK && strcmp(storage, "123.45") == 0);
    status = hps_cstr_render_decimal(&buffer, INT64_C(0), 18U);
    assert(status == HPS_STRING_OK && strcmp(storage, "0.000000000000000000") == 0);
    status = hps_cstr_render_decimal(&buffer, INT64_C(1), 19U);
    assert(status == HPS_STRING_INVALID_ARGUMENT && strcmp(storage, "0.000000000000000000") == 0);
    char tiny[2] = "x";
    hps_string_buffer small = {tiny, 1U, sizeof(tiny)};
    status = hps_cstr_render_decimal(&small, INT64_MIN, 0U);
    assert(status == HPS_STRING_NO_SPACE && small.length == 1U && strcmp(tiny, "x") == 0);
}

static void test_grammar_and_aliasing(void)
{
    char storage[160] = "keep";
    char scratch[160] = "scratch";
    hps_string_buffer buffer = {storage, 4U, sizeof(storage)};
    const hps_cstr_format_arg arguments[] = {
        {HPS_CSTR_ARG_TEXT, {"hello", 5U}, INT64_C(0), UINT64_C(0), 0U, 0U},
        {HPS_CSTR_ARG_TEXT, {"!", 1U}, INT64_C(0), UINT64_C(0), 0U, 0U},
        {HPS_CSTR_ARG_I64, {NULL, 0U}, INT64_MIN, UINT64_C(0), 0U, 0U},
        {HPS_CSTR_ARG_U64, {NULL, 0U}, INT64_C(0), UINT64_MAX, 0U, 0U},
        {HPS_CSTR_ARG_U64, {NULL, 0U}, INT64_C(0), UINT64_C(255), 0U, 0U},
        {HPS_CSTR_ARG_U64, {NULL, 0U}, INT64_C(0), UINT64_C(255), 0U, 0U},
        {HPS_CSTR_ARG_BOOL, {NULL, 0U}, INT64_C(0), UINT64_C(0), 1U, 0U},
        {HPS_CSTR_ARG_DECIMAL, {NULL, 0U}, -INT64_C(5), UINT64_C(0), 0U, 2U}
    };
    const char *pattern = "%s%c %lld %llu %x %X %b %f %%";
    hps_string_status status = hps_cstr_format(&buffer, scratch, sizeof(scratch),
        pattern, strlen(pattern) + 1U, arguments, sizeof(arguments) / sizeof(arguments[0]));
    assert(status == HPS_STRING_OK);
    assert(strcmp(storage, "hello! -9223372036854775808 18446744073709551615 ff FF true -0.05 %") == 0);
    const hps_cstr_format_arg aliases[] = {
        {HPS_CSTR_ARG_TEXT, {storage, 6U}, INT64_C(0), UINT64_C(0), 0U, 0U}
    };
    status = hps_cstr_format(&buffer, scratch, sizeof(scratch), "[%s]", 5U, aliases, 1U);
    assert(status == HPS_STRING_OK && strcmp(storage, "[hello!]") == 0);
    const hps_cstr_format_arg false_arg[] = {
        {HPS_CSTR_ARG_BOOL, {NULL, 0U}, INT64_C(0), UINT64_C(0), 0U, 0U}
    };
    status = hps_cstr_append_format(&buffer, scratch, sizeof(scratch), " %b", 4U, false_arg, 1U);
    assert(status == HPS_STRING_OK && strcmp(storage, "[hello!] false") == 0);
    status = hps_cstr_format(&buffer, scratch, sizeof(scratch), "", 1U, NULL, 0U);
    assert(status == HPS_STRING_OK && buffer.length == 0U && storage[0] == '\0');
    status = hps_cstr_format(&buffer, scratch, sizeof(scratch), "plain", 6U, NULL, 0U);
    assert(status == HPS_STRING_OK && strcmp(storage, "plain") == 0);
}

static void assert_bad_format(hps_string_buffer *buffer, char *scratch,
                              const char *pattern, const size_t capacity,
                              const hps_cstr_format_arg *args, const size_t count,
                              const hps_string_status expected)
{
    char destination_before[16];
    char scratch_before[16];
    const size_t length_before = buffer->length;
    (void)memcpy(destination_before, buffer->data, 16U);
    (void)memcpy(scratch_before, scratch, 16U);
    const hps_string_status status = hps_cstr_format(buffer, scratch, 16U,
                                                   pattern, capacity, args, count);
    assert(status == expected && buffer->length == length_before);
    assert(memcmp(destination_before, buffer->data, 16U) == 0);
    assert(memcmp(scratch_before, scratch, 16U) == 0);
}

static void test_failures_and_fit(void)
{
    char storage[16] = "keep";
    char scratch[16] = "scratch";
    hps_string_buffer buffer = {storage, 4U, sizeof(storage)};
    hps_cstr_format_arg argument = {HPS_CSTR_ARG_I64, {NULL, 0U}, INT64_C(42), UINT64_C(0), 0U, 0U};
    assert_bad_format(&buffer, scratch, "%s", 3U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "%d", 3U, NULL, 0U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "plain", 6U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "%", 2U, NULL, 0U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "%ll", 4U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "%n", 3U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "%08d", 5U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "%lf", 4U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, NULL, 1U, NULL, 0U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "abc", 0U, NULL, 0U, HPS_STRING_INVALID_ARGUMENT);
    const char unterminated[3] = {'a','b','c'};
    assert_bad_format(&buffer, scratch, unterminated, sizeof(unterminated), NULL, 0U, HPS_STRING_NOT_TERMINATED);
    argument.type = HPS_CSTR_ARG_TEXT;
    argument.text.data = "a\0b";
    argument.text.length = 3U;
    assert_bad_format(&buffer, scratch, "%s", 3U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    argument.text.data = NULL;
    argument.text.length = 1U;
    assert_bad_format(&buffer, scratch, "%s", 3U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    argument.text.data = "ab";
    argument.text.length = 2U;
    assert_bad_format(&buffer, scratch, "%c", 3U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    argument.type = HPS_CSTR_ARG_BOOL;
    argument.flag = 2U;
    assert_bad_format(&buffer, scratch, "%b", 3U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    argument.type = HPS_CSTR_ARG_DECIMAL;
    argument.scale = 19U;
    assert_bad_format(&buffer, scratch, "%f", 3U, &argument, 1U, HPS_STRING_INVALID_ARGUMENT);
    assert_bad_format(&buffer, scratch, "1234567890123456", 17U, NULL, 0U, HPS_STRING_NO_SPACE);
    hps_string_status status = hps_cstr_format(&buffer, scratch, 2U, "abc", 4U, NULL, 0U);
    assert(status == HPS_STRING_NO_SPACE && strcmp(storage, "keep") == 0 && strcmp(scratch, "scratch") == 0);
    status = hps_cstr_format(&buffer, NULL, 1U, "x", 2U, NULL, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_format(&buffer, scratch, 16U, "x", 2U, NULL, 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_format(NULL, scratch, 16U, "x", 2U, NULL, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    char exact[4] = "x";
    hps_string_buffer small = {exact, 1U, sizeof(exact)};
    status = hps_cstr_format(&small, scratch, sizeof(scratch), "abc", 4U, NULL, 0U);
    assert(status == HPS_STRING_OK && strcmp(exact, "abc") == 0);
    status = hps_cstr_append_format(&small, scratch, sizeof(scratch), "x", 2U, NULL, 0U);
    assert(status == HPS_STRING_NO_SPACE && strcmp(exact, "abc") == 0);
    buffer.length = SIZE_MAX;
    status = hps_cstr_format(&buffer, scratch, 16U, "x", 2U, NULL, 0U);
    assert(status == HPS_STRING_INVALID_STATE && strcmp(storage, "keep") == 0);
}

int32_t test_cstr_format(void)
{
    test_numeric_boundaries();
    test_grammar_and_aliasing();
    test_failures_and_fit();
    return INT32_C(0);
}
