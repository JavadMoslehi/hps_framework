#include "hps/json/tokenizer.h"
#include "internal.h"

/**
 * @file src/json/tokenizer.c
 * @brief Bounded transactional lexical JSON tokens and integer decoding.
 */

static uint8_t whitespace(const char *value)
{
    return (uint8_t)((*value == ' ') || (*value == '\t') ||
                     (*value == '\r') || (*value == '\n'));
}

static uint8_t digit(const char *value)
{
    return (uint8_t)((*value >= '0') && (*value <= '9'));
}

static uint8_t boundary(const hps_string_view input, const size_t position)
{
    uint8_t result = 1U;
    if (position < input.length) {
        const char *value = &input.data[position];
        result = (uint8_t)((whitespace(value) == 1U) || (*value == '{') ||
            (*value == '}') || (*value == '[') || (*value == ']') ||
            (*value == ',') || (*value == ':'));
    }
    return result;
}

static int32_t scan_number(const hps_string_view input, const size_t start,
                              size_t *end)
{
    size_t position = start;
    int32_t status = HPS_JSON_OK;
    if (input.data[position] == '-') {
        position += 1U;
    }
    if ((position == input.length) || (digit(&input.data[position]) == 0U)) {
        status = HPS_JSON_SYNTAX;
    } else if (input.data[position] == '0') {
        position += 1U;
    } else {
        while ((position < input.length) && (digit(&input.data[position]) == 1U)) {
            position += 1U;
        }
    }
    if ((status == HPS_JSON_OK) && (position < input.length) &&
        (input.data[position] == '.')) {
        position += 1U;
        const size_t first = position;
        while ((position < input.length) && (digit(&input.data[position]) == 1U)) {
            position += 1U;
        }
        if (position == first) {
            status = HPS_JSON_SYNTAX;
        }
    }
    if ((status == HPS_JSON_OK) && (position < input.length) &&
        ((input.data[position] == 'e') || (input.data[position] == 'E'))) {
        position += 1U;
        if ((position < input.length) &&
            ((input.data[position] == '+') || (input.data[position] == '-'))) {
            position += 1U;
        }
        const size_t first = position;
        while ((position < input.length) && (digit(&input.data[position]) == 1U)) {
            position += 1U;
        }
        if (position == first) {
            status = HPS_JSON_SYNTAX;
        }
    }
    if ((status == HPS_JSON_OK) && (boundary(input, position) == 0U)) {
        status = HPS_JSON_SYNTAX;
    }
    if (status == HPS_JSON_OK) {
        *end = position;
    }
    return status;
}

static int32_t scan_keyword(const hps_string_view input, const size_t start,
                               const char *keyword, const size_t length,
                               size_t *end)
{
    int32_t status = HPS_JSON_SYNTAX;
    if (length <= input.length - start) {
        status = HPS_JSON_OK;
        for (size_t index = 0U;
             (index < length) && (status == HPS_JSON_OK); index += 1U) {
            if (input.data[start + index] != keyword[index]) {
                status = HPS_JSON_SYNTAX;
            }
        }
        if ((status == HPS_JSON_OK) && (boundary(input, start + length) == 1U)) {
            *end = start + length;
        } else {
            status = HPS_JSON_SYNTAX;
        }
    }
    return status;
}

int32_t hps_json_tokenizer_init(hps_json_tokenizer *tokenizer,
                                 const hps_string_view input)
{
    int32_t status = HPS_JSON_INVALID_ARGUMENT;
    if ((tokenizer != NULL) && (input.length < SIZE_MAX) &&
        ((input.data != NULL) || (input.length == 0U))) {
        const hps_json_tokenizer initial = {input, 0U, 1U, 1U};
        *tokenizer = initial;
        status = HPS_JSON_OK;
    }
    return status;
}

int32_t hps_json_tokenizer_next(hps_json_tokenizer *tokenizer,
                                 hps_json_token *token)
{
    int32_t status = HPS_JSON_INVALID_ARGUMENT;
    if ((tokenizer != NULL) && (token != NULL)) {
        status = HPS_JSON_INVALID_STATE;
        if ((tokenizer->input.length < SIZE_MAX) &&
            ((tokenizer->input.data != NULL) || (tokenizer->input.length == 0U)) &&
            (tokenizer->position <= tokenizer->input.length) &&
            (tokenizer->line > 0U) && (tokenizer->column > 0U) &&
            (tokenizer->line <= tokenizer->position + 1U) &&
            (tokenizer->column <= tokenizer->position + 1U)) {
            hps_json_tokenizer next = *tokenizer;
            while ((next.position < next.input.length) &&
                   (whitespace(&next.input.data[next.position]) == 1U)) {
                if (next.input.data[next.position] == '\n') {
                    next.line += 1U;
                    next.column = 1U;
                } else {
                    next.column += 1U;
                }
                next.position += 1U;
            }
            hps_json_token found = {HPS_JSON_EOF, {NULL, 0U},
                next.position, next.line, next.column};
            status = HPS_JSON_OK;
            if (next.position < next.input.length) {
                size_t end = next.position + 1U;
                size_t decoded = 0U;
                switch (next.input.data[next.position]) {
                    case '{': found.kind = HPS_JSON_OBJECT_BEGIN; break;
                    case '}': found.kind = HPS_JSON_OBJECT_END; break;
                    case '[': found.kind = HPS_JSON_ARRAY_BEGIN; break;
                    case ']': found.kind = HPS_JSON_ARRAY_END; break;
                    case ',': found.kind = HPS_JSON_COMMA; break;
                    case ':': found.kind = HPS_JSON_COLON; break;
                    case '"':
                        found.kind = HPS_JSON_STRING;
                        status = hps_json_scan_string(next.input, next.position,
                                                         &end, NULL, &decoded);
                        break;
                    case 't':
                        found.kind = HPS_JSON_TRUE;
                        status = scan_keyword(next.input, next.position, "true", 4U, &end);
                        break;
                    case 'f':
                        found.kind = HPS_JSON_FALSE;
                        status = scan_keyword(next.input, next.position, "false", 5U, &end);
                        break;
                    case 'n':
                        found.kind = HPS_JSON_NULL;
                        status = scan_keyword(next.input, next.position, "null", 4U, &end);
                        break;
                    default:
                        found.kind = HPS_JSON_NUMBER;
                        status = scan_number(next.input, next.position, &end);
                        break;
                }
                if (status == HPS_JSON_OK) {
                    found.raw.data = &next.input.data[next.position];
                    found.raw.length = end - next.position;
                    /* Valid string/literal/number tokens contain no raw LF.
                     * column <= position+1, end <= length < SIZE_MAX. */
                    next.column += found.raw.length;
                    next.position = end;
                }
            }
            if (status == HPS_JSON_OK) {
                *tokenizer = next;
                *token = found;
            }
        }
    }
    return status;
}

static int32_t validate_scalar_token(const hps_json_token *token,
                                        const uint8_t required)
{
    int32_t status = HPS_JSON_INVALID_ARGUMENT;
    if ((token != NULL) && (token->kind == required)) {
        hps_json_tokenizer tokenizer;
        hps_json_token actual;
        status = hps_json_tokenizer_init(&tokenizer, token->raw);
        if (status == HPS_JSON_OK) {
            status = hps_json_tokenizer_next(&tokenizer, &actual);
        }
        if ((status == HPS_JSON_OK) &&
            ((actual.kind != required) || (actual.offset != 0U) ||
             (actual.raw.length != token->raw.length))) {
            status = HPS_JSON_SYNTAX;
        }
    }
    return status;
}

int32_t hps_json_token_decode_string(const hps_json_token *token,
                                      uint8_t *output, const size_t capacity,
                                      size_t *written)
{
    int32_t status = HPS_JSON_INVALID_ARGUMENT;
    if ((written != NULL) && ((output != NULL) || (capacity == 0U))) {
        status = validate_scalar_token(token, HPS_JSON_STRING);
        if (status == HPS_JSON_OK) {
            size_t end = 0U;
            size_t count = 0U;
            status = hps_json_scan_string(token->raw, 0U, &end, NULL, &count);
            if ((status == HPS_JSON_OK) && (count > capacity)) {
                status = HPS_JSON_NO_SPACE;
            }
            if (status == HPS_JSON_OK) {
                status = hps_json_scan_string(token->raw, 0U, &end, output, &count);
                if (status == HPS_JSON_OK) {
                    *written = count;
                }
            }
        }
    }
    return status;
}

int32_t hps_json_token_i64(const hps_json_token *token, int64_t *result)
{
    int32_t status = HPS_JSON_INVALID_ARGUMENT;
    if (result != NULL) {
        status = validate_scalar_token(token, HPS_JSON_NUMBER);
        if (status == HPS_JSON_OK) {
            size_t position = 0U;
            const uint8_t negative = (uint8_t)(token->raw.data[0] == '-');
            const uint64_t limit = (uint64_t)INT64_MAX + (uint64_t)negative;
            uint64_t magnitude = UINT64_C(0);
            if (negative == 1U) {
                position = 1U;
            }
            /* Establish integer spelling before range checks so a large decimal
             * or exponent has the same SYNTAX result as a small one. */
            for (size_t index = position;
                 (index < token->raw.length) && (status == HPS_JSON_OK); index += 1U) {
                if (digit(&token->raw.data[index]) == 0U) {
                    status = HPS_JSON_SYNTAX;
                }
            }
            while ((position < token->raw.length) && (status == HPS_JSON_OK)) {
                const uint64_t value = (uint64_t)((uint8_t)token->raw.data[position] - 0x30U);
                if (magnitude > (limit - value) / UINT64_C(10)) {
                    status = HPS_JSON_OUT_OF_RANGE;
                } else {
                    magnitude = magnitude * UINT64_C(10) + value;
                }
                position += 1U;
            }
            if (status == HPS_JSON_OK) {
                if (negative == 0U) {
                    *result = (int64_t)magnitude;
                } else if (magnitude == (uint64_t)INT64_MAX + UINT64_C(1)) {
                    *result = INT64_MIN;
                } else {
                    *result = -(int64_t)magnitude;
                }
            }
        }
    }
    return status;
}
