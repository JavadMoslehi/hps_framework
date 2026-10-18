#include "hps/cstr/format.h"
#include "../string/internal.h"
#include <string.h>

/**
 * @file src/cstr/format.c
 * @brief Two-pass typed formatting with caller-owned staging and exact decimals.
 */

/* At most 64 binary digits, 20 decimal digits, or 21 fixed-decimal characters.
 * The largest selected grammar number is decimal/hex; 80 also bounds helpers. */
#define FORMAT_NUMBER_CAPACITY ((size_t)80U)

static uint64_t format_magnitude(const int64_t value)
{
    return (value < INT64_C(0)) ?
        (uint64_t)(-(value + INT64_C(1))) + UINT64_C(1) : (uint64_t)value;
}

static size_t format_unsigned(char *output, uint64_t value,
                              const uint8_t radix, const uint8_t uppercase)
{
    const char *digits = (uppercase == 1U) ? "0123456789ABCDEF" : "0123456789abcdef";
    char reversed[FORMAT_NUMBER_CAPACITY];
    size_t length = 0U;
    size_t index;
    do {
        const size_t digit = (size_t)(value % (uint64_t)radix);
        reversed[length] = digits[digit];
        length += 1U;
        value /= (uint64_t)radix;
    } while (value != UINT64_C(0));
    for (index = 0U; index < length; index += 1U) {
        output[index] = reversed[length - index - 1U];
    }
    return length;
}

static size_t format_decimal(char *output, const int64_t coefficient,
                             const uint8_t scale)
{
    char digits[FORMAT_NUMBER_CAPACITY];
    const size_t digit_count = format_unsigned(digits, format_magnitude(coefficient), 10U, 0U);
    const size_t decimal_places = (size_t)scale;
    size_t length = 0U;
    size_t index = 0U;
    if (coefficient < INT64_C(0)) {
        output[length] = '-';
        length += 1U;
    }
    if (decimal_places == 0U) {
        (void)memcpy(&output[length], digits, digit_count);
        length += digit_count;
    } else if (digit_count <= decimal_places) {
        output[length] = '0';
        output[length + 1U] = '.';
        length += 2U;
        for (index = digit_count; index < decimal_places; index += 1U) {
            output[length] = '0';
            length += 1U;
        }
        (void)memcpy(&output[length], digits, digit_count);
        length += digit_count;
    } else {
        const size_t whole = digit_count - decimal_places;
        (void)memcpy(&output[length], digits, whole);
        length += whole;
        output[length] = '.';
        length += 1U;
        (void)memcpy(&output[length], &digits[whole], decimal_places);
        length += decimal_places;
    }
    return length;
}

int32_t hps_cstr_render_decimal(hps_string_buffer *destination,
                                const int64_t coefficient,
                                const uint8_t scale)
{
    size_t remaining = 0U;
    int32_t status = hps_string_buffer_remaining(destination, &remaining);
    if (status == HPS_STRING_OK) {
        if (scale > HPS_CSTR_DECIMAL_MAX_SCALE) {
            status = HPS_STRING_INVALID_ARGUMENT;
        } else {
            char output[FORMAT_NUMBER_CAPACITY];
            const size_t length = format_decimal(output, coefficient, scale);
            const hps_string_view text = {output, length};
            status = hps_string_buffer_assign(destination, text);
        }
    }
    return status;
}

/* Convert one validated token to a borrowed piece or bounded automatic number.
 * The token points into format storage, avoiding scalar char representation. */
static int32_t format_argument(const char *token,
                               const hps_cstr_format_arg *argument,
                               char *number, hps_string_view *piece)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    size_t length = 0U;
    if ((*token == 's') || (*token == 'c')) {
        if (argument->type == HPS_CSTR_ARG_TEXT) {
            status = hps_string_check_view(argument->text);
            if (status == HPS_STRING_OK) {
                if ((hps_string_has_nul(argument->text) == 1U) ||
                    ((*token == 'c') && (argument->text.length != 1U))) {
                    status = HPS_STRING_INVALID_ARGUMENT;
                } else {
                    *piece = argument->text;
                }
            }
        }
    } else if ((*token == 'd') || (*token == 'i')) {
        if (argument->type == HPS_CSTR_ARG_I64) {
            length = format_decimal(number, argument->signed_value, 0U);
            status = HPS_STRING_OK;
        }
    } else if ((*token == 'u') || (*token == 'x') || (*token == 'X')) {
        if (argument->type == HPS_CSTR_ARG_U64) {
            const uint8_t radix = (*token == 'u') ? 10U : 16U;
            length = format_unsigned(number, argument->unsigned_value, radix,
                                     (uint8_t)(*token == 'X'));
            status = HPS_STRING_OK;
        }
    } else if (*token == 'b') {
        if ((argument->type == HPS_CSTR_ARG_BOOL) && (argument->flag <= 1U)) {
            piece->data = (argument->flag == 1U) ? "true" : "false";
            piece->length = (argument->flag == 1U) ? 4U : 5U;
            status = HPS_STRING_OK;
        }
    } else if (*token == 'f') {
        if ((argument->type == HPS_CSTR_ARG_DECIMAL) &&
            (argument->scale <= HPS_CSTR_DECIMAL_MAX_SCALE)) {
            length = format_decimal(number, argument->signed_value, argument->scale);
            status = HPS_STRING_OK;
        }
    } else {
        /* Unknown tokens remain invalid, including %n and format widths. */
    }
    if ((status == HPS_STRING_OK) && (length != 0U)) {
        piece->data = number;
        piece->length = length;
    }
    return status;
}

/* The same parser drives preflight and emission; output==NULL is a read-only
 * sizing pass. Emission follows an entirely successful pass with unchanged input.
 * Caller disjointness/serialization prevents staged writes invalidating sources. */
static int32_t format_pass(const hps_string_view format,
                           const hps_cstr_format_arg *arguments,
                           const size_t argument_count,
                           char *output, size_t *result_length)
{
    int32_t status = HPS_STRING_OK;
    size_t position = 0U;
    size_t argument_index = 0U;
    size_t length = 0U;
    while ((position < format.length) && (status == HPS_STRING_OK)) {
        char number[FORMAT_NUMBER_CAPACITY];
        hps_string_view piece = {&format.data[position], 1U};
        if (format.data[position] == '%') {
            position += 1U;
            if (position == format.length) {
                status = HPS_STRING_INVALID_ARGUMENT;
            } else if (format.data[position] == '%') {
                piece.data = &format.data[position];
                position += 1U;
            } else {
                uint8_t length_modifier = 0U;
                if (format.data[position] == 'l') {
                    length_modifier = 1U;
                    position += 1U;
                    if ((position < format.length) && (format.data[position] == 'l')) {
                        position += 1U;
                    }
                }
                if ((position == format.length) || (argument_index == argument_count)) {
                    status = HPS_STRING_INVALID_ARGUMENT;
                } else {
                    const char *token = &format.data[position];
                    if ((length_modifier == 1U) && (*token != 'd') && (*token != 'i') &&
                        (*token != 'u') && (*token != 'x') && (*token != 'X')) {
                        status = HPS_STRING_INVALID_ARGUMENT;
                    } else {
                        status = format_argument(token, &arguments[argument_index], number, &piece);
                    }
                    argument_index += 1U;
                    position += 1U;
                }
            }
        } else {
            position += 1U;
        }
        if (status == HPS_STRING_OK) {
            if (piece.length > (SIZE_MAX - length)) {
                status = HPS_STRING_NO_SPACE;
            } else {
                if ((output != NULL) && (piece.length != 0U)) {
                    (void)memcpy(&output[length], piece.data, piece.length);
                }
                length += piece.length;
            }
        }
    }
    if ((status == HPS_STRING_OK) && (argument_index != argument_count)) {
        status = HPS_STRING_INVALID_ARGUMENT;
    }
    if (status == HPS_STRING_OK) {
        *result_length = length;
    }
    return status;
}

static int32_t format_write(hps_string_buffer *destination,
                            char *scratch, const size_t scratch_capacity,
                            const char *format, const size_t format_capacity,
                            const hps_cstr_format_arg *arguments,
                            const size_t argument_count, const uint8_t append)
{
    size_t remaining = 0U;
    int32_t status = hps_string_buffer_remaining(destination, &remaining);
    if (status == HPS_STRING_OK) {
        if ((scratch == NULL) || (scratch_capacity == 0U) ||
            ((arguments == NULL) && (argument_count != 0U)) ||
            (argument_count == SIZE_MAX)) {
            status = HPS_STRING_INVALID_ARGUMENT;
        } else {
            hps_string_view pattern = {NULL, 0U};
            status = hps_string_view_init_cstr(&pattern, format, format_capacity);
            if (status == HPS_STRING_OK) {
                size_t length = 0U;
                status = format_pass(pattern, arguments, argument_count, NULL, &length);
                if (status == HPS_STRING_OK) {
                    const size_t available = (append == 1U) ? remaining : destination->capacity - 1U;
                    if ((length > available) || (length >= scratch_capacity)) {
                        status = HPS_STRING_NO_SPACE;
                    } else {
                        size_t emitted = 0U;
                        status = format_pass(pattern, arguments, argument_count, scratch, &emitted);
                        if (status == HPS_STRING_OK) {
                            const hps_string_view text = {scratch, emitted};
                            scratch[emitted] = '\0';
                            status = (append == 1U) ? hps_string_buffer_append(destination, text) :
                                hps_string_buffer_assign(destination, text);
                        }
                    }
                }
            }
        }
    }
    return status;
}

int32_t hps_cstr_format(hps_string_buffer *destination,
                        char *scratch, const size_t scratch_capacity,
                        const char *format, const size_t format_capacity,
                        const hps_cstr_format_arg *arguments,
                        const size_t argument_count)
{
    return format_write(destination, scratch, scratch_capacity, format, format_capacity,
                        arguments, argument_count, 0U);
}

int32_t hps_cstr_append_format(hps_string_buffer *destination,
                               char *scratch, const size_t scratch_capacity,
                               const char *format, const size_t format_capacity,
                               const hps_cstr_format_arg *arguments,
                               const size_t argument_count)
{
    return format_write(destination, scratch, scratch_capacity, format, format_capacity,
                        arguments, argument_count, 1U);
}
