#include "../test_suites.h"
#include "hps/json/json.h"
#include "hps/string/hps_fixed.h"

#include <assert.h>
#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include <string.h>

/**
 * @file tests/json/test_json.c
 * @brief Independent syntax, Unicode, resource, failure and JSON output checks.
 */

HPS_STRING_FIXED_DEFINE(json_test_storage, 512U)

static hps_string_view text(const char *input)
{
    const hps_string_view result = {input, strlen(input)};
    return result;
}

static void same_token(const hps_json_token *actual, const hps_json_token *expected)
{
    assert(actual->kind == expected->kind);
    assert(actual->raw.data == expected->raw.data);
    assert(actual->raw.length == expected->raw.length);
    assert(actual->offset == expected->offset);
    assert(actual->line == expected->line);
    assert(actual->column == expected->column);
}

static void test_tokenization(void)
{
    const char *source = " \n{\"a\":true,\"b\":[-2.5e+3,null,false]}";
    const uint8_t expected[] = {
        HPS_JSON_OBJECT_BEGIN, HPS_JSON_STRING, HPS_JSON_COLON, HPS_JSON_TRUE,
        HPS_JSON_COMMA, HPS_JSON_STRING, HPS_JSON_COLON, HPS_JSON_ARRAY_BEGIN,
        HPS_JSON_NUMBER, HPS_JSON_COMMA, HPS_JSON_NULL, HPS_JSON_COMMA,
        HPS_JSON_FALSE, HPS_JSON_ARRAY_END, HPS_JSON_OBJECT_END, HPS_JSON_EOF
    };
    hps_json_tokenizer tokenizer;
    hps_json_token token;
    assert(hps_json_tokenizer_init(&tokenizer, text(source)) == HPS_JSON_OK);
    for (size_t index = 0U; index < sizeof(expected); index += 1U) {
        assert(hps_json_tokenizer_next(&tokenizer, &token) == HPS_JSON_OK);
        assert(token.kind == expected[index]);
        assert(token.line == 2U);
        if (index == 0U) {
            assert(token.offset == 2U && token.column == 1U);
        }
        if (token.kind == HPS_JSON_TRUE) {
            assert(token.raw.length == 4U && token.raw.data == source + 7U);
            assert(token.offset == 7U && token.column == 6U);
        }
        if (token.kind == HPS_JSON_FALSE) {
            assert(token.raw.length == 5U && memcmp(token.raw.data, "false", 5U) == 0);
        }
        if (token.kind == HPS_JSON_EOF) {
            assert(token.raw.data == NULL && token.raw.length == 0U);
        }
    }
    const hps_json_token final = token;
    assert(hps_json_tokenizer_next(&tokenizer, &token) == HPS_JSON_OK);
    same_token(&token, &final);

    const char *invalid[] = {"-", "01", "1.", "1e", "1e+", "truex", "null0",
        "\"\\q\"", "\"\\u000X\"", "\"\\uD800\"", "\"\\uDC00\"", "\"a\n\"",
        "\"\xC0\xAF\"", "\"\xED\xA0\x80\"", "\"\xF4\x90\x80\x80\"",
        "\"\xE2\x82\"", "\"\x80\"", "\vnull", "/*x*/null", "\xEF\xBB\xBFnull"};
    for (size_t index = 0U; index < sizeof(invalid) / sizeof(invalid[0]); index += 1U) {
        assert(hps_json_tokenizer_init(&tokenizer, text(invalid[index])) == HPS_JSON_OK);
        const hps_json_tokenizer before = tokenizer;
        token = final;
        assert(hps_json_tokenizer_next(&tokenizer, &token) == HPS_JSON_SYNTAX);
        assert(tokenizer.position == before.position && tokenizer.line == before.line);
        assert(tokenizer.column == before.column && tokenizer.input.data == before.input.data);
        same_token(&token, &final);
    }
    tokenizer.position = tokenizer.input.length + 1U;
    assert(hps_json_tokenizer_next(&tokenizer, &token) == HPS_JSON_INVALID_STATE);
    assert(hps_json_tokenizer_next(NULL, &token) == HPS_JSON_INVALID_ARGUMENT);
    const hps_string_view malformed = {NULL, 1U};
    assert(hps_json_tokenizer_init(&tokenizer, malformed) == HPS_JSON_INVALID_ARGUMENT);
}

static void test_documents(void)
{
    hps_json_token tokens[32];
    hps_json_frame frames[8];
    const hps_json_token sentinel = {UINT8_C(88), {"!", 1U}, 99U, 98U, 97U};
    for (size_t index = 0U; index < 32U; index += 1U) {
        tokens[index] = sentinel;
    }
    const hps_json_document initial = {{"!", 1U}, tokens, 22U, 33U};
    hps_json_document document = initial;
    const char *invalid[] = {"", " ", "true false", "[", "[1,]", "[,1]", "[1 2]",
        "{", "{\"a\"}", "{\"a\":}", "{\"a\":1,}", "{1:2}", "{\"a\":1 \"b\":2}",
        "[}", "{]", "[[1]", "[]0", "//x\n{}", "{\"a\":/*x*/2}"};
    for (size_t index = 0U; index < sizeof(invalid) / sizeof(invalid[0]); index += 1U) {
        assert(hps_json_read(text(invalid[index]), tokens, 32U, frames, 8U,
                                &document) == HPS_JSON_SYNTAX);
        assert(document.input.data == initial.input.data && document.tokens == initial.tokens);
        assert(document.token_count == initial.token_count && document.maximum_depth == initial.maximum_depth);
        for (size_t slot = 0U; slot < 32U; slot += 1U) {
            same_token(&tokens[slot], &sentinel);
        }
    }
    assert(hps_json_read(text("[]"), tokens, 32U, NULL, 0U, &document) == HPS_JSON_DEPTH_EXCEEDED);
    assert(hps_json_read(text("[[]]"), tokens, 32U, frames, 1U, &document) == HPS_JSON_DEPTH_EXCEEDED);
    assert(hps_json_read(text("[0]"), tokens, 2U, frames, 8U, &document) == HPS_JSON_NO_SPACE);
    assert(document.token_count == initial.token_count);
    same_token(&tokens[0], &sentinel);
    assert(hps_json_read(text("null"), tokens, 1U, NULL, 0U, &document) == HPS_JSON_OK);
    assert(document.token_count == 1U && document.maximum_depth == 0U);
    assert(hps_json_read(text(" {\"a\":[1,true],\"a\":null} "), tokens, 32U, frames, 8U,
                            &document) == HPS_JSON_OK);
    assert(document.token_count == 13U && document.maximum_depth == 2U);
    assert(tokens[1].kind == HPS_JSON_STRING && tokens[3].kind == HPS_JSON_ARRAY_BEGIN);
    assert(hps_json_read(text("1"), NULL, 0U, frames, 8U, &document) == HPS_JSON_NO_SPACE);
    assert(hps_json_read(text("1"), NULL, 1U, frames, 8U, &document) == HPS_JSON_INVALID_ARGUMENT);
    assert(hps_json_read(text("1"), tokens, SIZE_MAX, frames, 8U, &document) == HPS_JSON_INVALID_ARGUMENT);
    assert(hps_json_read(text("1"), tokens, 32U, frames, SIZE_MAX, &document) == HPS_JSON_INVALID_ARGUMENT);

    json_test_storage_t nested = HPS_STRING_FIXED_STATIC_INIT;
    hps_json_token many_tokens[400];
    hps_json_frame many_frames[200];
    for (size_t index = 0U; index < 200U; index += 1U) {
        nested.data[index] = '[';
        nested.data[200U + index] = ']';
    }
    const hps_string_view deep = {nested.data, 400U};
    assert(hps_json_read(deep, many_tokens, 400U, many_frames, 200U, &document) == HPS_JSON_OK);
    assert(document.maximum_depth == 200U && document.token_count == 400U);
    assert(hps_json_read(deep, many_tokens, 400U, many_frames, 199U, &document) == HPS_JSON_DEPTH_EXCEEDED);
}

static hps_json_token one_token(const char *source)
{
    hps_json_tokenizer tokenizer;
    hps_json_token token;
    assert(hps_json_tokenizer_init(&tokenizer, text(source)) == HPS_JSON_OK);
    assert(hps_json_tokenizer_next(&tokenizer, &token) == HPS_JSON_OK);
    return token;
}

static void test_scalar_decoding(void)
{
    hps_json_token token = one_token("\"a\\u0000\\n\\/\\u00e9\\uD834\\uDD1E\"");
    const uint8_t expected[] = {0x61U, 0U, 10U, 0x2FU, 0xC3U, 0xA9U, 0xF0U, 0x9DU, 0x84U, 0x9EU};
    uint8_t bytes[16];
    uint8_t before[16];
    (void)memset(bytes, 0xAA, sizeof(bytes));
    (void)memcpy(before, bytes, sizeof(before));
    size_t count = 77U;
    assert(hps_json_token_decode_string(&token, bytes, 9U, &count) == HPS_JSON_NO_SPACE);
    assert(count == 77U && memcmp(bytes, before, sizeof(bytes)) == 0);
    assert(hps_json_token_decode_string(&token, bytes, 10U, &count) == HPS_JSON_OK);
    assert(count == sizeof(expected) && memcmp(bytes, expected, sizeof(expected)) == 0);
    assert(bytes[10] == 0xAAU);
    token = one_token("\"\xC3\xA9\xF0\x9D\x84\x9E\"");
    assert(hps_json_token_decode_string(&token, bytes, sizeof(bytes), &count) == HPS_JSON_OK);
    assert(count == 6U && memcmp(bytes, expected + 4U, 6U) == 0);
    const uint8_t boundary_bytes[] = {0xC2U, 0x80U, 0xF4U, 0x8FU, 0xBFU, 0xBFU};
    token = one_token("\"\\u0080\\uDBFF\\uDFFF\"");
    assert(hps_json_token_decode_string(&token, bytes, sizeof(bytes), &count) == HPS_JSON_OK);
    assert(count == sizeof(boundary_bytes) && memcmp(bytes, boundary_bytes, count) == 0);
    token = one_token("\"\xC2\x80\xF4\x8F\xBF\xBF\"");
    assert(hps_json_token_decode_string(&token, bytes, sizeof(bytes), &count) == HPS_JSON_OK);
    assert(count == sizeof(boundary_bytes) && memcmp(bytes, boundary_bytes, count) == 0);
    token = one_token("\"\"");
    assert(hps_json_token_decode_string(&token, NULL, 0U, &count) == HPS_JSON_OK && count == 0U);
    token.raw = text("\"\\uD800\"");
    count = 71U;
    (void)memcpy(before, bytes, sizeof(before));
    assert(hps_json_token_decode_string(&token, bytes, sizeof(bytes), &count) == HPS_JSON_SYNTAX);
    assert(count == 71U && memcmp(bytes, before, sizeof(bytes)) == 0);

    int64_t value = INT64_C(77);
    token = one_token("-9223372036854775808");
    assert(hps_json_token_i64(&token, &value) == HPS_JSON_OK && value == INT64_MIN);
    token = one_token("9223372036854775807");
    assert(hps_json_token_i64(&token, &value) == HPS_JSON_OK && value == INT64_MAX);
    token = one_token("9223372036854775808");
    assert(hps_json_token_i64(&token, &value) == HPS_JSON_OUT_OF_RANGE && value == INT64_MAX);
    token = one_token("-9223372036854775809");
    assert(hps_json_token_i64(&token, &value) == HPS_JSON_OUT_OF_RANGE && value == INT64_MAX);
    token = one_token("1e9999");
    assert(hps_json_token_i64(&token, &value) == HPS_JSON_SYNTAX && value == INT64_MAX);
    const char *nonintegers[] = {
        "999999999999999999999999e0", "999999999999999999999999.0",
        "-999999999999999999999999E+1", "-999999999999999999999999.5"
    };
    for (size_t index = 0U; index < sizeof(nonintegers) / sizeof(nonintegers[0]); index += 1U) {
        token = one_token(nonintegers[index]);
        assert(hps_json_token_i64(&token, &value) == HPS_JSON_SYNTAX);
        assert(value == INT64_MAX);
    }
    token = one_token("-0");
    assert(hps_json_token_i64(&token, &value) == HPS_JSON_OK && value == INT64_C(0));
    token.raw = text(" 1");
    assert(hps_json_token_i64(&token, &value) == HPS_JSON_SYNTAX && value == INT64_C(0));
    token.raw = text("01");
    assert(hps_json_token_i64(&token, &value) == HPS_JSON_SYNTAX && value == INT64_C(0));
}

static void test_compact_writer(void)
{
    json_test_storage_t storage;
    json_test_storage_t before;
    hps_string_buffer output;
    hps_json_frame frames[8];
    (void)memset(storage.data, '?', sizeof(storage.data));
    assert(json_test_storage_capacity() == sizeof(storage.data));
    assert(json_test_storage_bind(&storage, &output) == HPS_STRING_OK);
    assert(hps_string_buffer_assign(&output, text("previous")) == HPS_STRING_OK);
    before = storage;
    assert(hps_json_write_compact(&output, text(" [1,] "), frames, 8U) == HPS_JSON_SYNTAX);
    assert(output.length == 8U && memcmp(storage.data, before.data, sizeof(storage.data)) == 0);
    assert(hps_json_write_compact(&output, text(" [[]] "), frames, 1U) == HPS_JSON_DEPTH_EXCEEDED);
    assert(memcmp(storage.data, before.data, sizeof(storage.data)) == 0);
    assert(hps_json_write_compact(&output, text("null"), frames, SIZE_MAX) == HPS_JSON_INVALID_ARGUMENT);
    assert(memcmp(storage.data, before.data, sizeof(storage.data)) == 0);
    const char *source = " \n { \"x\" : [ -0, 1.20e+9, \" a \\n b \" ], \"x\":null } \r ";
    const char *expected = "{\"x\":[-0,1.20e+9,\" a \\n b \"],\"x\":null}";
    assert(hps_json_write_compact(&output, text(source), frames, 8U) == HPS_JSON_OK);
    assert(strcmp(storage.data, expected) == 0 && output.length == strlen(expected));
    assert(storage.data[output.length + 1U] == '?');
    hps_string_buffer adopted;
    assert(json_test_storage_adopt(&storage, &adopted) == HPS_STRING_OK);
    assert(adopted.data == output.data && adopted.length == output.length);
    hps_string_buffer tiny;
    assert(hps_string_buffer_init(&tiny, before.data, 3U) == HPS_STRING_OK);
    before.data[1] = '!';
    assert(hps_json_write_compact(&tiny, text("null"), NULL, 0U) == HPS_JSON_NO_SPACE);
    assert(tiny.length == 0U && before.data[0] == '\0' && before.data[1] == '!');
    tiny.length = tiny.capacity;
    assert(hps_json_write_compact(&tiny, text("null"), NULL, 0U) == HPS_JSON_INVALID_STATE);
    assert(hps_json_write_compact(NULL, text("null"), NULL, 0U) == HPS_JSON_INVALID_ARGUMENT);
}

int32_t test_json(void)
{
    test_tokenization();
    test_documents();
    test_scalar_decoding();
    test_compact_writer();
    return INT32_C(0);
}
