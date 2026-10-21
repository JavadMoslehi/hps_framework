#include "../test_suites.h"
#include "hps/string/hps_tokenizer.h"

#include <assert.h>
#include <string.h>

/**
 * @file tests/string/test_string_tokenizer.c
 * @brief Token syntax, literal retention, progress and transactional parse errors.
 */

static void expect_token(const hps_string_view input,
                         const hps_string_view delimiters, size_t *cursor,
                         const uint8_t flags, const char *expected,
                         const size_t expected_length)
{
    hps_string_view part = {NULL, 0U};
    uint8_t found = 0U;
    assert(hps_string_tokenizer_next(input, delimiters, cursor, flags, &part, &found) == HPS_STRING_OK);
    assert(found == 1U && part.length == expected_length);
    if (expected_length == 0U) { assert(part.data == NULL); }
    else { assert(memcmp(part.data, expected, expected_length) == 0); }
}

static void test_literals_and_progress(void)
{
    const hps_string_view input = {",; alpha ;\"b,c\";d\\,e;'f;g'; tail ,;", sizeof(",; alpha ;\"b,c\";d\\,e;'f;g'; tail ,;") - 1U};
    const hps_string_view delimiters = {",;", 2U};
    const uint8_t flags = (uint8_t)(HPS_STRING_TOKEN_TRIM | HPS_STRING_TOKEN_QUOTES | HPS_STRING_TOKEN_ESCAPES);
    size_t cursor = 0U;
    expect_token(input, delimiters, &cursor, flags, "alpha", 5U);
    expect_token(input, delimiters, &cursor, flags, "\"b,c\"", 5U);
    expect_token(input, delimiters, &cursor, flags, "d\\,e", 4U);
    expect_token(input, delimiters, &cursor, flags, "'f;g'", 5U);
    expect_token(input, delimiters, &cursor, flags, "tail", 4U);
    assert(cursor == HPS_STRING_NPOS);
    hps_string_view part = {"keep", 4U};
    uint8_t found = UINT8_MAX;
    assert(hps_string_tokenizer_next(input, delimiters, &cursor, flags, &part, &found) == HPS_STRING_OK);
    assert(found == 0U && part.length == 4U && part.data[0] == 'k');

    const hps_string_view mixed = {"'a\"b',\"c'd\",\"a\\\"b,c\"", sizeof("'a\"b',\"c'd\",\"a\\\"b,c\"") - 1U};
    cursor = 0U;
    expect_token(mixed, delimiters, &cursor, flags, "'a\"b'", 5U);
    expect_token(mixed, delimiters, &cursor, flags, "\"c'd\"", 5U);
    expect_token(mixed, delimiters, &cursor, flags, "\"a\\\"b,c\"", sizeof("\"a\\\"b,c\"") - 1U);
    assert(cursor == HPS_STRING_NPOS);
}

static void test_empty_and_counted_inputs(void)
{
    const hps_string_view none = {NULL, 0U};
    const hps_string_view delimiters = {",;", 2U};
    hps_string_view part = {"keep", 4U};
    uint8_t found = UINT8_MAX;
    size_t cursor = 0U;
    assert(hps_string_tokenizer_next(none, delimiters, &cursor, 0U, &part, &found) == HPS_STRING_OK);
    assert(cursor == HPS_STRING_NPOS && found == 0U && part.length == 4U);
    const hps_string_view separators = {",;;,", 4U};
    cursor = 0U;
    found = UINT8_MAX;
    assert(hps_string_tokenizer_next(separators, delimiters, &cursor, 0U, &part, &found) == HPS_STRING_OK);
    assert(cursor == HPS_STRING_NPOS && found == 0U && part.length == 4U);

    const hps_string_view text = {" a,b ", 5U};
    cursor = 0U;
    expect_token(text, none, &cursor, HPS_STRING_TOKEN_TRIM, "a,b", 3U);
    assert(cursor == HPS_STRING_NPOS);
    cursor = text.length;
    assert(hps_string_tokenizer_next(text, none, &cursor, 0U, &part, &found) == HPS_STRING_OK);
    assert(cursor == HPS_STRING_NPOS && found == 0U);

    const hps_string_view whitespace = {" \t\r\n\v\f", 6U};
    cursor = 0U;
    expect_token(whitespace, none, &cursor, HPS_STRING_TOKEN_TRIM, "", 0U);
    const hps_string_view binary_text = {"a\0b,c", 5U};
    cursor = 0U;
    expect_token(binary_text, delimiters, &cursor, 0U, "a\0b", 3U);
    expect_token(binary_text, delimiters, &cursor, 0U, "c", 1U);
    const hps_string_view nul_separator = {"\0", 1U};
    cursor = 0U;
    expect_token(binary_text, nul_separator, &cursor, 0U, "a", 1U);
    expect_token(binary_text, nul_separator, &cursor, 0U, "b,c", 3U);

    const hps_string_view basic = {"abc", 3U};
    const hps_string_view set = {"XY", 2U};
    cursor = 0U;
    expect_token(basic, set, &cursor, 0U, "abc", 3U);
}

static void test_syntax_precedes_delimiter_membership(void)
{
    const hps_string_view quote_input = {"'a,b'", 5U};
    const hps_string_view quote_set = {",'", 2U};
    size_t cursor = 0U;
    expect_token(quote_input, quote_set, &cursor, HPS_STRING_TOKEN_QUOTES, "'a,b'", 5U);
    assert(cursor == HPS_STRING_NPOS);
    cursor = 0U;
    expect_token(quote_input, quote_set, &cursor, 0U, "a", 1U);
    expect_token(quote_input, quote_set, &cursor, 0U, "b", 1U);
    assert(cursor == HPS_STRING_NPOS);

    const hps_string_view following_quote = {"a,'b,c'", 7U};
    cursor = 0U;
    expect_token(following_quote, quote_set, &cursor, HPS_STRING_TOKEN_QUOTES, "a", 1U);
    assert(cursor == 2U);
    expect_token(following_quote, quote_set, &cursor, HPS_STRING_TOKEN_QUOTES, "'b,c'", 5U);
    assert(cursor == HPS_STRING_NPOS);

    const hps_string_view escape_input = {"\\,x,\\,y", 7U};
    const hps_string_view escape_set = {",\\", 2U};
    cursor = 0U;
    expect_token(escape_input, escape_set, &cursor, HPS_STRING_TOKEN_ESCAPES, "\\,x", 3U);
    assert(cursor == 4U);
    expect_token(escape_input, escape_set, &cursor, HPS_STRING_TOKEN_ESCAPES, "\\,y", 3U);
    assert(cursor == HPS_STRING_NPOS);
    cursor = 0U;
    expect_token(escape_input, escape_set, &cursor, 0U, "x", 1U);
    expect_token(escape_input, escape_set, &cursor, 0U, "y", 1U);
    assert(cursor == HPS_STRING_NPOS);

    const hps_string_view combined = {",\"a,b\",\\,c", sizeof(",\"a,b\",\\,c") - 1U};
    const hps_string_view combined_set = {",'\"\\", 4U};
    const uint8_t flags = (uint8_t)(HPS_STRING_TOKEN_QUOTES | HPS_STRING_TOKEN_ESCAPES);
    cursor = 0U;
    expect_token(combined, combined_set, &cursor, flags, "\"a,b\"", 5U);
    expect_token(combined, combined_set, &cursor, flags, "\\,c", 3U);
    assert(cursor == HPS_STRING_NPOS);
}

static void expect_error(const hps_string_view input,
                         const hps_string_view delimiters,
                         const size_t initial_cursor, const uint8_t flags,
                         const int32_t expected)
{
    size_t cursor = initial_cursor;
    hps_string_view part = {"keep", 4U};
    const char *original = part.data;
    uint8_t found = UINT8_MAX;
    assert(hps_string_tokenizer_next(input, delimiters, &cursor, flags, &part, &found) == expected);
    assert(cursor == initial_cursor && part.data == original && part.length == 4U && found == UINT8_MAX);
}

static void test_errors_and_disabled_features(void)
{
    const hps_string_view delimiters = {",", 1U};
    const hps_string_view bad_quote = {"ok,'bad", 7U};
    size_t cursor = 0U;
    expect_token(bad_quote, delimiters, &cursor, HPS_STRING_TOKEN_QUOTES, "ok", 2U);
    assert(cursor == 3U);
    expect_error(bad_quote, delimiters, cursor, HPS_STRING_TOKEN_QUOTES, HPS_STRING_INVALID_ARGUMENT);
    const hps_string_view bad_escape = {"bad\\", 4U};
    expect_error(bad_escape, delimiters, 0U, HPS_STRING_TOKEN_ESCAPES, HPS_STRING_INVALID_ARGUMENT);
    cursor = 0U;
    expect_token(bad_escape, delimiters, &cursor, 0U, "bad\\", 4U);
    cursor = 3U;
    expect_token(bad_quote, delimiters, &cursor, 0U, "'bad", 4U);

    const hps_string_view malformed = {NULL, 1U};
    const hps_string_view oversized = {"", HPS_STRING_NPOS};
    expect_error(malformed, delimiters, 0U, 0U, HPS_STRING_INVALID_ARGUMENT);
    expect_error(bad_quote, malformed, 0U, 0U, HPS_STRING_INVALID_ARGUMENT);
    expect_error(oversized, delimiters, 0U, 0U, HPS_STRING_INVALID_ARGUMENT);
    expect_error(bad_quote, delimiters, 0U, 8U, HPS_STRING_INVALID_ARGUMENT);
    expect_error(bad_quote, delimiters, bad_quote.length + 1U, 0U, HPS_STRING_OUT_OF_RANGE);
    hps_string_view part = {NULL, 0U};
    uint8_t found = UINT8_MAX;
    cursor = 0U;
    assert(hps_string_tokenizer_next(bad_quote, delimiters, NULL, 0U, &part, &found) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_tokenizer_next(bad_quote, delimiters, &cursor, 0U, NULL, &found) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_tokenizer_next(bad_quote, delimiters, &cursor, 0U, &part, NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(cursor == 0U && part.data == NULL && part.length == 0U && found == UINT8_MAX);
}

int32_t test_string_tokenizer(void)
{
    test_literals_and_progress();
    test_empty_and_counted_inputs();
    test_syntax_precedes_delimiter_membership();
    test_errors_and_disabled_features();
    return INT32_C(0);
}
