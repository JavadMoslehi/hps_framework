#include "hps/cstr/algorithm.h"

#include "hps/cstr/convert.h"
#include "../string/internal.h"
#include <string.h>

/**
 * @file src/cstr/algorithm.c
 * @brief Validated bounded token, lexical path and deterministic dump operations.
 */

static uint8_t member_of(const char *character, const char *members,
                         const size_t count)
{
    uint8_t found = 0U;
    for (size_t index = 0U; index < count; index += 1U) {
        if (*character == members[index]) {
            found = 1U;
        }
    }
    return found;
}

hps_string_status hps_cstr_validate(const char *input,
                                    const size_t input_capacity,
                                    const uint8_t character_class,
                                    uint8_t *result)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if ((result != NULL) && (character_class <= HPS_CSTR_CLASS_ALNUM)) {
        hps_string_view text = {NULL, 0U};
        status = hps_string_view_init_cstr(&text, input, input_capacity);
        if (status == HPS_STRING_OK) {
            const char *members = "";
            size_t count = 0U;
            switch (character_class) {
                case HPS_CSTR_CLASS_BINARY:
                    members = "01";
                    count = 2U;
                    break;
                case HPS_CSTR_CLASS_DECIMAL:
                    members = "0123456789";
                    count = 10U;
                    break;
                case HPS_CSTR_CLASS_OCTAL:
                    members = "01234567";
                    count = 8U;
                    break;
                case HPS_CSTR_CLASS_HEX:
                    members = "0123456789ABCDEFabcdef";
                    count = 22U;
                    break;
                case HPS_CSTR_CLASS_ALPHA:
                    members = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
                    count = 52U;
                    break;
                case HPS_CSTR_CLASS_ALNUM:
                    members = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
                    count = 62U;
                    break;
                default:
                    break;
            }
            uint8_t valid = (uint8_t)(text.length > 0U);
            for (size_t index = 0U;
                 (index < text.length) && (valid == 1U); index += 1U) {
                valid = member_of(&text.data[index], members, count);
            }
            *result = valid;
        }
    }
    return status;
}

static hps_string_status split_inputs(const char *input,
                                       const size_t input_capacity,
                                       const char *delimiter,
                                       const size_t delimiter_capacity,
                                       hps_string_view *text,
                                       hps_string_view *separator)
{
    hps_string_status status = hps_string_view_init_cstr(text, input,
                                                       input_capacity);
    if (status == HPS_STRING_OK) {
        status = hps_string_view_init_cstr(separator, delimiter,
                                          delimiter_capacity);
        if ((status == HPS_STRING_OK) && (separator->length == 0U)) {
            status = HPS_STRING_INVALID_ARGUMENT;
        }
    }
    return status;
}

/* Only initialized, bounded inputs reach this helper. SIZE_MAX is available as
 * an end marker because a C-string length is strictly below its readable bound.
 * A delimiter consumes at least one character; trailing empty fields are emitted
 * once. This common iterator keeps split count, indexed access and export aligned.
 */
static uint8_t next_token(const hps_string_view text,
                          const hps_string_view separator, size_t *position,
                          hps_string_view *result)
{
    uint8_t found = 0U;
    if (*position != SIZE_MAX) {
        const size_t start = *position;
        size_t match = SIZE_MAX;
        if (separator.length <= (text.length - start)) {
            const size_t last = text.length - separator.length;
            size_t candidate = start;
            while ((candidate <= last) && (match == SIZE_MAX)) {
                if (memcmp(&text.data[candidate], separator.data,
                           separator.length) == 0) {
                    match = candidate;
                } else {
                    candidate += 1U;
                }
            }
        }
        const size_t end = (match == SIZE_MAX) ? text.length : match;
        result->length = end - start;
        result->data = (result->length == 0U) ? NULL : &text.data[start];
        *position = (match == SIZE_MAX) ? SIZE_MAX : match + separator.length;
        found = 1U;
    }
    return found;
}

static size_t token_count(const hps_string_view text,
                           const hps_string_view separator)
{
    size_t position = (text.length == 0U) ? SIZE_MAX : 0U;
    size_t count = 0U;
    hps_string_view token = {NULL, 0U};
    while (next_token(text, separator, &position, &token) == 1U) {
        count += 1U;
    }
    return count;
}

hps_string_status hps_cstr_split_count(const char *input,
                                       const size_t input_capacity,
                                       const char *delimiter,
                                       const size_t delimiter_capacity,
                                       size_t *result)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_string_view text = {NULL, 0U};
        hps_string_view separator = {NULL, 0U};
        status = split_inputs(input, input_capacity, delimiter,
                              delimiter_capacity, &text, &separator);
        if (status == HPS_STRING_OK) {
            *result = token_count(text, separator);
        }
    }
    return status;
}

hps_string_status hps_cstr_split_at(const char *input,
                                    const size_t input_capacity,
                                    const char *delimiter,
                                    const size_t delimiter_capacity,
                                    const size_t index, hps_string_view *result)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_string_view text = {NULL, 0U};
        hps_string_view separator = {NULL, 0U};
        status = split_inputs(input, input_capacity, delimiter,
                              delimiter_capacity, &text, &separator);
        if (status == HPS_STRING_OK) {
            size_t position = (text.length == 0U) ? SIZE_MAX : 0U;
            size_t current = 0U;
            hps_string_view token = {NULL, 0U};
            status = HPS_STRING_OUT_OF_RANGE;
            while ((status == HPS_STRING_OUT_OF_RANGE) &&
                   (next_token(text, separator, &position, &token) == 1U)) {
                if (current == index) {
                    *result = token;
                    status = HPS_STRING_OK;
                } else {
                    current += 1U;
                }
            }
        }
    }
    return status;
}

hps_string_status hps_cstr_split(const char *input, const size_t input_capacity,
                                 const char *delimiter,
                                 const size_t delimiter_capacity,
                                 hps_string_view *tokens,
                                 const size_t token_capacity, size_t *written)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if ((written != NULL) && ((tokens != NULL) || (token_capacity == 0U))) {
        hps_string_view text = {NULL, 0U};
        hps_string_view separator = {NULL, 0U};
        status = split_inputs(input, input_capacity, delimiter,
                              delimiter_capacity, &text, &separator);
        if (status == HPS_STRING_OK) {
            const size_t count = token_count(text, separator);
            if (count > token_capacity) {
                status = HPS_STRING_NO_SPACE;
            } else {
                size_t position = (text.length == 0U) ? SIZE_MAX : 0U;
                for (size_t index = 0U; index < count; index += 1U) {
                    (void)next_token(text, separator, &position, &tokens[index]);
                }
                *written = count;
            }
        }
    }
    return status;
}

static hps_string_status reserve_length(size_t *length, const size_t additional)
{
    hps_string_status status = HPS_STRING_OUT_OF_RANGE;
    if (additional <= (SIZE_MAX - *length)) {
        *length += additional;
        status = HPS_STRING_OK;
    }
    return status;
}

static hps_string_status token_valid(const hps_string_view token)
{
    hps_string_status status = hps_string_check_view(token);
    if (status == HPS_STRING_OK) {
        for (size_t index = 0U;
             (index < token.length) && (status == HPS_STRING_OK); index += 1U) {
            if (token.data[index] == '\0') {
                status = HPS_STRING_INVALID_ARGUMENT;
            }
        }
    }
    return status;
}

static void write_view(char *output, size_t *position, const hps_string_view text)
{
    if (text.length > 0U) {
        (void)memcpy(&output[*position], text.data, text.length);
        *position += text.length;
    }
}

hps_string_status hps_cstr_join(char *output, const size_t output_capacity,
                                const hps_string_view *tokens, const size_t count,
                                const char *delimiter,
                                const size_t delimiter_capacity)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if ((output != NULL) && ((tokens != NULL) || (count == 0U))) {
        hps_string_view separator = {NULL, 0U};
        status = hps_string_view_init_cstr(&separator, delimiter,
                                          delimiter_capacity);
        size_t length = 0U;
        for (size_t index = 0U;
             (index < count) && (status == HPS_STRING_OK); index += 1U) {
            status = token_valid(tokens[index]);
            if (status == HPS_STRING_OK) {
                status = reserve_length(&length, tokens[index].length);
                if ((status == HPS_STRING_OK) && (index > 0U)) {
                    status = reserve_length(&length, separator.length);
                }
            }
        }
        if ((status == HPS_STRING_OK) && (length >= output_capacity)) {
            status = HPS_STRING_NO_SPACE;
        }
        if (status == HPS_STRING_OK) {
            size_t position = 0U;
            for (size_t index = 0U; index < count; index += 1U) {
                if (index > 0U) {
                    write_view(output, &position, separator);
                }
                write_view(output, &position, tokens[index]);
            }
            output[position] = '\0';
        }
    }
    return status;
}

hps_string_status hps_cstr_group(char *output, const size_t output_capacity,
                                 const char *input, const size_t input_capacity,
                                 const size_t group_length,
                                 const char *separator,
                                 const size_t separator_capacity)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if ((output != NULL) && (group_length > 0U)) {
        hps_string_view text = {NULL, 0U};
        hps_string_view between = {NULL, 0U};
        status = split_inputs(input, input_capacity, separator,
                              separator_capacity, &text, &between);
        if (status == HPS_STRING_OK) {
            const size_t separators = (text.length == 0U) ?
                0U : (text.length - 1U) / group_length;
            size_t length = text.length;
            if ((separators > 0U) &&
                (between.length > ((SIZE_MAX - length) / separators))) {
                status = HPS_STRING_OUT_OF_RANGE;
            } else {
                length += separators * between.length;
                if (length >= output_capacity) {
                    status = HPS_STRING_NO_SPACE;
                }
            }
            if (status == HPS_STRING_OK) {
                size_t position = 0U;
                for (size_t index = 0U; index < text.length; index += 1U) {
                    if ((index > 0U) &&
                        (((text.length - index) % group_length) == 0U)) {
                        write_view(output, &position, between);
                    }
                    output[position] = text.data[index];
                    position += 1U;
                }
                output[position] = '\0';
            }
        }
    }
    return status;
}

hps_string_status hps_cstr_filename(const char *path, const size_t path_capacity,
                                    const uint8_t remove_extension,
                                    hps_string_view *result)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if ((result != NULL) && (remove_extension <= 1U)) {
        hps_string_view text = {NULL, 0U};
        status = hps_string_view_init_cstr(&text, path, path_capacity);
        if (status == HPS_STRING_OK) {
            size_t start = 0U;
            size_t dot = SIZE_MAX;
            for (size_t index = 0U; index < text.length; index += 1U) {
                if ((text.data[index] == '/') || (text.data[index] == '\\')) {
                    start = index + 1U;
                    dot = SIZE_MAX;
                } else if (text.data[index] == '.') {
                    dot = index;
                } else {
                    /* Other characters are ordinary lexical filename text. */
                }
            }
            const size_t end = ((remove_extension == 1U) &&
                (dot != SIZE_MAX) && (dot != start)) ? dot : text.length;
            status = hps_string_view_slice(text, start, end - start, result);
        }
    }
    return status;
}

hps_string_status hps_cstr_select(const char *left, const size_t left_capacity,
                                  const char *right, const size_t right_capacity,
                                  const uint8_t maximum,
                                  hps_string_view *result)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if ((result != NULL) && (maximum <= 1U)) {
        hps_string_view lhs = {NULL, 0U};
        hps_string_view rhs = {NULL, 0U};
        status = hps_string_view_init_cstr(&lhs, left, left_capacity);
        if (status == HPS_STRING_OK) {
            status = hps_string_view_init_cstr(&rhs, right, right_capacity);
            if (status == HPS_STRING_OK) {
                int32_t comparison = INT32_C(0);
                status = hps_string_view_compare(lhs, rhs, &comparison);
                if (status == HPS_STRING_OK) {
                    const uint8_t select_left = (maximum == 1U) ?
                        (uint8_t)(comparison > INT32_C(0)) :
                        (uint8_t)(comparison < INT32_C(0));
                    *result = (select_left == 1U) ? lhs : rhs;
                }
            }
        }
    }
    return status;
}

hps_string_status hps_cstr_title_value(char *output,
                                       const size_t output_capacity,
                                       const char *title,
                                       const size_t title_capacity,
                                       const char *fill,
                                       const size_t fill_capacity,
                                       const char *value,
                                       const size_t value_capacity,
                                       const size_t width)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if (output != NULL) {
        hps_string_view heading = {NULL, 0U};
        hps_string_view middle = {NULL, 0U};
        hps_string_view text = {NULL, 0U};
        status = hps_string_view_init_cstr(&heading, title, title_capacity);
        if (status == HPS_STRING_OK) {
            status = hps_string_view_init_cstr(&middle, fill, fill_capacity);
        }
        if (status == HPS_STRING_OK) {
            status = hps_string_view_init_cstr(&text, value, value_capacity);
        }
        if (status == HPS_STRING_OK) {
            if ((width >= output_capacity) || (heading.length > width) ||
                (text.length > (width - heading.length))) {
                status = HPS_STRING_NO_SPACE;
            } else {
                const size_t budget = width - heading.length - text.length;
                const size_t repetitions = (middle.length == 0U) ?
                    0U : budget / middle.length;
                size_t position = 0U;
                write_view(output, &position, heading);
                for (size_t index = 0U; index < repetitions; index += 1U) {
                    write_view(output, &position, middle);
                }
                write_view(output, &position, text);
                output[position] = '\0';
            }
        }
    }
    return status;
}

hps_string_status hps_cstr_dump_i64(char *output, const size_t output_capacity,
                                    const char *field,
                                    const size_t field_capacity,
                                    const int64_t value)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if (output != NULL) {
        hps_string_view name = {NULL, 0U};
        status = hps_string_view_init_cstr(&name, field, field_capacity);
        if (status == HPS_STRING_OK) {
            char number[21];
            status = hps_cstr_render_i64(number, sizeof(number), value);
            if (status == HPS_STRING_OK) {
                hps_string_view decimal = {NULL, 0U};
                status = hps_string_view_init_cstr(&decimal, number,
                                                  sizeof(number));
                if (status == HPS_STRING_OK) {
                    const hps_string_view parts[4] = {
                        name, {": ", 2U}, decimal, {"\n", 1U}
                    };
                    status = hps_cstr_join(output, output_capacity, parts, 4U,
                                           "", 1U);
                }
            }
        }
    }
    return status;
}

hps_string_status hps_cstr_dump_bytes(char *output, const size_t output_capacity,
                                      const char *field,
                                      const size_t field_capacity,
                                      const uint8_t *data, const size_t length)
{
    hps_string_status status = HPS_STRING_INVALID_ARGUMENT;
    if ((output != NULL) && ((data != NULL) || (length == 0U))) {
        hps_string_view name = {NULL, 0U};
        status = hps_string_view_init_cstr(&name, field, field_capacity);
        if (status == HPS_STRING_OK) {
            size_t required = name.length;
            status = reserve_length(&required, 10U);
            if (status == HPS_STRING_OK) {
                if (length > ((SIZE_MAX - required) / 4U)) {
                    status = HPS_STRING_OUT_OF_RANGE;
                } else {
                    required += length * 4U;
                    if (required >= output_capacity) {
                        status = HPS_STRING_NO_SPACE;
                    }
                }
            }
            if (status == HPS_STRING_OK) {
                const char *printable =
                    " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
                const char *hex = "0123456789ABCDEF";
                size_t position = 0U;
                write_view(output, &position, name);
                const hps_string_view punctuation = {": ", 2U};
                write_view(output, &position, punctuation);
                for (size_t index = 0U; index < length; index += 1U) {
                    output[position] = ((data[index] >= 32U) &&
                                        (data[index] <= 126U)) ?
                        printable[data[index] - 32U] : '.';
                    position += 1U;
                }
                const hps_string_view label = {" | HEX:", 7U};
                write_view(output, &position, label);
                for (size_t index = 0U; index < length; index += 1U) {
                    output[position] = ' ';
                    output[position + 1U] = hex[data[index] >> 4U];
                    output[position + 2U] = hex[data[index] & 0x0FU];
                    position += 3U;
                }
                output[position] = '\n';
                output[position + 1U] = '\0';
            }
        }
    }
    return status;
}
