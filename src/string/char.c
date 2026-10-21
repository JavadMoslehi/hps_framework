#include "hps/string/hps_char.h"

/**
 * @file src/string/char.c
 * @brief Explicit ASCII-octet predicates, folding and hexadecimal mapping.
 */

uint8_t hps_char_is_binary(const uint8_t value)
{
    return (uint8_t)((value == 48U) || (value == 49U));
}

uint8_t hps_char_is_octal(const uint8_t value)
{
    return (uint8_t)((value >= 48U) && (value <= 55U));
}

uint8_t hps_char_is_decimal(const uint8_t value)
{
    return (uint8_t)((value >= 48U) && (value <= 57U));
}

uint8_t hps_char_is_hex_lower(const uint8_t value)
{
    return (uint8_t)((value >= 97U) && (value <= 102U));
}

uint8_t hps_char_is_hex_upper(const uint8_t value)
{
    return (uint8_t)((value >= 65U) && (value <= 70U));
}

uint8_t hps_char_is_hex(const uint8_t value)
{
    return (uint8_t)((hps_char_is_decimal(value) == 1U) || (hps_char_is_hex_lower(value) == 1U) || (hps_char_is_hex_upper(value) == 1U));
}

uint8_t hps_char_is_alpha_lower(const uint8_t value)
{
    return (uint8_t)((value >= 97U) && (value <= 122U));
}

uint8_t hps_char_is_alpha_upper(const uint8_t value)
{
    return (uint8_t)((value >= 65U) && (value <= 90U));
}

uint8_t hps_char_is_alpha(const uint8_t value)
{
    return (uint8_t)((hps_char_is_alpha_lower(value) == 1U) || (hps_char_is_alpha_upper(value) == 1U));
}

uint8_t hps_char_is_alphanumeric(const uint8_t value)
{
    return (uint8_t)((hps_char_is_alpha(value) == 1U) || (hps_char_is_decimal(value) == 1U));
}

uint8_t hps_char_is_whitespace(const uint8_t value)
{
    return (uint8_t)((value == 32U) || ((value >= 9U) && (value <= 13U)));
}

uint8_t hps_char_is_printable(const uint8_t value)
{
    return (uint8_t)((value >= 32U) && (value <= 126U));
}

uint8_t hps_char_is_control(const uint8_t value)
{
    return (uint8_t)((value < 32U) || (value == 127U));
}

uint8_t hps_char_is_sign(const uint8_t value)
{
    return (uint8_t)((value == 43U) || (value == 45U));
}

uint8_t hps_char_is_path_separator(const uint8_t value)
{
    return (uint8_t)((value == 47U) || (value == 92U));
}

uint8_t hps_char_is_line_ending(const uint8_t value)
{
    return (uint8_t)((value == 10U) || (value == 13U));
}

uint8_t hps_char_to_lower(const uint8_t value)
{
    return (hps_char_is_alpha_upper(value) == 1U) ?
        (uint8_t)(value + 32U) : value;
}

uint8_t hps_char_to_upper(const uint8_t value)
{
    return (hps_char_is_alpha_lower(value) == 1U) ?
        (uint8_t)(value - 32U) : value;
}

int32_t hps_char_hex_to_nibble(const uint8_t value)
{
    int32_t result = -INT32_C(1);
    if (hps_char_is_decimal(value) == 1U) {
        result = (int32_t)(value - 48U);
    } else if (hps_char_is_hex_lower(value) == 1U) {
        result = (int32_t)(value - 97U) + INT32_C(10);
    } else if (hps_char_is_hex_upper(value) == 1U) {
        result = (int32_t)(value - 65U) + INT32_C(10);
    } else {
        /* Nonhexadecimal input keeps the legacy invalid sentinel. */
    }
    return result;
}

uint8_t hps_char_nibble_to_hex_lower(const uint8_t nibble)
{
    const uint8_t value = (uint8_t)(nibble & UINT8_C(15));
    return (value < 10U) ? (uint8_t)(48U + value) :
        (uint8_t)(97U + (value - 10U));
}

uint8_t hps_char_nibble_to_hex_upper(const uint8_t nibble)
{
    const uint8_t value = (uint8_t)(nibble & UINT8_C(15));
    return (value < 10U) ? (uint8_t)(48U + value) :
        (uint8_t)(65U + (value - 10U));
}
