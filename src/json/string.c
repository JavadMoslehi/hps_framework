#include "hps/json/hps_tokenizer.h"
#include "internal.h"

/**
 * @file src/json/string.c
 * @brief Shared strict UTF-8 and JSON escape scanning with counted byte output.
 */

static uint8_t hex_digit(const uint8_t value)
{
    uint8_t result = UINT8_MAX;
    if ((value >= 0x30U) && (value <= 0x39U)) {
        result = (uint8_t)(value - 0x30U);
    } else if ((value >= 0x41U) && (value <= 0x46U)) {
        result = (uint8_t)(value - 0x41U + 10U);
    } else if ((value >= 0x61U) && (value <= 0x66U)) {
        result = (uint8_t)(value - 0x61U + 10U);
    } else {
        /* Non-hex input keeps the invalid sentinel. */
    }
    return result;
}

static int32_t read_hex4(const hps_string_view input, size_t *position,
                            uint32_t *value)
{
    int32_t status = HPS_JSON_SYNTAX;
    if ((input.length - *position) >= 4U) {
        uint32_t scalar = UINT32_C(0);
        status = HPS_JSON_OK;
        for (size_t index = 0U;
             (index < 4U) && (status == HPS_JSON_OK); index += 1U) {
            const uint8_t digit = hex_digit((uint8_t)input.data[*position]);
            if (digit == UINT8_MAX) {
                status = HPS_JSON_SYNTAX;
            } else {
                scalar = scalar * UINT32_C(16) + (uint32_t)digit;
                *position += 1U;
            }
        }
        if (status == HPS_JSON_OK) {
            *value = scalar;
        }
    }
    return status;
}

static int32_t read_escape(const hps_string_view input, size_t *position,
                              uint32_t *value)
{
    int32_t status = HPS_JSON_SYNTAX;
    if (*position < input.length) {
        const uint8_t escaped = (uint8_t)input.data[*position];
        *position += 1U;
        status = HPS_JSON_OK;
        switch (escaped) {
            case 0x22U: *value = UINT32_C(0x22); break;
            case 0x5CU: *value = UINT32_C(0x5C); break;
            case 0x2FU: *value = UINT32_C(0x2F); break;
            case 0x62U: *value = UINT32_C(8); break;
            case 0x66U: *value = UINT32_C(12); break;
            case 0x6EU: *value = UINT32_C(10); break;
            case 0x72U: *value = UINT32_C(13); break;
            case 0x74U: *value = UINT32_C(9); break;
            case 0x75U:
                status = read_hex4(input, position, value);
                if ((status == HPS_JSON_OK) &&
                    (*value >= UINT32_C(0xD800)) &&
                    (*value <= UINT32_C(0xDBFF))) {
                    const uint32_t high = *value;
                    if (((input.length - *position) < 6U) ||
                        (input.data[*position] != '\\') ||
                        (input.data[*position + 1U] != 'u')) {
                        status = HPS_JSON_SYNTAX;
                    } else {
                        *position += 2U;
                        status = read_hex4(input, position, value);
                        if ((status == HPS_JSON_OK) &&
                            (*value >= UINT32_C(0xDC00)) &&
                            (*value <= UINT32_C(0xDFFF))) {
                            *value = UINT32_C(0x10000) +
                                (high - UINT32_C(0xD800)) * UINT32_C(1024) +
                                (*value - UINT32_C(0xDC00));
                        } else {
                            status = HPS_JSON_SYNTAX;
                        }
                    }
                } else if ((status == HPS_JSON_OK) &&
                           (*value >= UINT32_C(0xDC00)) &&
                           (*value <= UINT32_C(0xDFFF))) {
                    status = HPS_JSON_SYNTAX;
                } else {
                    /* Other BMP scalars, including U+0000, are valid. */
                }
                break;
            default: status = HPS_JSON_SYNTAX; break;
        }
    }
    return status;
}

static int32_t read_utf8(const hps_string_view input, size_t *position,
                            uint32_t *value)
{
    const uint8_t first = (uint8_t)input.data[*position];
    size_t width = 0U;
    uint32_t scalar = UINT32_C(0);
    uint32_t minimum = UINT32_C(0);
    int32_t status = HPS_JSON_OK;
    if ((first >= 0xC2U) && (first <= 0xDFU)) {
        width = 2U;
        scalar = (uint32_t)(first & 0x1FU);
        minimum = UINT32_C(0x80);
    } else if ((first >= 0xE0U) && (first <= 0xEFU)) {
        width = 3U;
        scalar = (uint32_t)(first & 0x0FU);
        minimum = UINT32_C(0x800);
    } else if ((first >= 0xF0U) && (first <= 0xF4U)) {
        width = 4U;
        scalar = (uint32_t)(first & 0x07U);
        minimum = UINT32_C(0x10000);
    } else {
        status = HPS_JSON_SYNTAX;
    }
    if ((status == HPS_JSON_OK) && ((input.length - *position) < width)) {
        status = HPS_JSON_SYNTAX;
    }
    for (size_t index = 1U;
         (index < width) && (status == HPS_JSON_OK); index += 1U) {
        const uint8_t next = (uint8_t)input.data[*position + index];
        if ((next < 0x80U) || (next > 0xBFU)) {
            status = HPS_JSON_SYNTAX;
        } else {
            scalar = scalar * UINT32_C(64) + (uint32_t)(next & 0x3FU);
        }
    }
    if ((status == HPS_JSON_OK) &&
        ((scalar < minimum) || (scalar > UINT32_C(0x10FFFF)) ||
         ((scalar >= UINT32_C(0xD800)) && (scalar <= UINT32_C(0xDFFF))))) {
        status = HPS_JSON_SYNTAX;
    }
    if (status == HPS_JSON_OK) {
        *position += width;
        *value = scalar;
    }
    return status;
}

static int32_t emit_scalar(const uint32_t scalar, uint8_t *output,
                              size_t *count, const size_t bound)
{
    size_t width = 1U;
    if (scalar > UINT32_C(0xFFFF)) {
        width = 4U;
    } else if (scalar > UINT32_C(0x7FF)) {
        width = 3U;
    } else if (scalar > UINT32_C(0x7F)) {
        width = 2U;
    } else {
        /* ASCII occupies one output octet, including NUL. */
    }
    int32_t status = HPS_JSON_OUT_OF_RANGE;
    if (width <= bound - *count) {
        if (output != NULL) {
            if (width == 1U) {
                output[*count] = (uint8_t)scalar;
            } else {
                const uint8_t prefix = (width == 2U) ? 0xC0U :
                    ((width == 3U) ? 0xE0U : 0xF0U);
                const uint32_t shift = (uint32_t)((width - 1U) * 6U);
                output[*count] = (uint8_t)((uint32_t)prefix | (scalar >> shift));
                for (size_t index = 1U; index < width; index += 1U) {
                    const uint32_t tail_shift = (uint32_t)((width - index - 1U) * 6U);
                    output[*count + index] = (uint8_t)(UINT32_C(0x80) |
                        ((scalar >> tail_shift) & UINT32_C(0x3F)));
                }
            }
        }
        *count += width;
        status = HPS_JSON_OK;
    }
    return status;
}

int32_t hps_json_scan_string(const hps_string_view input, const size_t start,
                                size_t *end, uint8_t *output, size_t *written)
{
    size_t position = start + 1U;
    size_t count = 0U;
    uint8_t complete = 0U;
    int32_t status = HPS_JSON_OK;
    while ((position < input.length) && (complete == 0U) &&
           (status == HPS_JSON_OK)) {
        const uint8_t byte = (uint8_t)input.data[position];
        uint32_t scalar = (uint32_t)byte;
        if (byte == 0x22U) {
            position += 1U;
            complete = 1U;
        } else {
            if (byte == 0x5CU) {
                position += 1U;
                status = read_escape(input, &position, &scalar);
            } else if (byte < 0x20U) {
                status = HPS_JSON_SYNTAX;
            } else if (byte >= 0x80U) {
                status = read_utf8(input, &position, &scalar);
            } else {
                position += 1U;
            }
            if (status == HPS_JSON_OK) {
                status = emit_scalar(scalar, output, &count, input.length);
            }
        }
    }
    if ((status == HPS_JSON_OK) && (complete == 0U)) {
        status = HPS_JSON_SYNTAX;
    }
    if (status == HPS_JSON_OK) {
        *end = position;
        *written = count;
    }
    return status;
}
