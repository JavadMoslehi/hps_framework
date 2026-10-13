#include "../test_suites.h"
#include "hps/cstr/algorithm.h"
#include "hps/cstr/convert.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/**
 * @file tests/cstr/test_cstr_convert.c
 * @brief Independent integer, encoding, token, lexical and dump boundary tests.
 */

static void test_integer_boundaries(void)
{
    uint64_t unsigned_value = UINT64_C(71);
    hps_string_status status = hps_cstr_parse_u64("18446744073709551615", 21U,
                                                10U, &unsigned_value);
    assert(status == HPS_STRING_OK && unsigned_value == UINT64_MAX);
    status = hps_cstr_parse_u64("18446744073709551616", 21U, 10U,
                               &unsigned_value);
    assert(status == HPS_STRING_OUT_OF_RANGE && unsigned_value == UINT64_MAX);
    status = hps_cstr_parse_u64("FFFFFFFFFFFFFFFF", 17U, 16U, &unsigned_value);
    assert(status == HPS_STRING_OK && unsigned_value == UINT64_MAX);
    status = hps_cstr_parse_u64("10000000000000000", 18U, 16U, &unsigned_value);
    assert(status == HPS_STRING_OUT_OF_RANGE && unsigned_value == UINT64_MAX);
    status = hps_cstr_parse_u64("777", 4U, 8U, &unsigned_value);
    assert(status == HPS_STRING_OK && unsigned_value == UINT64_C(511));
    status = hps_cstr_parse_u64("11111111", 9U, 2U, &unsigned_value);
    assert(status == HPS_STRING_OK && unsigned_value == UINT64_C(255));
    const char *invalid[] = {"", "+1", "-1", " 1", "1 ", "0xFF", "12a"};
    for (size_t index = 0U; index < sizeof(invalid) / sizeof(invalid[0]);
         index += 1U) {
        status = hps_cstr_parse_u64(invalid[index], strlen(invalid[index]) + 1U,
                                   10U, &unsigned_value);
        assert(status == HPS_STRING_INVALID_ARGUMENT);
        assert(unsigned_value == UINT64_C(255));
    }
    const char unterminated[2] = {'1', '2'};
    status = hps_cstr_parse_u64(unterminated, sizeof(unterminated), 10U,
                               &unsigned_value);
    assert(status == HPS_STRING_NOT_TERMINATED && unsigned_value == UINT64_C(255));
    status = hps_cstr_parse_u64("1", 2U, 3U, &unsigned_value);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_parse_u64(NULL, 1U, 10U, &unsigned_value);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_parse_u64("1", 0U, 10U, &unsigned_value);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    status = hps_cstr_parse_u64("1", 2U, 10U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);

    int64_t signed_value = INT64_C(71);
    status = hps_cstr_parse_i64("-9223372036854775808", 21U, &signed_value);
    assert(status == HPS_STRING_OK && signed_value == INT64_MIN);
    status = hps_cstr_parse_i64("-9223372036854775809", 21U, &signed_value);
    assert(status == HPS_STRING_OUT_OF_RANGE && signed_value == INT64_MIN);
    status = hps_cstr_parse_i64("+9223372036854775807", 21U, &signed_value);
    assert(status == HPS_STRING_OK && signed_value == INT64_MAX);
    status = hps_cstr_parse_i64("9223372036854775808", 20U, &signed_value);
    assert(status == HPS_STRING_OUT_OF_RANGE && signed_value == INT64_MAX);
    status = hps_cstr_parse_i64("-0", 3U, &signed_value);
    assert(status == HPS_STRING_OK && signed_value == INT64_C(0));
    status = hps_cstr_parse_i64("+", 2U, &signed_value);
    assert(status == HPS_STRING_INVALID_ARGUMENT && signed_value == INT64_C(0));
    status = hps_cstr_parse_i64("--1", 4U, &signed_value);
    assert(status == HPS_STRING_INVALID_ARGUMENT && signed_value == INT64_C(0));

    char output[66];
    (void)memset(output, '?', sizeof(output));
    status = hps_cstr_render_i64(output, 21U, INT64_MIN);
    assert(status == HPS_STRING_OK && strcmp(output, "-9223372036854775808") == 0);
    assert(output[21] == '?');
    char before[66];
    (void)memcpy(before, output, sizeof(before));
    status = hps_cstr_render_i64(output, 20U, INT64_MIN);
    assert(status == HPS_STRING_NO_SPACE && memcmp(output, before, sizeof(output)) == 0);
    status = hps_cstr_render_u64(output, 17U, UINT64_MAX, 16U);
    assert(status == HPS_STRING_OK && strcmp(output, "FFFFFFFFFFFFFFFF") == 0);
    status = hps_cstr_render_u64(output, sizeof(output), UINT64_C(0), 2U);
    assert(status == HPS_STRING_OK && strcmp(output, "0") == 0);
    status = hps_cstr_render_u64(output, sizeof(output), UINT64_C(511), 8U);
    assert(status == HPS_STRING_OK && strcmp(output, "777") == 0);
    status = hps_cstr_render_u64(output, sizeof(output), UINT64_C(511), 9U);
    assert(status == HPS_STRING_INVALID_ARGUMENT && strcmp(output, "777") == 0);
    const uint8_t radices[4] = {2U, 8U, 10U, 16U};
    for (size_t radix = 0U; radix < 4U; radix += 1U) {
        for (uint8_t bit = 0U; bit < 64U; bit += 1U) {
            const uint64_t values[2] = {
                UINT64_C(1) << bit, UINT64_MAX - (UINT64_C(1) << bit)
            };
            for (size_t index = 0U; index < 2U; index += 1U) {
                status = hps_cstr_render_u64(output, sizeof(output), values[index],
                                             radices[radix]);
                assert(status == HPS_STRING_OK);
                status = hps_cstr_parse_u64(output, sizeof(output), radices[radix],
                                            &unsigned_value);
                assert(status == HPS_STRING_OK && unsigned_value == values[index]);
            }
        }
    }
}

static void test_byte_formats(void)
{
    const uint8_t bytes[4] = {0U, 1U, 10U, 255U};
    const char *expected[7] = {
        "00010AFF", "00 01 0A FF", "0x00,0x01,0x0A,0xFF",
        "00000000000000010000101011111111",
        "00000000 00000001 00001010 11111111", "0,1,10,255", "[0, 1, 10, 255]"
    };
    char output[96];
    char before[96];
    for (uint8_t format = 0U; format < 7U; format += 1U) {
        (void)memset(output, '?', sizeof(output));
        const size_t length = strlen(expected[format]);
        hps_string_status status = hps_cstr_encode_bytes(output, length + 1U,
                                                        bytes, 4U, format);
        assert(status == HPS_STRING_OK && strcmp(output, expected[format]) == 0);
        assert(output[length + 1U] == '?');
        (void)memcpy(before, output, sizeof(before));
        status = hps_cstr_encode_bytes(output, length, bytes, 4U, format);
        assert(status == HPS_STRING_NO_SPACE);
        assert(memcmp(output, before, sizeof(output)) == 0);
        status = hps_cstr_encode_bytes(output, sizeof(output), NULL, 0U, format);
        assert(status == HPS_STRING_OK);
        assert(strcmp(output, (format == 6U) ? "[]" : "") == 0);
    }
    uint8_t decoded[8] = {0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU};
    size_t written = 99U;
    hps_string_status status = hps_cstr_decode_bytes(decoded, sizeof(decoded),
        expected[0], strlen(expected[0]) + 1U, HPS_CSTR_BYTES_HEX_COMPACT, &written);
    assert(status == HPS_STRING_OK && written == 4U);
    assert(memcmp(decoded, bytes, sizeof(bytes)) == 0 && decoded[4] == 0xAAU);
    status = hps_cstr_decode_bytes(decoded, sizeof(decoded), "aBc", 4U,
                                   HPS_CSTR_BYTES_HEX_COMPACT, &written);
    assert(status == HPS_STRING_OK && written == 2U);
    assert(decoded[0] == 0x0AU && decoded[1] == 0xBCU);
    const uint8_t saved[8] = {0x0AU, 0xBCU, 10U, 255U, 0xAAU, 0xAAU, 0xAAU, 0xAAU};
    status = hps_cstr_decode_bytes(decoded, sizeof(decoded), "001G", 5U,
                                   HPS_CSTR_BYTES_HEX_COMPACT, &written);
    assert(status == HPS_STRING_INVALID_ARGUMENT && written == 2U);
    assert(memcmp(decoded, saved, sizeof(saved)) == 0);
    status = hps_cstr_decode_bytes(decoded, 1U, "0011", 5U,
                                   HPS_CSTR_BYTES_HEX_COMPACT, &written);
    assert(status == HPS_STRING_NO_SPACE && written == 2U);
    assert(memcmp(decoded, saved, sizeof(saved)) == 0);
    status = hps_cstr_decode_bytes(decoded, sizeof(decoded), expected[3],
                                   strlen(expected[3]) + 1U,
                                   HPS_CSTR_BYTES_BINARY_COMPACT, &written);
    assert(status == HPS_STRING_OK && written == 4U);
    assert(memcmp(decoded, bytes, sizeof(bytes)) == 0);
    status = hps_cstr_decode_bytes(decoded, sizeof(decoded), "1111111", 8U,
                                   HPS_CSTR_BYTES_BINARY_COMPACT, &written);
    assert(status == HPS_STRING_INVALID_ARGUMENT && written == 4U);
    status = hps_cstr_decode_bytes(NULL, 0U, "", 1U,
                                   HPS_CSTR_BYTES_HEX_COMPACT, &written);
    assert(status == HPS_STRING_OK && written == 0U);
    const char unterminated[2] = {'F', 'F'};
    status = hps_cstr_decode_bytes(decoded, sizeof(decoded), unterminated, 2U,
                                   HPS_CSTR_BYTES_HEX_COMPACT, &written);
    assert(status == HPS_STRING_NOT_TERMINATED && written == 0U);
    status = hps_cstr_decode_bytes(decoded, sizeof(decoded), "1", 2U,
                                   HPS_CSTR_BYTES_HEX_SPACED, &written);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    const uint8_t text[3] = {0x41U, 0x5AU, 0xFFU};
    status = hps_cstr_encode_bytes(output, sizeof(output), text, 3U,
                                   HPS_CSTR_BYTES_TEXT);
    assert(status == HPS_STRING_OK && memcmp(output, text, 3U) == 0);
    assert(output[3] == '\0');
    status = hps_cstr_decode_bytes(decoded, sizeof(decoded), output,
                                   sizeof(output), HPS_CSTR_BYTES_TEXT, &written);
    assert(status == HPS_STRING_OK && written == 3U && memcmp(decoded, text, 3U) == 0);
    (void)memcpy(before, output, sizeof(before));
    status = hps_cstr_encode_bytes(output, sizeof(output), bytes, 4U,
                                   HPS_CSTR_BYTES_TEXT);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(before, output, sizeof(output)) == 0);
    char bits[8] = "01001";
    status = hps_cstr_invert_binary(bits, sizeof(bits), bits, sizeof(bits), 3U);
    assert(status == HPS_STRING_OK && strcmp(bits, "10110") == 0);
    status = hps_cstr_invert_binary(bits, sizeof(bits), bits, sizeof(bits), 4U);
    assert(status == HPS_STRING_OK && strcmp(bits, "10110") == 0);
    status = hps_cstr_invert_binary(bits, sizeof(bits), "01X", 4U, 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT && strcmp(bits, "10110") == 0);
    status = hps_cstr_invert_binary(bits, 5U, "00000", 6U, 1U);
    assert(status == HPS_STRING_NO_SPACE && strcmp(bits, "10110") == 0);
    const char *boolean = NULL;
    status = hps_cstr_boolalpha(1U, &boolean);
    assert(status == HPS_STRING_OK && strcmp(boolean, "true") == 0);
    status = hps_cstr_boolalpha(2U, &boolean);
    assert(status == HPS_STRING_INVALID_ARGUMENT && strcmp(boolean, "true") == 0);
    status = hps_cstr_boolalpha(0U, &boolean);
    assert(status == HPS_STRING_OK && strcmp(boolean, "false") == 0);
}

static void test_validation_and_tokens(void)
{
    const char *valid[6] = {"10101", "0123456789", "0761", "A0bf9", "Az", "a9Z"};
    const char *invalid[6] = {"102", "-1", "8", "G", "A1", "a_"};
    for (uint8_t character_class = 0U; character_class < 6U; character_class += 1U) {
        uint8_t result = 9U;
        hps_string_status status = hps_cstr_validate(valid[character_class],
            strlen(valid[character_class]) + 1U, character_class, &result);
        assert(status == HPS_STRING_OK && result == 1U);
        status = hps_cstr_validate(invalid[character_class],
            strlen(invalid[character_class]) + 1U, character_class, &result);
        assert(status == HPS_STRING_OK && result == 0U);
        status = hps_cstr_validate("", 1U, character_class, &result);
        assert(status == HPS_STRING_OK && result == 0U);
    }
    uint8_t flag = 7U;
    hps_string_status status = hps_cstr_validate("1", 2U, 6U, &flag);
    assert(status == HPS_STRING_INVALID_ARGUMENT && flag == 7U);
    const char unterminated[1] = {'1'};
    status = hps_cstr_validate(unterminated, 1U, HPS_CSTR_CLASS_BINARY, &flag);
    assert(status == HPS_STRING_NOT_TERMINATED && flag == 7U);

    const char input[] = "::one::::two::";
    size_t count = 99U;
    status = hps_cstr_split_count(input, sizeof(input), "::", 3U, &count);
    assert(status == HPS_STRING_OK && count == 5U);
    hps_string_view tokens[5];
    for (size_t index = 0U; index < 5U; index += 1U) {
        tokens[index].data = "saved";
        tokens[index].length = 5U;
    }
    status = hps_cstr_split(input, sizeof(input), "::", 3U, tokens, 4U, &count);
    assert(status == HPS_STRING_NO_SPACE && count == 5U);
    for (size_t index = 0U; index < 5U; index += 1U) {
        assert(tokens[index].length == 5U && strcmp(tokens[index].data, "saved") == 0);
    }
    status = hps_cstr_split(input, sizeof(input), "::", 3U, tokens, 5U, &count);
    assert(status == HPS_STRING_OK && count == 5U);
    assert(tokens[0].data == NULL && tokens[0].length == 0U);
    assert(tokens[2].data == NULL && tokens[2].length == 0U);
    assert(tokens[4].data == NULL && tokens[4].length == 0U);
    assert(tokens[1].data == &input[2] && tokens[1].length == 3U);
    assert(tokens[3].data == &input[9] && tokens[3].length == 3U);
    hps_string_view token = {"saved", 5U};
    status = hps_cstr_split_at(input, sizeof(input), "::", 3U, 3U, &token);
    assert(status == HPS_STRING_OK && token.data == &input[9] && token.length == 3U);
    status = hps_cstr_split_at(input, sizeof(input), "::", 3U, SIZE_MAX, &token);
    assert(status == HPS_STRING_OUT_OF_RANGE && token.data == &input[9]);
    status = hps_cstr_split_count(input, sizeof(input), "", 1U, &count);
    assert(status == HPS_STRING_INVALID_ARGUMENT && count == 5U);
    status = hps_cstr_split("", 1U, "::", 3U, NULL, 0U, &count);
    assert(status == HPS_STRING_OK && count == 0U);
    status = hps_cstr_split_at("", 1U, ":", 2U, 0U, &token);
    assert(status == HPS_STRING_OUT_OF_RANGE && token.data == &input[9]);
    char output[80];
    status = hps_cstr_join(output, sizeof(output), tokens, 5U, "|", 2U);
    assert(status == HPS_STRING_OK && strcmp(output, "|one||two|") == 0);
    status = hps_cstr_join(output, 9U, tokens, 5U, "|", 2U);
    assert(status == HPS_STRING_NO_SPACE && strcmp(output, "|one||two|") == 0);
    const char embedded[3] = {'a', '\0', 'b'};
    const hps_string_view bad[1] = {{embedded, sizeof(embedded)}};
    status = hps_cstr_join(output, sizeof(output), bad, 1U, "", 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT && strcmp(output, "|one||two|") == 0);
    const hps_string_view reserved_length[1] = {{"x", SIZE_MAX}};
    status = hps_cstr_join(output, sizeof(output), reserved_length, 1U, "", 1U);
    assert(status == HPS_STRING_INVALID_ARGUMENT && strcmp(output, "|one||two|") == 0);
    status = hps_cstr_join(output, sizeof(output), NULL, 0U, "", 1U);
    assert(status == HPS_STRING_OK && output[0] == '\0');
    status = hps_cstr_split_count("aaa", 4U, "aa", 3U, &count);
    assert(status == HPS_STRING_OK && count == 2U);
}

static void test_group_paths_and_dumps(void)
{
    char output[96] = "saved";
    hps_string_status status = hps_cstr_group(output, sizeof(output),
        "1234567", 8U, 3U, ",", 2U);
    assert(status == HPS_STRING_OK && strcmp(output, "1,234,567") == 0);
    status = hps_cstr_group(output, sizeof(output), "123456", 7U, 3U, "::", 3U);
    assert(status == HPS_STRING_OK && strcmp(output, "123::456") == 0);
    status = hps_cstr_group(output, 8U, "123456", 7U, 3U, "::", 3U);
    assert(status == HPS_STRING_NO_SPACE && strcmp(output, "123::456") == 0);
    status = hps_cstr_group(output, sizeof(output), "x", 2U, SIZE_MAX, ",", 2U);
    assert(status == HPS_STRING_OK && strcmp(output, "x") == 0);
    status = hps_cstr_group(output, sizeof(output), "x", 2U, 0U, ",", 2U);
    assert(status == HPS_STRING_INVALID_ARGUMENT && strcmp(output, "x") == 0);
    const char path[] = "parent.dir\\folder/file.tar.gz";
    hps_string_view file = {NULL, 0U};
    status = hps_cstr_filename(path, sizeof(path), 0U, &file);
    assert(status == HPS_STRING_OK && file.length == 11U);
    assert(memcmp(file.data, "file.tar.gz", file.length) == 0);
    status = hps_cstr_filename(path, sizeof(path), 1U, &file);
    assert(status == HPS_STRING_OK && file.length == 8U);
    assert(memcmp(file.data, "file.tar", file.length) == 0);
    status = hps_cstr_filename("dir/.profile", 13U, 1U, &file);
    assert(status == HPS_STRING_OK && file.length == 8U);
    status = hps_cstr_filename("dir/", 5U, 1U, &file);
    assert(status == HPS_STRING_OK && file.data == NULL && file.length == 0U);
    status = hps_cstr_filename("", 1U, 0U, &file);
    assert(status == HPS_STRING_OK && file.data == NULL && file.length == 0U);
    const char left[] = "alpha";
    const char right[] = "beta";
    status = hps_cstr_select(left, sizeof(left), right, sizeof(right), 0U, &file);
    assert(status == HPS_STRING_OK && file.data == left);
    status = hps_cstr_select(left, sizeof(left), right, sizeof(right), 1U, &file);
    assert(status == HPS_STRING_OK && file.data == right);
    status = hps_cstr_title_value(output, sizeof(output), "name", 5U, ".", 2U,
                                  "42", 3U, 10U);
    assert(status == HPS_STRING_OK && strcmp(output, "name....42") == 0);
    status = hps_cstr_title_value(output, sizeof(output), "name", 5U, "::", 3U,
                                  "42", 3U, 9U);
    assert(status == HPS_STRING_OK && strcmp(output, "name::42") == 0);
    status = hps_cstr_title_value(output, sizeof(output), "name", 5U, ".", 2U,
                                  "42", 3U, 5U);
    assert(status == HPS_STRING_NO_SPACE && strcmp(output, "name::42") == 0);
    status = hps_cstr_title_value(output, sizeof(output), "name", 5U, "", 1U,
                                  "42", 3U, 20U);
    assert(status == HPS_STRING_OK && strcmp(output, "name42") == 0);
    status = hps_cstr_dump_i64(output, sizeof(output), "value", 6U, INT64_MIN);
    assert(status == HPS_STRING_OK && strcmp(output, "value: -9223372036854775808\n") == 0);
    const uint8_t bytes[4] = {0x41U, 0U, 0x7EU, 0xFFU};
    status = hps_cstr_dump_bytes(output, sizeof(output), "data", 5U, bytes, 4U);
    assert(status == HPS_STRING_OK && strcmp(output, "data: A.~. | HEX: 41 00 7E FF\n") == 0);
    status = hps_cstr_dump_bytes(output, 30U, "data", 5U, bytes, 4U);
    assert(status == HPS_STRING_NO_SPACE && strcmp(output, "data: A.~. | HEX: 41 00 7E FF\n") == 0);
    status = hps_cstr_dump_bytes(output, sizeof(output), "data", 5U, NULL, 0U);
    assert(status == HPS_STRING_OK && strcmp(output, "data:  | HEX:\n") == 0);
    status = hps_cstr_dump_bytes(output, sizeof(output), "data", 5U, bytes, SIZE_MAX);
    assert(status == HPS_STRING_OUT_OF_RANGE && strcmp(output, "data:  | HEX:\n") == 0);
}

int32_t test_cstr_convert(void)
{
    test_integer_boundaries();
    test_byte_formats();
    test_validation_and_tokens();
    test_group_paths_and_dumps();
    return INT32_C(0);
}
