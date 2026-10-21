#include "../test_suites.h"
#include "hps/cstr/hps_core.h"
#include <assert.h>
#include <string.h>

/**
 * @file tests/cstr/test_cstr.c
 * @brief Independent bounded C-string transformation and failure checks.
 */

static void assert_text(const hps_string_buffer *buffer, const char *text,
                         size_t length)
{
    assert(buffer->length == length);
    assert(memcmp(buffer->data, text, length + 1U) == 0);
}

static void bind_text(hps_string_buffer *buffer, char *storage, size_t capacity,
                       const char *text, size_t length)
{
    int32_t status = hps_string_buffer_init(buffer, storage, capacity);
    assert(status == HPS_STRING_OK);
    const hps_string_view source = {text, length};
    status = hps_string_buffer_assign(buffer, source);
    assert(status == HPS_STRING_OK);
}

static void test_inspection(void)
{
    const char unterminated[4] = {'a', 'b', 'c', 'd'};
    char storage[8] = "abc";
    size_t length = 97U;
    uint8_t flag = 97U;
    int32_t status = hps_cstr_length(storage, sizeof(storage), &length);
    assert(status == HPS_STRING_OK && length == 3U);
    status = hps_cstr_length(unterminated, sizeof(unterminated), &length);
    assert(status == HPS_STRING_NOT_TERMINATED && length == 3U);
    status = hps_cstr_length(NULL, 1U, &length);
    assert(status == HPS_STRING_INVALID_ARGUMENT && length == 3U);
    status = hps_cstr_length(storage, 0U, &length);
    assert(status == HPS_STRING_INVALID_ARGUMENT && length == 3U);
    status = hps_cstr_length(storage, sizeof(storage), NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_is_terminated(unterminated, sizeof(unterminated), &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_cstr_is_terminated(storage, sizeof(storage), &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_cstr_is_terminated(NULL, 1U, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 1U);
    status = hps_cstr_is_terminated(storage, sizeof(storage), NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_is_empty(storage, sizeof(storage), &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_cstr_is_empty("", 1U, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_cstr_is_empty(unterminated, sizeof(unterminated), &flag);
    assert(status == HPS_STRING_NOT_TERMINATED && flag == 1U);
    status = hps_cstr_is_empty(storage, sizeof(storage), NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_has_capacity_for(storage, sizeof(storage), 4U, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_cstr_has_capacity_for(storage, sizeof(storage), 5U, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_cstr_has_capacity_for(storage, sizeof(storage), SIZE_MAX, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_cstr_has_capacity_for(NULL, 1U, 0U, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 0U);
    status = hps_cstr_has_capacity_for(storage, sizeof(storage), 1U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_copy_length(storage, sizeof(storage), 0U, &length);
    assert(status == HPS_STRING_OK && length == 0U);
    status = hps_cstr_copy_length(storage, sizeof(storage), 1U, &length);
    assert(status == HPS_STRING_OK && length == 0U);
    status = hps_cstr_copy_length(storage, sizeof(storage), 3U, &length);
    assert(status == HPS_STRING_OK && length == 2U);
    status = hps_cstr_copy_length(storage, sizeof(storage), SIZE_MAX, &length);
    assert(status == HPS_STRING_OK && length == 3U);
    status = hps_cstr_copy_length(unterminated, sizeof(unterminated), 2U, &length);
    assert(status == HPS_STRING_NOT_TERMINATED && length == 3U);
    status = hps_cstr_copy_length(storage, sizeof(storage), 2U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);

    const hps_string_view allowed = {"123", 3U};
    status = hps_cstr_consists_of("121", 4U, allowed, &flag);
    assert(status == HPS_STRING_OK && flag == 1U);
    status = hps_cstr_consists_of("12a", 4U, allowed, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    status = hps_cstr_consists_of("", 1U, allowed, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    const hps_string_view empty = {NULL, 0U};
    status = hps_cstr_consists_of("1", 2U, empty, &flag);
    assert(status == HPS_STRING_OK && flag == 0U);
    const hps_string_view malformed = {NULL, 1U};
    status = hps_cstr_consists_of("1", 2U, malformed, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 0U);
    status = hps_cstr_consists_of("1", 2U, allowed, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
}

static void test_clear_and_compare(void)
{
    char storage[8] = "abc#def";
    storage[3] = '\0';
    hps_string_buffer buffer = {storage, 3U, sizeof(storage)};
    int32_t status = hps_cstr_clear_tail(&buffer);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "abc", 3U);
    assert(memcmp(storage + 3U, "\0\0\0\0", 5U) == 0);
    status = hps_cstr_clear(&buffer);
    assert(status == HPS_STRING_OK && buffer.length == 0U);
    assert(memcmp(storage, "\0\0\0\0\0\0\0", sizeof(storage)) == 0);
    status = hps_cstr_clear(NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_clear_tail(NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);

    int32_t order = INT32_C(97);
    status = hps_cstr_compare("ABc", 4U, "aBC", 4U, HPS_STRING_NPOS, 1U, &order);
    assert(status == HPS_STRING_OK && order == 0);
    status = hps_cstr_compare("abx", 4U, "aby", 4U, 2U, 0U, &order);
    assert(status == HPS_STRING_OK && order == 0);
    status = hps_cstr_compare("abx", 4U, "aby", 4U, 3U, 0U, &order);
    assert(status == HPS_STRING_OK && order == -1);
    status = hps_cstr_compare("abc", 4U, "ab", 3U, HPS_STRING_NPOS, 1U, &order);
    assert(status == HPS_STRING_OK && order == 1);
    status = hps_cstr_compare("\x80", 2U, "\x7f", 2U, HPS_STRING_NPOS, 1U, &order);
    assert(status == HPS_STRING_OK && order == 1);
    status = hps_cstr_compare("a", 2U, "b", 2U, 0U, 0U, &order);
    assert(status == HPS_STRING_OK && order == 0);
    status = hps_cstr_compare(NULL, 1U, "b", 2U, 0U, 0U, &order);
    assert(status == HPS_STRING_INVALID_ARGUMENT && order == 0);
    status = hps_cstr_compare("ab", 2U, "b", 2U, 0U, 0U, &order);
    assert(status == HPS_STRING_NOT_TERMINATED && order == 0);
    status = hps_cstr_compare("a", 2U, "b", 2U, 0U, 2U, &order);
    assert(status == HPS_STRING_INVALID_ARGUMENT && order == 0);
    status = hps_cstr_compare("a", 2U, "b", 2U, 0U, 0U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
}

static void test_copy_and_search(void)
{
    char storage[16] = "###############";
    hps_string_buffer buffer = {NULL, 0U, 0U};
    bind_text(&buffer, storage, sizeof(storage), "keep", 4U);
    int32_t status = hps_cstr_copy_range(&buffer, "aZ9\x80", 5U,
                                                   0U, HPS_STRING_NPOS,
                                                   HPS_CSTR_LOWER_CASE);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "az9\x80", 4U);
    status = hps_cstr_copy_range(&buffer, storage, sizeof(storage), 0U,
                                  HPS_STRING_NPOS, HPS_CSTR_UPPER_CASE);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "AZ9\x80", 4U);
    status = hps_cstr_copy_range(&buffer, storage, sizeof(storage), 1U, 2U,
                                  HPS_CSTR_KEEP_CASE);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "Z9", 2U);
    char snapshot[16];
    (void)memcpy(snapshot, storage, sizeof(storage));
    status = hps_cstr_copy_range(&buffer, "abc", 4U, 0U, 4U, HPS_CSTR_KEEP_CASE);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    status = hps_cstr_copy_range(&buffer, "abc", 4U, SIZE_MAX,
                                  HPS_STRING_NPOS, HPS_CSTR_KEEP_CASE);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    status = hps_cstr_copy_range(&buffer, "abcdefghijklmnop", 17U,
                                  0U, HPS_STRING_NPOS, HPS_CSTR_KEEP_CASE);
    assert(status == HPS_STRING_NO_SPACE);
    status = hps_cstr_copy_range(&buffer, "abc", 3U, 0U, 1U, HPS_CSTR_KEEP_CASE);
    assert(status == HPS_STRING_NOT_TERMINATED);
    status = hps_cstr_copy_range(&buffer, "abc", 4U, 0U, 1U, 3U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_text(&buffer, "Z9", 2U);
    assert(memcmp(storage, snapshot, sizeof(storage)) == 0);
    status = hps_cstr_copy_range(&buffer, "abc", 4U, 3U, 0U, HPS_CSTR_KEEP_CASE);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "", 0U);
    status = hps_cstr_copy_range(NULL, "abc", 4U, 0U, 1U, HPS_CSTR_KEEP_CASE);
    assert(status == HPS_STRING_INVALID_ARGUMENT);

    size_t found = 97U;
    status = hps_cstr_find("abcabc", 7U, "abc", 4U, 1U, &found);
    assert(status == HPS_STRING_OK && found == 3U);
    status = hps_cstr_rfind("abcabc", 7U, "abc", 4U, 1U, &found);
    assert(status == HPS_STRING_OK && found == 0U);
    status = hps_cstr_rfind("abcabc", 7U, "abc", 4U, HPS_STRING_NPOS, &found);
    assert(status == HPS_STRING_OK && found == 3U);
    status = hps_cstr_find("abcabc", 7U, "", 1U, 6U, &found);
    assert(status == HPS_STRING_OK && found == 6U);
    status = hps_cstr_find("abcabc", 7U, "abc", 4U, SIZE_MAX, &found);
    assert(status == HPS_STRING_OUT_OF_RANGE && found == 6U);
    status = hps_cstr_find("abcabc", 7U, "z", 2U, 0U, &found);
    assert(status == HPS_STRING_OK && found == HPS_STRING_NPOS);
    status = hps_cstr_find("abcabc", 7U, "z", 1U, 0U, &found);
    assert(status == HPS_STRING_NOT_TERMINATED && found == HPS_STRING_NPOS);
    const hps_string_view character = {"b", 1U};
    status = hps_cstr_find_char("abcabc", 7U, character, 2U, &found);
    assert(status == HPS_STRING_OK && found == 4U);
    status = hps_cstr_rfind_char("abcabc", 7U, character, 2U, &found);
    assert(status == HPS_STRING_OK && found == 1U);
    const hps_string_view nul = {"", 1U};
    status = hps_cstr_rfind_char("abcabc", 7U, nul, HPS_STRING_NPOS, &found);
    assert(status == HPS_STRING_OK && found == HPS_STRING_NPOS);
    const hps_string_view empty = {NULL, 0U};
    status = hps_cstr_find_char("abcabc", 7U, empty, 0U, &found);
    assert(status == HPS_STRING_INVALID_ARGUMENT && found == HPS_STRING_NPOS);
    status = hps_cstr_find("a", 2U, "a", 2U, 0U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_rfind("a", 2U, "a", 2U, 0U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_find_char("a", 2U, character, 0U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_rfind_char("a", 2U, character, 0U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
}

static void test_reverse_replace_filter(void)
{
    char storage[16] = "###############";
    hps_string_buffer buffer = {NULL, 0U, 0U};
    bind_text(&buffer, storage, sizeof(storage), "abcdef", 6U);
    int32_t status = hps_cstr_reverse(&buffer, 1U, 4U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "aedcbf", 6U);
    status = hps_cstr_reverse(&buffer, 0U, HPS_STRING_NPOS);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "fbcdea", 6U);
    status = hps_cstr_reverse(&buffer, 6U, 0U);
    assert(status == HPS_STRING_OK);
    status = hps_cstr_reverse(&buffer, 0U, 7U);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    status = hps_cstr_reverse(&buffer, SIZE_MAX, 0U);
    assert(status == HPS_STRING_OUT_OF_RANGE);
    assert_text(&buffer, "fbcdea", 6U);
    bind_text(&buffer, storage, sizeof(storage), "ababa", 5U);
    const hps_string_view old_character = {storage, 1U};
    const hps_string_view new_character = {storage + 1U, 1U};
    status = hps_cstr_replace_char(&buffer, old_character, new_character);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "bbbbb", 5U);
    const hps_string_view nul = {"", 1U};
    status = hps_cstr_replace_char(&buffer, new_character, nul);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_text(&buffer, "bbbbb", 5U);
    const hps_string_view empty = {NULL, 0U};
    status = hps_cstr_replace_char(&buffer, empty, new_character);
    assert(status == HPS_STRING_INVALID_ARGUMENT);

    bind_text(&buffer, storage, sizeof(storage), "a1-b2 c3", 8U);
    const hps_string_view remove_set = {"- ", 2U};
    status = hps_cstr_filter(&buffer, remove_set, HPS_CSTR_REMOVE_SET);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "a1b2c3", 6U);
    status = hps_cstr_filter(&buffer, empty, HPS_CSTR_KEEP_DIGITS);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "123", 3U);
    status = hps_cstr_filter(&buffer, empty, HPS_CSTR_REMOVE_DIGITS);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "", 0U);
    bind_text(&buffer, storage, sizeof(storage), "a1\x80" "2b", 5U);
    status = hps_cstr_filter(&buffer, empty, HPS_CSTR_REMOVE_DIGITS);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "a\x80" "b", 3U);
    char snapshot[16];
    (void)memcpy(snapshot, storage, sizeof(storage));
    status = hps_cstr_filter(&buffer, remove_set, HPS_CSTR_KEEP_DIGITS);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_filter(&buffer, empty, 3U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(storage, snapshot, sizeof(storage)) == 0);
    const hps_string_view malformed = {NULL, 1U};
    status = hps_cstr_filter(&buffer, malformed, HPS_CSTR_REMOVE_SET);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_text(&buffer, "a\x80" "b", 3U);
}

static void test_remove_and_trim(void)
{
    char storage[32] = "###############################";
    hps_string_buffer buffer = {NULL, 0U, 0U};
    bind_text(&buffer, storage, sizeof(storage), "ababaxaba", 9U);
    int32_t status = hps_cstr_remove_substring(&buffer, "aba", 4U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "bax", 3U);
    status = hps_cstr_remove_substring(&buffer, "", 1U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "bax", 3U);
    status = hps_cstr_remove_substring(&buffer, "bax", 4U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "", 0U);
    bind_text(&buffer, storage, sizeof(storage), "---abc___", 9U);
    const hps_string_view left = {"-", 1U};
    const hps_string_view right = {"_", 1U};
    status = hps_cstr_trim(&buffer, left, right);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "abc", 3U);
    bind_text(&buffer, storage, sizeof(storage), "xyxyabczzzz", 11U);
    const hps_string_view left_pattern = {storage, 2U};
    const hps_string_view right_pattern = {storage + 7U, 2U};
    status = hps_cstr_trim_substrings(&buffer, left_pattern, right_pattern);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "abc", 3U);
    const hps_string_view empty = {NULL, 0U};
    status = hps_cstr_trim_substrings(&buffer, empty, empty);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "abc", 3U);
    bind_text(&buffer, storage, sizeof(storage), "xyxy", 4U);
    const hps_string_view xy = {"xy", 2U};
    status = hps_cstr_trim_substrings(&buffer, xy, xy);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "", 0U);
    status = hps_cstr_trim(&buffer, empty, empty);
    assert(status == HPS_STRING_OK);
    const hps_string_view malformed = {NULL, 1U};
    status = hps_cstr_trim(&buffer, left, malformed);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_trim_substrings(&buffer, malformed, empty);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_remove_substring(&buffer, "a", 1U);
    assert(status == HPS_STRING_NOT_TERMINATED);
    assert_text(&buffer, "", 0U);
}

static void test_padding(void)
{
    char storage[16] = "###############";
    hps_string_buffer buffer = {NULL, 0U, 0U};
    bind_text(&buffer, storage, sizeof(storage), "abc", 3U);
    const hps_string_view left = {"xy", 2U};
    const hps_string_view right = {"!", 1U};
    const hps_string_view empty = {NULL, 0U};
    int32_t status = hps_cstr_pad(&buffer, left, 2U, right, 1U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "xyxyabc!", 8U);
    char snapshot[16];
    (void)memcpy(snapshot, storage, sizeof(storage));
    status = hps_cstr_pad(&buffer, left, SIZE_MAX, right, 1U);
    assert(status == HPS_STRING_NO_SPACE);
    status = hps_cstr_pad(&buffer, left, 3U, right, 2U);
    assert(status == HPS_STRING_NO_SPACE);
    assert(memcmp(storage, snapshot, sizeof(storage)) == 0);
    const hps_string_view embedded = {"a\0b", 3U};
    status = hps_cstr_pad(&buffer, empty, 0U, embedded, 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(storage, snapshot, sizeof(storage)) == 0);
    status = hps_cstr_pad(&buffer, empty, SIZE_MAX, empty, SIZE_MAX);
    assert(status == HPS_STRING_OK);
    assert(memcmp(storage, snapshot, sizeof(storage)) == 0);
    status = hps_cstr_repeat(&buffer, left, 7U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "xyxyxyxyxyxyxy", 14U);
    status = hps_cstr_repeat(&buffer, left, 8U);
    assert(status == HPS_STRING_NO_SPACE);
    assert_text(&buffer, "xyxyxyxyxyxyxy", 14U);
    status = hps_cstr_repeat(&buffer, right, 15U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "!!!!!!!!!!!!!!!", 15U);
    status = hps_cstr_repeat(&buffer, empty, SIZE_MAX);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "", 0U);
    status = hps_cstr_repeat(&buffer, embedded, 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_text(&buffer, "", 0U);
    bind_text(&buffer, storage, sizeof(storage), "abc", 3U);
    status = hps_cstr_pad_to(&buffer, left, 8U, 1U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "xyxyxabc", 8U);
    status = hps_cstr_pad_to(&buffer, right, 10U, 0U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "xyxyxabc!!", 10U);
    status = hps_cstr_pad_to(&buffer, empty, 8U, 0U);
    assert(status == HPS_STRING_OK);
    assert_text(&buffer, "xyxyxabc!!", 10U);
    status = hps_cstr_pad_to(&buffer, empty, 11U, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_pad_to(&buffer, left, SIZE_MAX, 0U);
    assert(status == HPS_STRING_NO_SPACE);
    status = hps_cstr_pad_to(&buffer, left, 10U, 2U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert_text(&buffer, "xyxyxabc!!", 10U);
}

static void test_mutation_failures(void)
{
    char storage[8] = "abc####";
    storage[3] = '\0';
    char snapshot[8];
    (void)memcpy(snapshot, storage, sizeof(storage));
    hps_string_buffer buffer = {storage, SIZE_MAX, sizeof(storage)};
    const hps_string_view empty = {NULL, 0U};
    const hps_string_view one = {"a", 1U};
    int32_t status = hps_cstr_reverse(&buffer, 0U, 1U);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_replace_char(&buffer, one, one);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_filter(&buffer, empty, HPS_CSTR_REMOVE_SET);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_remove_substring(&buffer, "a", 2U);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_trim(&buffer, empty, empty);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_trim_substrings(&buffer, empty, empty);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_pad(&buffer, empty, 0U, empty, 0U);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_repeat(&buffer, empty, 0U);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_pad_to(&buffer, empty, 0U, 0U);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_copy_range(&buffer, "a", 2U, 0U, 1U, HPS_CSTR_KEEP_CASE);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_clear(&buffer);
    assert(status == HPS_STRING_INVALID_STATE);
    status = hps_cstr_clear_tail(&buffer);
    assert(status == HPS_STRING_INVALID_STATE);
    assert(buffer.length == SIZE_MAX);
    assert(memcmp(storage, snapshot, sizeof(storage)) == 0);
    status = hps_cstr_reverse(NULL, 0U, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_replace_char(NULL, one, one);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_filter(NULL, empty, HPS_CSTR_REMOVE_SET);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_remove_substring(NULL, "a", 2U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_trim(NULL, empty, empty);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_trim_substrings(NULL, empty, empty);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_pad(NULL, empty, 0U, empty, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_repeat(NULL, empty, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_pad_to(NULL, empty, 0U, 0U);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
}

int32_t test_cstr(void)
{
    test_inspection();
    test_clear_and_compare();
    test_copy_and_search();
    test_reverse_replace_filter();
    test_remove_and_trim();
    test_padding();
    test_mutation_failures();
    return INT32_C(0);
}
