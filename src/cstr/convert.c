#include "hps/cstr/convert.h"

#include <string.h>

/**
 * @file src/cstr/convert.c
 * @brief Checked integer arithmetic and complete bounded byte conversions.
 */

/* Match literal digits rather than assuming contiguous alphabet encodings. */
static uint8_t digit_value(const char *character)
{
    const char *upper = "0123456789ABCDEF";
    const char *lower = "abcdef";
    uint8_t result = UINT8_MAX;
    for (uint8_t index = 0U; index < 16U; index += 1U) {
        if ((*character == upper[index]) ||
            ((index >= 10U) && (*character == lower[index - 10U]))) {
            result = index;
        }
    }
    return result;
}

static uint8_t radix_valid(const uint8_t radix)
{
    return (uint8_t)((radix == 2U) || (radix == 8U) ||
                     (radix == 10U) || (radix == 16U));
}

static int32_t parse_digits(const hps_string_view text,
                            const uint8_t radix,
                            const uint64_t limit, uint64_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    uint64_t value = UINT64_C(0);
    if (text.length > 0U) {
        status = HPS_STRING_OK;
        for (size_t index = 0U;
             (index < text.length) && (status == HPS_STRING_OK); index += 1U) {
            const uint8_t digit = digit_value(&text.data[index]);
            if (digit >= radix) {
                status = HPS_STRING_INVALID_ARGUMENT;
            } else if (value > (limit - (uint64_t)digit) / (uint64_t)radix) {
                status = HPS_STRING_OUT_OF_RANGE;
            } else {
                value = value * (uint64_t)radix + (uint64_t)digit;
            }
        }
        if (status == HPS_STRING_OK) {
            *result = value;
        }
    }
    return status;
}

int32_t hps_cstr_parse_u64(const char *input,
                           const size_t input_capacity,
                           const uint8_t radix, uint64_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((result != NULL) && (radix_valid(radix) == 1U)) {
        hps_string_view text = {NULL, 0U};
        status = hps_string_view_init_cstr(&text, input, input_capacity);
        if (status == HPS_STRING_OK) {
            status = parse_digits(text, radix, UINT64_MAX, result);
        }
    }
    return status;
}

int32_t hps_cstr_parse_i64(const char *input,
                           const size_t input_capacity,
                           int64_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_string_view text = {NULL, 0U};
        status = hps_string_view_init_cstr(&text, input, input_capacity);
        if (status == HPS_STRING_OK) {
            uint8_t negative = 0U;
            if ((text.length > 0U) &&
                ((text.data[0] == '-') || (text.data[0] == '+'))) {
                negative = (uint8_t)(text.data[0] == '-');
                text.length -= 1U;
                text.data = (text.length == 0U) ? NULL : &input[1];
            }
            uint64_t magnitude = UINT64_C(0);
            const uint64_t limit = (uint64_t)INT64_MAX + (uint64_t)negative;
            status = parse_digits(text, 10U, limit, &magnitude);
            if (status == HPS_STRING_OK) {
                if (negative == 0U) {
                    *result = (int64_t)magnitude;
                } else if (magnitude == ((uint64_t)INT64_MAX + UINT64_C(1))) {
                    *result = INT64_MIN;
                } else {
                    *result = -(int64_t)magnitude;
                }
            }
        }
    }
    return status;
}

/* Private sign selection is 0/1. Unsigned division removes at least one digit
 * per iteration, so 64 binary digits bound both work and temporary storage. */
static int32_t render_integer(char *output, const size_t capacity,
                              uint64_t magnitude,
                              const uint8_t radix,
                              const uint8_t negative)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((output != NULL) && (radix_valid(radix) == 1U)) {
        char reverse[65];
        const char *digits = "0123456789ABCDEF";
        size_t length = 0U;
        do {
            reverse[length] = digits[magnitude % (uint64_t)radix];
            length += 1U;
            magnitude /= (uint64_t)radix;
        } while (magnitude != UINT64_C(0));
        reverse[length] = '\0';
        const size_t required = length + (size_t)negative;
        status = HPS_STRING_NO_SPACE;
        if (required < capacity) {
            size_t offset = 0U;
            if (negative == 1U) {
                output[offset] = '-';
                offset += 1U;
            }
            for (size_t index = 0U; index < length; index += 1U) {
                output[offset + index] = reverse[length - index - 1U];
            }
            output[required] = '\0';
            status = HPS_STRING_OK;
        }
    }
    return status;
}

int32_t hps_cstr_render_u64(char *output,
                            const size_t output_capacity,
                            const uint64_t value,
                            const uint8_t radix)
{
    return render_integer(output, output_capacity, value, radix, 0U);
}

int32_t hps_cstr_render_i64(char *output,
                            const size_t output_capacity,
                            const int64_t value)
{
    const uint8_t negative = (uint8_t)(value < INT64_C(0));
    /* Adding one before negation makes INT64_MIN safe; restoring one unsigned
     * afterwards yields a magnitude at most INT64_MAX+1. */
    const uint64_t magnitude = (negative == 1U) ?
        (uint64_t)(-(value + INT64_C(1))) + UINT64_C(1) : (uint64_t)value;
    return render_integer(output, output_capacity, magnitude, 10U, negative);
}

static size_t byte_width(const uint8_t value, const uint8_t format)
{
    size_t width = 0U;
    switch (format) {
        case HPS_CSTR_BYTES_HEX_COMPACT:
        case HPS_CSTR_BYTES_HEX_SPACED:
            width = 2U;
            break;
        case HPS_CSTR_BYTES_HEX_PREFIXED:
            width = 4U;
            break;
        case HPS_CSTR_BYTES_BINARY_COMPACT:
        case HPS_CSTR_BYTES_BINARY_GROUPED:
            width = 8U;
            break;
        case HPS_CSTR_BYTES_DECIMAL_CSV:
        case HPS_CSTR_BYTES_DECIMAL_BRACKETS:
            width = (value < 10U) ? 1U : ((value < 100U) ? 2U : 3U);
            break;
        case HPS_CSTR_BYTES_TEXT:
            width = 1U;
            break;
        default:
            break;
    }
    return width;
}

static size_t separator_width(const uint8_t format)
{
    size_t width = 0U;
    if ((format == HPS_CSTR_BYTES_HEX_SPACED) ||
        (format == HPS_CSTR_BYTES_HEX_PREFIXED) ||
        (format == HPS_CSTR_BYTES_BINARY_GROUPED) ||
        (format == HPS_CSTR_BYTES_DECIMAL_CSV)) {
        width = 1U;
    } else if (format == HPS_CSTR_BYTES_DECIMAL_BRACKETS) {
        width = 2U;
    } else {
        /* Compact hex/binary and text have no separator. */
    }
    return width;
}

/* Caller preflight proves the complete write extent. Digits are selected from
 * immutable execution-character-set literals, not cast from raw byte values. */
static size_t write_byte(char *output, const uint8_t value, const uint8_t format)
{
    const char *digits = "0123456789ABCDEF";
    size_t count = 0U;
    if ((format == HPS_CSTR_BYTES_HEX_COMPACT) ||
        (format == HPS_CSTR_BYTES_HEX_SPACED) ||
        (format == HPS_CSTR_BYTES_HEX_PREFIXED)) {
        if (format == HPS_CSTR_BYTES_HEX_PREFIXED) {
            output[count] = '0';
            output[count + 1U] = 'x';
            count += 2U;
        }
        output[count] = digits[(uint8_t)(value >> 4U)];
        output[count + 1U] = digits[value & 0x0FU];
        count += 2U;
    } else if ((format == HPS_CSTR_BYTES_BINARY_COMPACT) ||
               (format == HPS_CSTR_BYTES_BINARY_GROUPED)) {
        for (uint8_t bit = 0U; bit < 8U; bit += 1U) {
            output[bit] = ((value & (uint8_t)(UINT8_C(1) << (7U - bit))) != 0U) ?
                '1' : '0';
        }
        count = 8U;
    } else {
        const size_t width = byte_width(value, format);
        uint16_t remaining = (uint16_t)value;
        for (size_t index = width; index > 0U; index -= 1U) {
            output[index - 1U] = digits[remaining % 10U];
            remaining /= 10U;
        }
        count = width;
    }
    return count;
}

int32_t hps_cstr_encode_bytes(char *output,
                              const size_t output_capacity,
                              const uint8_t *data, const size_t length,
                              const uint8_t format)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((output != NULL) && (format <= HPS_CSTR_BYTES_TEXT) &&
        ((data != NULL) || (length == 0U))) {
        const size_t separator = separator_width(format);
        size_t required = (format == HPS_CSTR_BYTES_DECIMAL_BRACKETS) ? 2U : 0U;
        status = HPS_STRING_OK;
        for (size_t index = 0U;
             (index < length) && (status == HPS_STRING_OK); index += 1U) {
            const size_t width = byte_width(data[index], format) +
                ((index == 0U) ? 0U : separator);
            if ((format == HPS_CSTR_BYTES_TEXT) && (data[index] == 0U)) {
                status = HPS_STRING_INVALID_ARGUMENT;
            } else if (width > (SIZE_MAX - required)) {
                status = HPS_STRING_OUT_OF_RANGE;
            } else {
                required += width;
            }
        }
        if ((status == HPS_STRING_OK) && (required >= output_capacity)) {
            status = HPS_STRING_NO_SPACE;
        }
        if (status == HPS_STRING_OK) {
            if (format == HPS_CSTR_BYTES_TEXT) {
                if (length > 0U) {
                    (void)memmove(output, data, length);
                }
            } else {
                size_t position = 0U;
                if (format == HPS_CSTR_BYTES_DECIMAL_BRACKETS) {
                    output[position] = '[';
                    position += 1U;
                }
                for (size_t index = 0U; index < length; index += 1U) {
                    if ((index > 0U) && (separator > 0U)) {
                        output[position] =
                            ((format == HPS_CSTR_BYTES_HEX_SPACED) ||
                             (format == HPS_CSTR_BYTES_BINARY_GROUPED)) ? ' ' : ',';
                        position += 1U;
                        if (separator == 2U) {
                            output[position] = ' ';
                            position += 1U;
                        }
                    }
                    position += write_byte(&output[position], data[index], format);
                }
                if (format == HPS_CSTR_BYTES_DECIMAL_BRACKETS) {
                    output[position] = ']';
                }
            }
            output[required] = '\0';
        }
    }
    return status;
}

int32_t hps_cstr_decode_bytes(uint8_t *output,
                              const size_t output_capacity,
                              const char *input,
                              const size_t input_capacity,
                              const uint8_t format, size_t *written)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((written != NULL) && ((output != NULL) || (output_capacity == 0U)) &&
        ((format == HPS_CSTR_BYTES_HEX_COMPACT) ||
         (format == HPS_CSTR_BYTES_BINARY_COMPACT) ||
         (format == HPS_CSTR_BYTES_TEXT))) {
        hps_string_view text = {NULL, 0U};
        status = hps_string_view_init_cstr(&text, input, input_capacity);
        if (status == HPS_STRING_OK) {
            size_t required = text.length;
            uint8_t radix = 0U;
            if (format == HPS_CSTR_BYTES_HEX_COMPACT) {
                required = text.length / 2U + text.length % 2U;
                radix = 16U;
            } else if (format == HPS_CSTR_BYTES_BINARY_COMPACT) {
                required = text.length / 8U;
                radix = 2U;
                if ((text.length % 8U) != 0U) {
                    status = HPS_STRING_INVALID_ARGUMENT;
                }
            } else {
                /* TEXT is a representation-preserving copy. */
            }
            if (radix != 0U) {
                for (size_t index = 0U;
                     (index < text.length) && (status == HPS_STRING_OK);
                     index += 1U) {
                    if (digit_value(&text.data[index]) >= radix) {
                        status = HPS_STRING_INVALID_ARGUMENT;
                    }
                }
            }
            if ((status == HPS_STRING_OK) && (required > output_capacity)) {
                status = HPS_STRING_NO_SPACE;
            }
            if ((status == HPS_STRING_OK) && (required > 0U) && (output == NULL)) {
                status = HPS_STRING_INVALID_ARGUMENT;
            }
            if (status == HPS_STRING_OK) {
                if (format == HPS_CSTR_BYTES_TEXT) {
                    if (required > 0U) {
                        (void)memmove(output, text.data, required);
                    }
                } else {
                    const size_t group = (radix == 16U) ? 2U : 8U;
                    size_t source = 0U;
                    for (size_t index = 0U; index < required; index += 1U) {
                        const size_t digits =
                            ((index == 0U) && (radix == 16U) &&
                             ((text.length % 2U) != 0U)) ? 1U : group;
                        uint16_t value = 0U;
                        for (size_t digit = 0U; digit < digits; digit += 1U) {
                            value = (uint16_t)(value * (uint16_t)radix +
                                (uint16_t)digit_value(&text.data[source]));
                            source += 1U;
                        }
                        output[index] = (uint8_t)value;
                    }
                }
                *written = required;
            }
        }
    }
    return status;
}

int32_t hps_cstr_invert_binary(char *output,
                               const size_t output_capacity,
                               const char *input,
                               const size_t input_capacity,
                               const uint64_t inverse_count)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (output != NULL) {
        hps_string_view text = {NULL, 0U};
        status = hps_string_view_init_cstr(&text, input, input_capacity);
        if (status == HPS_STRING_OK) {
            if (text.length == 0U) {
                status = HPS_STRING_INVALID_ARGUMENT;
            }
            for (size_t index = 0U;
                 (index < text.length) && (status == HPS_STRING_OK); index += 1U) {
                if ((text.data[index] != '0') && (text.data[index] != '1')) {
                    status = HPS_STRING_INVALID_ARGUMENT;
                }
            }
            if ((status == HPS_STRING_OK) && (text.length >= output_capacity)) {
                status = HPS_STRING_NO_SPACE;
            }
            if (status == HPS_STRING_OK) {
                if ((inverse_count % UINT64_C(2)) == UINT64_C(0)) {
                    (void)memmove(output, input, text.length + 1U);
                } else {
                    for (size_t index = 0U; index < text.length; index += 1U) {
                        output[index] = (input[index] == '0') ? '1' : '0';
                    }
                    output[text.length] = '\0';
                }
            }
        }
    }
    return status;
}

int32_t hps_cstr_boolalpha(const uint8_t flag, const char **result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((result != NULL) && (flag <= 1U)) {
        *result = (flag == 1U) ? "true" : "false";
        status = HPS_STRING_OK;
    }
    return status;
}
