#include "../test_suites.h"
#include "hps/string/hps_buffer.h"
#include "hps/string/hps_view.h"
#include <assert.h>
#include "hps/base/hps_stdint.h"
#include <string.h>

/**
 * @file tests/string/test_string.c
 * @brief Independent bounded text, failure preservation and overlap checks.
 */

static void test_result_representation(void)
{
    assert(sizeof(hps_string_view_init(NULL, NULL, 0U)) == sizeof(int32_t));
    assert(sizeof(HPS_STRING_OK) == sizeof(int32_t));
    assert(sizeof(HPS_STRING_INVALID_ARGUMENT) == sizeof(int32_t));
    assert(sizeof(HPS_STRING_OUT_OF_RANGE) == sizeof(int32_t));
    assert(sizeof(HPS_STRING_NO_SPACE) == sizeof(int32_t));
    assert(sizeof(HPS_STRING_INVALID_STATE) == sizeof(int32_t));
    assert(sizeof(HPS_STRING_NOT_TERMINATED) == sizeof(int32_t));
    assert(HPS_STRING_OK == INT32_C(0));
    assert(HPS_STRING_INVALID_ARGUMENT == -INT32_C(1));
    assert(HPS_STRING_OUT_OF_RANGE == -INT32_C(2));
    assert(HPS_STRING_NO_SPACE == -INT32_C(3));
    assert(HPS_STRING_INVALID_STATE == -INT32_C(4));
    assert(HPS_STRING_NOT_TERMINATED == -INT32_C(5));
}

static void assert_view(hps_string_view view, const char *data, size_t length)
{
    assert(view.data == data);
    assert(view.length == length);
}

static void assert_buffer(const hps_string_buffer *buffer, const char *data,
                          size_t length, size_t capacity)
{
    assert(buffer->data == data);
    assert(buffer->length == length);
    assert(buffer->capacity == capacity);
}

static void test_view_construction_and_slicing(void)
{
    const char *text = "a\0bc";
    const hps_string_view sentinel = {"keep", 4U};
    hps_string_view view = sentinel;
    int32_t status = hps_string_view_init(&view, text, 4U);
    assert(status == HPS_STRING_OK);
    assert_view(view, text, 4U);

    hps_string_view slice = sentinel;
    status = hps_string_view_slice(view, 1U, 2U, &slice);
    assert(status == HPS_STRING_OK);
    assert_view(slice, text + 1U, 2U);
    status = hps_string_view_slice(view, SIZE_MAX, 0U, &slice);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    assert_view(slice, text + 1U, 2U);
    status = hps_string_view_slice(view, 1U, SIZE_MAX, &slice);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    assert_view(slice, text + 1U, 2U);
    status = hps_string_view_slice(view, 4U, 1U, &slice);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    status = hps_string_view_slice(view, 4U, 0U, &slice);
    assert(status == HPS_STRING_OK);
    assert_view(slice, NULL, 0U);
    status = hps_string_view_slice(view, 5U, 0U, &slice);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    assert_view(slice, NULL, 0U);
    status = hps_string_view_slice(view, 2U, 2U, &view);
    assert(status == HPS_STRING_OK);
    assert_view(view, text + 2U, 2U);

    status = hps_string_view_init(&view, NULL, 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_view(view, text + 2U, 2U);
    status = hps_string_view_init(&view, text, SIZE_MAX);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_view(view, text + 2U, 2U);
    status = hps_string_view_init(&view, text, 0U);
    assert(status == HPS_STRING_OK);
    assert_view(view, NULL, 0U);
    status = hps_string_view_init(&view, NULL, 0U);
    assert(status == HPS_STRING_OK);
    status = hps_string_view_slice(view, 0U, 0U, &slice);
    assert(status == HPS_STRING_OK);
    assert_view(slice, NULL, 0U);

    status = hps_string_view_init_cstr(&view, text, 4U);
    assert(status == HPS_STRING_OK);
    assert_view(view, text, 1U);
    status = hps_string_view_init_cstr(&view, "abcd", 4U);
    assert(status == HPS_STRING_NOT_TERMINATED);
    assert_view(view, text, 1U);
    const char unterminated[4] = {'a', 'b', 'c', 'd'};
    status = hps_string_view_init_cstr(&view, unterminated, sizeof(unterminated));
    assert(status == HPS_STRING_NOT_TERMINATED);
    assert_view(view, text, 1U);
    status = hps_string_view_init_cstr(&view, "abcd", 5U);
    assert(status == HPS_STRING_OK);
    assert(view.length == 4U);
    status = hps_string_view_init_cstr(&view, "", 1U);
    assert(status == HPS_STRING_OK);
    assert_view(view, NULL, 0U);
    status = hps_string_view_init_cstr(&view, NULL, 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_init_cstr(&view, text, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_view(view, NULL, 0U);
}

static void test_view_comparison(void)
{
    const hps_string_view abc = {"abc", 3U};
    const hps_string_view abd = {"abd", 3U};
    const hps_string_view ab = {"ab", 2U};
    const hps_string_view empty = {NULL, 0U};
    const hps_string_view embedded = {"a\0c", 3U};
    const hps_string_view high = {"\x80", 1U};
    const hps_string_view low = {"\x7f", 1U};
    uint8_t flag = 97U;
    int32_t order = INT32_C(97);
    int32_t status = hps_string_view_equal(abc, abc, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_string_view_equal(abc, abd, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_string_view_equal(abc, ab, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_string_view_equal(empty, empty, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_string_view_equal(embedded, embedded, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_string_view_compare(abc, abc, &order);
    assert(status == HPS_STRING_OK && order == 0);
    status = hps_string_view_compare(abc, abd, &order);
    assert(status == HPS_STRING_OK && order == -1);
    status = hps_string_view_compare(abd, abc, &order);
    assert(status == HPS_STRING_OK && order == 1);
    status = hps_string_view_compare(ab, abc, &order);
    assert(status == HPS_STRING_OK && order == -1);
    status = hps_string_view_compare(abc, ab, &order);
    assert(status == HPS_STRING_OK && order == 1);
    status = hps_string_view_compare(empty, abc, &order);
    assert(status == HPS_STRING_OK && order == -1);
    status = hps_string_view_compare(embedded, abc, &order);
    assert(status == HPS_STRING_OK && order == -1);
    status = hps_string_view_compare(high, low, &order);
    assert(status == HPS_STRING_OK && order == 1);
    status = hps_string_view_compare(low, high, &order);
    assert(status == HPS_STRING_OK && order == -1);

    status = hps_string_view_starts_with(abc, ab, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_string_view_starts_with(ab, abc, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_string_view_starts_with(abc, abd, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_string_view_starts_with(abc, empty, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    const hps_string_view bc = {"bc", 2U};
    status = hps_string_view_ends_with(abc, bc, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_string_view_ends_with(ab, abc, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_string_view_ends_with(abc, ab, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_string_view_ends_with(abc, empty, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_string_view_ends_with(empty, empty, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
}

static void test_view_searches(void)
{
    const hps_string_view haystack = {"ababa", 5U};
    const hps_string_view needle = {"aba", 3U};
    const hps_string_view absent = {"z", 1U};
    const hps_string_view overlong = {"ababab", 6U};
    const hps_string_view empty = {NULL, 0U};
    size_t index = 97U;
    uint8_t flag = 97U;
    int32_t status = hps_string_view_find(haystack, needle, 0U, &index);
    assert(status == HPS_STRING_OK && index == 0U);
    status = hps_string_view_find(haystack, needle, 1U, &index);
    assert(status == HPS_STRING_OK && index == 2U);
    status = hps_string_view_find(haystack, needle, 3U, &index);
    assert(status == HPS_STRING_OK && index == HPS_STRING_NPOS);
    status = hps_string_view_find(haystack, absent, 0U, &index);
    assert(status == HPS_STRING_OK && index == HPS_STRING_NPOS);
    status = hps_string_view_find(haystack, overlong, 0U, &index);
    assert(status == HPS_STRING_OK && index == HPS_STRING_NPOS);
    status = hps_string_view_find(haystack, empty, 5U, &index);
    assert(status == HPS_STRING_OK && index == 5U);
    status = hps_string_view_find(haystack, needle, 5U, &index);
    assert(status == HPS_STRING_OK && index == HPS_STRING_NPOS);
    status = hps_string_view_find(haystack, empty, SIZE_MAX, &index);
    assert(status == HPS_STRING_OUT_OF_RANGE && index == HPS_STRING_NPOS);
    status = hps_string_view_find(empty, empty, 0U, &index);
    assert(status == HPS_STRING_OK && index == 0U);
    status = hps_string_view_find(empty, needle, 0U, &index);
    assert(status == HPS_STRING_OK && index == HPS_STRING_NPOS);
    status = hps_string_view_rfind(haystack, needle, HPS_STRING_NPOS, &index);
    assert(status == HPS_STRING_OK && index == 2U);
    status = hps_string_view_rfind(haystack, needle, 1U, &index);
    assert(status == HPS_STRING_OK && index == 0U);
    status = hps_string_view_rfind(haystack, needle, 0U, &index);
    assert(status == HPS_STRING_OK && index == 0U);
    status = hps_string_view_rfind(haystack, absent, 0U, &index);
    assert(status == HPS_STRING_OK && index == HPS_STRING_NPOS);
    status = hps_string_view_rfind(haystack, overlong, HPS_STRING_NPOS, &index);
    assert(status == HPS_STRING_OK && index == HPS_STRING_NPOS);
    status = hps_string_view_rfind(haystack, empty, HPS_STRING_NPOS, &index);
    assert(status == HPS_STRING_OK && index == 5U);
    status = hps_string_view_rfind(haystack, empty, 2U, &index);
    assert(status == HPS_STRING_OK && index == 2U);
    status = hps_string_view_rfind(empty, empty, HPS_STRING_NPOS, &index);
    assert(status == HPS_STRING_OK && index == 0U);
    status = hps_string_view_rfind(empty, needle, HPS_STRING_NPOS, &index);
    assert(status == HPS_STRING_OK && index == HPS_STRING_NPOS);
    status = hps_string_view_contains(haystack, needle, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_string_view_contains(haystack, absent, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_string_view_contains(empty, empty, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);

    const hps_string_view binary_text = {"a\0bc\0d", 6U};
    const hps_string_view nul_needle = {"\0b", 2U};
    status = hps_string_view_find(binary_text, nul_needle, 0U, &index);
    assert(status == HPS_STRING_OK && index == 1U);
    status = hps_string_view_rfind(binary_text, nul_needle, HPS_STRING_NPOS, &index);
    assert(status == HPS_STRING_OK && index == 1U);
}

static void test_view_trimming(void)
{
    const char *text = "\t\n\v\f\r hello \t\n\v\f\r";
    const hps_string_view view = {text, 17U};
    hps_string_view result = {"keep", 4U};
    int32_t status = hps_string_view_trim_whitespace(view, &result);
    assert(status == HPS_STRING_OK);
    assert_view(result, text + 6U, 5U);
    assert_view(view, text, 17U);

    const char *custom_text = "xyabcyxx";
    const hps_string_view custom = {custom_text, 8U};
    const hps_string_view chars = {"xy", 2U};
    status = hps_string_view_trim(custom, chars, &result);
    assert(status == HPS_STRING_OK);
    assert_view(result, custom_text + 2U, 3U);
    const hps_string_view empty = {NULL, 0U};
    status = hps_string_view_trim(custom, empty, &result);
    assert(status == HPS_STRING_OK);
    assert_view(result, custom_text, 8U);
    const hps_string_view all_members = {"xyxy", 4U};
    status = hps_string_view_trim(all_members, chars, &result);
    assert(status == HPS_STRING_OK);
    assert_view(result, NULL, 0U);
    status = hps_string_view_trim(empty, chars, &result);
    assert(status == HPS_STRING_OK);
    assert_view(result, NULL, 0U);
    const hps_string_view all_space = {" \t\n\r\v\f", 6U};
    status = hps_string_view_trim_whitespace(all_space, &result);
    assert(status == HPS_STRING_OK);
    assert_view(result, NULL, 0U);
    status = hps_string_view_trim_whitespace(empty, &result);
    assert(status == HPS_STRING_OK);
    assert_view(result, NULL, 0U);
    const hps_string_view high = {"\x80hello\x80", 7U};
    status = hps_string_view_trim_whitespace(high, &result);
    assert(status == HPS_STRING_OK);
    assert_view(result, high.data, 7U);
}

static void test_view_export(void)
{
    char output[8] = "#######";
    const hps_string_view source = {"abcd", 4U};
    int32_t status = hps_string_view_copy_cstr(source, output, 5U);
    assert(status == HPS_STRING_OK);
    assert(memcmp(output, "abcd\0##", sizeof(output)) == 0);
    status = hps_string_view_copy_cstr(source, output, 4U);
    assert(status == HPS_STRING_NO_SPACE);
    assert(memcmp(output, "abcd\0##", sizeof(output)) == 0);
    status = hps_string_view_copy_cstr(source, output, 0U);
    assert(status == HPS_STRING_NO_SPACE);
    const hps_string_view embedded = {"a\0bc", 4U};
    status = hps_string_view_copy_cstr(embedded, output, sizeof(output));
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(output, "abcd\0##", sizeof(output)) == 0);
    status = hps_string_view_copy_cstr(embedded, output, 4U);
    assert(status == HPS_STRING_NO_SPACE);
    const hps_string_view empty = {NULL, 0U};
    status = hps_string_view_copy_cstr(empty, output, 1U);
    assert(status == HPS_STRING_OK);
    assert(memcmp(output, "\0bcd\0##", sizeof(output)) == 0);

    char overlap[10] = "abcdef###";
    const hps_string_view overlapping = {overlap + 1U, 5U};
    status = hps_string_view_copy_cstr(overlapping, overlap, sizeof(overlap));
    assert(status == HPS_STRING_OK);
    assert(memcmp(overlap, "bcdef\0###", sizeof(overlap)) == 0);
    const hps_string_view same = {overlap, 5U};
    status = hps_string_view_copy_cstr(same, overlap + 1U, 9U);
    assert(status == HPS_STRING_OK);
    assert(memcmp(overlap, "bbcdef\0##", sizeof(overlap)) == 0);
}

static void test_view_argument_failures(void)
{
    const hps_string_view good = {"a", 1U};
    const hps_string_view malformed = {NULL, SIZE_MAX};
    const hps_string_view maximum = {"a", SIZE_MAX};
    hps_string_view result = good;
    uint8_t flag = 97U;
    int32_t order = INT32_C(97);
    size_t index = 97U;
    char output[3] = "##";
    int32_t status = hps_string_view_init(NULL, good.data, good.length);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_init_cstr(NULL, "a", 2U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_slice(good, 0U, 1U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_slice(malformed, 0U, 0U, &result);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_view(result, good.data, good.length);
    status = hps_string_view_equal(good, malformed, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_equal(malformed, good, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_equal(good, good, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_compare(good, malformed, &order);
    assert(status == HPS_STRING_INVALID_ARGUMENT && order == 97);
    status = hps_string_view_compare(malformed, good, &order);
    assert(status == HPS_STRING_INVALID_ARGUMENT && order == 97);
    status = hps_string_view_compare(good, good, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_starts_with(malformed, good, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_starts_with(good, malformed, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_starts_with(good, good, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_ends_with(malformed, good, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_ends_with(good, malformed, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_ends_with(good, good, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_find(malformed, good, 0U, &index);
    assert(status == HPS_STRING_INVALID_ARGUMENT && index == 97U);
    status = hps_string_view_find(good, malformed, 0U, &index);
    assert(status == HPS_STRING_INVALID_ARGUMENT && index == 97U);
    status = hps_string_view_find(good, good, 0U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_rfind(malformed, good, 0U, &index);
    assert(status == HPS_STRING_INVALID_ARGUMENT && index == 97U);
    status = hps_string_view_rfind(good, malformed, 0U, &index);
    assert(status == HPS_STRING_INVALID_ARGUMENT && index == 97U);
    status = hps_string_view_rfind(good, good, 0U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_contains(malformed, good, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_contains(good, malformed, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_contains(good, good, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_trim(malformed, good, &result);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_trim(good, malformed, &result);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_trim(good, good, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_trim_whitespace(malformed, &result);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_view(result, good.data, good.length);
    status = hps_string_view_trim_whitespace(good, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_copy_cstr(malformed, output, sizeof(output));
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(output, "##", sizeof(output)) == 0);
    status = hps_string_view_copy_cstr(good, NULL, 2U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    /* Maximum length is a reserved search sentinel, never a readable extent. */
    status = hps_string_view_slice(maximum, 0U, 0U, &result);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_view_equal(maximum, good, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 97U);
    status = hps_string_view_compare(maximum, good, &order);
    assert(status == HPS_STRING_INVALID_ARGUMENT && order == 97);
    status = hps_string_view_find(maximum, good, 0U, &index);
    assert(status == HPS_STRING_INVALID_ARGUMENT && index == 97U);
    status = hps_string_view_rfind(good, maximum, 0U, &index);
    assert(status == HPS_STRING_INVALID_ARGUMENT && index == 97U);
    status = hps_string_view_trim_whitespace(maximum, &result);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_view(result, good.data, good.length);
    status = hps_string_view_copy_cstr(maximum, output, sizeof(output));
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(output, "##", sizeof(output)) == 0);
}

static void test_buffer_construction(void)
{
    char storage[8] = "abcdef#";
    hps_string_buffer buffer = {storage, 6U, sizeof(storage)};
    int32_t status = hps_string_buffer_init(&buffer, storage, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_buffer(&buffer, storage, 6U, sizeof(storage));
    assert(memcmp(storage, "abcdef#", sizeof(storage)) == 0);
    status = hps_string_buffer_init(&buffer, NULL, 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_init(NULL, storage, sizeof(storage));
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_init(&buffer, storage, sizeof(storage));
    assert(status == HPS_STRING_OK);
    assert_buffer(&buffer, storage, 0U, sizeof(storage));
    assert(memcmp(storage, "\0bcdef#", sizeof(storage)) == 0);

    char terminated[5] = "abcd";
    status = hps_string_buffer_init_cstr(&buffer, terminated, sizeof(terminated));
    assert(status == HPS_STRING_OK);
    assert_buffer(&buffer, terminated, 4U, sizeof(terminated));
    assert(memcmp(terminated, "abcd", sizeof(terminated)) == 0);
    status = hps_string_buffer_init_cstr(&buffer, storage + 1U, 6U);
    assert(status == HPS_STRING_NOT_TERMINATED);
    assert_buffer(&buffer, terminated, 4U, sizeof(terminated));
    char unterminated[4] = {'a', 'b', 'c', 'd'};
    status = hps_string_buffer_init_cstr(&buffer, unterminated, sizeof(unterminated));
    assert(status == HPS_STRING_NOT_TERMINATED);
    assert_buffer(&buffer, terminated, 4U, sizeof(terminated));
    assert(memcmp(unterminated, "abcd", sizeof(unterminated)) == 0);
    status = hps_string_buffer_init_cstr(&buffer, storage, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_init_cstr(&buffer, NULL, 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_init_cstr(NULL, storage, sizeof(storage));
    assert(status == HPS_STRING_INVALID_ARGUMENT);

    char tiny[1] = "";
    status = hps_string_buffer_init(&buffer, tiny, sizeof(tiny));
    assert(status == HPS_STRING_OK);
    const hps_string_view empty = {NULL, 0U};
    const hps_string_view one = {"a", 1U};
    status = hps_string_buffer_assign(&buffer, empty);
    assert(status == HPS_STRING_OK);
    status = hps_string_buffer_append(&buffer, empty);
    assert(status == HPS_STRING_OK);
    status = hps_string_buffer_append(&buffer, one);
    assert(status == HPS_STRING_NO_SPACE);
    status = hps_string_buffer_assign(&buffer, one);
    assert(status == HPS_STRING_NO_SPACE);
    assert_buffer(&buffer, tiny, 0U, 1U);
    assert(tiny[0] == '\0');
}

static void test_buffer_mutation_and_borrowing(void)
{
    char storage[8] = "#######";
    hps_string_buffer buffer = {NULL, 0U, 0U};
    int32_t status = hps_string_buffer_init(&buffer, storage, sizeof(storage));
    assert(status == HPS_STRING_OK);
    const hps_string_view abc = {"abc", 3U};
    status = hps_string_buffer_assign(&buffer, abc);
    assert(status == HPS_STRING_OK);
    assert_buffer(&buffer, storage, 3U, sizeof(storage));
    assert(memcmp(storage, "abc\0###", sizeof(storage)) == 0);
    size_t remaining = 97U;
    status = hps_string_buffer_remaining(&buffer, &remaining);
    assert(status == HPS_STRING_OK && remaining == 4U);
    hps_string_view borrowed = {NULL, 0U};
    status = hps_string_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_STRING_OK);
    assert_view(borrowed, storage, 3U);
    const char *cstr = NULL;
    status = hps_string_buffer_cstr(&buffer, &cstr);
    assert(status == HPS_STRING_OK && cstr == storage);

    const hps_string_view defg = {"defg", 4U};
    status = hps_string_buffer_append(&buffer, defg);
    assert(status == HPS_STRING_OK);
    assert_buffer(&buffer, storage, 7U, sizeof(storage));
    assert(memcmp(storage, "abcdefg", sizeof(storage)) == 0);
    status = hps_string_buffer_remaining(&buffer, &remaining);
    assert(status == HPS_STRING_OK && remaining == 0U);
    status = hps_string_buffer_append(&buffer, abc);
    assert(status == HPS_STRING_NO_SPACE);
    assert_buffer(&buffer, storage, 7U, sizeof(storage));
    assert(memcmp(storage, "abcdefg", sizeof(storage)) == 0);
    const hps_string_view oversized = {"abcdefgh", 8U};
    status = hps_string_buffer_assign(&buffer, oversized);
    assert(status == HPS_STRING_NO_SPACE);
    const hps_string_view malformed = {NULL, SIZE_MAX};
    status = hps_string_buffer_assign(&buffer, malformed);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_append(&buffer, malformed);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(storage, "abcdefg", sizeof(storage)) == 0);
    const hps_string_view embedded = {"a\0b", 3U};
    status = hps_string_buffer_assign(&buffer, embedded);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(storage, "abcdefg", sizeof(storage)) == 0);
    status = hps_string_buffer_append(&buffer, embedded);
    assert(status == HPS_STRING_NO_SPACE);
    assert(memcmp(storage, "abcdefg", sizeof(storage)) == 0);

    status = hps_string_buffer_erase(&buffer, 2U, 3U);
    assert(status == HPS_STRING_OK);
    assert_buffer(&buffer, storage, 4U, sizeof(storage));
    assert(memcmp(storage, "abfg\0fg", sizeof(storage)) == 0);
    status = hps_string_buffer_append(&buffer, embedded);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(storage, "abfg\0fg", sizeof(storage)) == 0);
    status = hps_string_buffer_erase(&buffer, 1U, 0U);
    assert(status == HPS_STRING_OK);
    assert(memcmp(storage, "abfg\0fg", sizeof(storage)) == 0);
    status = hps_string_buffer_erase(&buffer, SIZE_MAX, 0U);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    status = hps_string_buffer_erase(&buffer, 1U, SIZE_MAX);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    status = hps_string_buffer_erase(&buffer, 4U, 1U);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    assert(memcmp(storage, "abfg\0fg", sizeof(storage)) == 0);
    status = hps_string_buffer_erase(&buffer, 4U, 0U);
    assert(status == HPS_STRING_OK);
    status = hps_string_buffer_erase(&buffer, 2U, 2U);
    assert(status == HPS_STRING_OK);
    assert(memcmp(storage, "ab\0g\0fg", sizeof(storage)) == 0);
    status = hps_string_buffer_truncate(&buffer, SIZE_MAX);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    assert_buffer(&buffer, storage, 2U, sizeof(storage));
    status = hps_string_buffer_truncate(&buffer, 2U);
    assert(status == HPS_STRING_OK);
    status = hps_string_buffer_truncate(&buffer, 1U);
    assert(status == HPS_STRING_OK);
    assert(memcmp(storage, "a\0\0g\0fg", sizeof(storage)) == 0);
    status = hps_string_buffer_clear(&buffer);
    assert(status == HPS_STRING_OK);
    assert(memcmp(storage, "\0\0\0g\0fg", sizeof(storage)) == 0);
    status = hps_string_buffer_remaining(&buffer, &remaining);
    assert(status == HPS_STRING_OK && remaining == 7U);
    status = hps_string_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_STRING_OK);
    assert_view(borrowed, NULL, 0U);
    status = hps_string_buffer_cstr(&buffer, &cstr);
    assert(status == HPS_STRING_OK && cstr == storage && cstr[0] == '\0');
}

static void test_buffer_state_failures(void)
{
    char storage[5] = "abcd";
    const hps_string_view empty = {NULL, 0U};
    hps_string_buffer buffer = {storage, SIZE_MAX, sizeof(storage)};
    hps_string_view result = {"keep", 4U};
    const char *cstr = result.data;
    size_t remaining = 97U;
    int32_t status = hps_string_buffer_assign(&buffer, empty);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_string_buffer_append(&buffer, empty);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_string_buffer_erase(&buffer, 0U, 0U);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_string_buffer_truncate(&buffer, 0U);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_string_buffer_clear(&buffer);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_string_buffer_as_view(&buffer, &result);
    assert(status == HPS_STRING_INVALID_STATE);
    assert_view(result, cstr, 4U);
    status = hps_string_buffer_cstr(&buffer, &cstr);
    assert(status == HPS_STRING_INVALID_STATE);
    assert(cstr == result.data);
    status = hps_string_buffer_remaining(&buffer, &remaining);
    assert(status == HPS_STRING_INVALID_STATE && remaining == 97U);
    assert_buffer(&buffer, storage, SIZE_MAX, sizeof(storage));
    assert(memcmp(storage, "abcd", sizeof(storage)) == 0);

    buffer.length = 3U;
    status = hps_string_buffer_clear(&buffer);
    assert(status == HPS_STRING_INVALID_STATE);
    assert_buffer(&buffer, storage, 3U, sizeof(storage));
    assert(memcmp(storage, "abcd", sizeof(storage)) == 0);
    storage[1] = '\0';
    buffer.length = 4U;
    status = hps_string_buffer_assign(&buffer, empty);
    assert(status == HPS_STRING_INVALID_STATE);
    assert(memcmp(storage, "a\0cd", sizeof(storage)) == 0);
    buffer.data = NULL;
    buffer.length = 0U;
    status = hps_string_buffer_clear(&buffer);
    assert(status == HPS_STRING_INVALID_STATE);
    buffer.data = storage;
    buffer.capacity = 0U;
    status = hps_string_buffer_clear(&buffer);
    assert(status == HPS_STRING_INVALID_STATE);
    assert(memcmp(storage, "a\0cd", sizeof(storage)) == 0);
}

static void test_buffer_argument_failures(void)
{
    char storage[2] = "a";
    hps_string_buffer buffer = {storage, 1U, sizeof(storage)};
    const hps_string_view empty = {NULL, 0U};
    hps_string_view result = {"keep", 4U};
    const char *cstr = result.data;
    size_t remaining = 97U;
    int32_t status = hps_string_buffer_assign(NULL, empty);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_append(NULL, empty);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_erase(NULL, 0U, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_truncate(NULL, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_clear(NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_as_view(NULL, &result);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_view(result, cstr, 4U);
    status = hps_string_buffer_as_view(&buffer, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_cstr(NULL, &cstr);
    assert(status == HPS_STRING_INVALID_ARGUMENT && cstr == result.data);
    status = hps_string_buffer_cstr(&buffer, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_string_buffer_remaining(NULL, &remaining);
    assert(status == HPS_STRING_INVALID_ARGUMENT && remaining == 97U);
    status = hps_string_buffer_remaining(&buffer, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_buffer(&buffer, storage, 1U, sizeof(storage));
    assert(memcmp(storage, "a", sizeof(storage)) == 0);
}

static void test_buffer_overlap_model(void)
{
    const char *initial = "abcdef";
    const size_t capacities[3] = {7U, 10U, 13U};
    size_t capacity_index;
    size_t offset;
    size_t count;
    uint8_t append;
    size_t i;

    /* Independent immutable source snapshots define complete byte expectations.
     * Every substring of the initial text is assigned/appended with three bounds,
     * including self-copy, empty, exact fit and failure with no partial writes.
     */
    for (capacity_index = 0U; capacity_index < 3U; ++capacity_index) {
        for (offset = 0U; offset <= 6U; ++offset) {
            for (count = 0U; count <= 6U - offset; ++count) {
                for (append = 0U; append < 2U; ++append) {
                    char storage[20];
                    char expected[20];
                    for (i = 0U; i < sizeof(storage); ++i) {
                        storage[i] = '#';
                        expected[i] = '#';
                    }
                    for (i = 0U; i < 7U; ++i) {
                        storage[i] = initial[i];
                        expected[i] = initial[i];
                    }
                    const size_t capacity = capacities[capacity_index];
                    hps_string_buffer buffer = {storage, 6U, capacity};
                    const hps_string_view source = {storage + offset, count};
                    const size_t start = (append == 1U) ? 6U : 0U;
                    const size_t result_length = start + count;
                    int32_t status;
                    if (append == 1U) {
                        status = hps_string_buffer_append(&buffer, source);
                    } else {
                        status = hps_string_buffer_assign(&buffer, source);
                    }
                    if (result_length < capacity) {
                        for (i = 0U; i < count; ++i) {
                            expected[start + i] = initial[offset + i];
                        }
                        expected[result_length] = '\0';
                        assert(status == HPS_STRING_OK);
                        assert_buffer(&buffer, storage, result_length, capacity);
                    } else {
                        assert(status == HPS_STRING_NO_SPACE);
                        assert_buffer(&buffer, storage, 6U, capacity);
                    }
                    assert(memcmp(storage, expected, sizeof(storage)) == 0);
                }
            }
        }
    }
}

int32_t test_string(void)
{
    test_result_representation();
    test_view_construction_and_slicing();
    test_view_comparison();
    test_view_searches();
    test_view_trimming();
    test_view_export();
    test_view_argument_failures();
    test_buffer_construction();
    test_buffer_mutation_and_borrowing();
    test_buffer_state_failures();
    test_buffer_argument_failures();
    test_buffer_overlap_model();
    return INT32_C(0);
}
