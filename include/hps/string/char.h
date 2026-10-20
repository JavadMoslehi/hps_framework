#ifndef HPS_STRING_CHAR_H
#define HPS_STRING_CHAR_H

/**
 * @file hps/string/char.h
 * @brief Locale-independent ASCII character classes and bounded conversions.
 */

#include "hps/base/hps_stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Values are ASCII octets, never locale/Unicode characters. Predicates return
 * exactly 0/1. Folding preserves values outside A-Z/a-z. No storage, allocation
 * or side effects; O(1). hex_lower/upper recognize letters a-f/A-F only, as in
 * CSEP; is_hex also accepts decimal digits. hex_to_nibble returns -1 on invalid
 * input; nibble_to_hex retains legacy low-nibble masking for every uint8_t. */
uint8_t hps_char_is_binary(uint8_t value);
uint8_t hps_char_is_octal(uint8_t value);
uint8_t hps_char_is_decimal(uint8_t value);
uint8_t hps_char_is_hex_lower(uint8_t value);
uint8_t hps_char_is_hex_upper(uint8_t value);
uint8_t hps_char_is_hex(uint8_t value);
uint8_t hps_char_is_alpha_lower(uint8_t value);
uint8_t hps_char_is_alpha_upper(uint8_t value);
uint8_t hps_char_is_alpha(uint8_t value);
uint8_t hps_char_is_alphanumeric(uint8_t value);
uint8_t hps_char_is_whitespace(uint8_t value);
uint8_t hps_char_is_printable(uint8_t value);
uint8_t hps_char_is_control(uint8_t value);
uint8_t hps_char_is_sign(uint8_t value);
uint8_t hps_char_is_path_separator(uint8_t value);
uint8_t hps_char_is_line_ending(uint8_t value);
uint8_t hps_char_to_lower(uint8_t value);
uint8_t hps_char_to_upper(uint8_t value);
int32_t hps_char_hex_to_nibble(uint8_t value);
uint8_t hps_char_nibble_to_hex_lower(uint8_t nibble);
uint8_t hps_char_nibble_to_hex_upper(uint8_t nibble);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_STRING_CHAR_H */
