#ifndef HPS_CSTR_CONVERT_H
#define HPS_CSTR_CONVERT_H

/**
 * @file hps/cstr/convert.h
 * @brief Checked integer and byte conversions with bounded C-string inputs.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/string/view.h"

#define HPS_CSTR_BYTES_HEX_COMPACT ((uint8_t)0U)
#define HPS_CSTR_BYTES_HEX_SPACED ((uint8_t)1U)
#define HPS_CSTR_BYTES_HEX_PREFIXED ((uint8_t)2U)
#define HPS_CSTR_BYTES_BINARY_COMPACT ((uint8_t)3U)
#define HPS_CSTR_BYTES_BINARY_GROUPED ((uint8_t)4U)
#define HPS_CSTR_BYTES_DECIMAL_CSV ((uint8_t)5U)
#define HPS_CSTR_BYTES_DECIMAL_BRACKETS ((uint8_t)6U)
#define HPS_CSTR_BYTES_TEXT ((uint8_t)7U)

#ifdef __cplusplus
extern "C" {
#endif

/* Every input capacity is a real readable extent including NUL; scanning stops
 * at its first NUL. Missing NUL => NOT_TERMINATED. NULL required arguments/zero
 * input capacity => INVALID_ARGUMENT. Outputs and metadata must be disjoint from
 * inputs and one another unless explicitly stated. Caller owns live storage and
 * serializes conflicting access. All failures preserve outputs, including tails.
 * No allocation, OS, locale, Unicode processing, runtime registration or locks.
 * Exact output capacity includes NUL; no silent truncation or trailing clearing.
 * The counted/temporary char storage proposal is recorded in the string review.
 */

/* Entire nonempty input is consumed, without whitespace, prefixes or separators.
 * Radix is 2, 8, 10 or 16. Unsigned input has no sign; signed decimal accepts one
 * optional '+'/'-'. Invalid syntax => INVALID_ARGUMENT; overflow => OUT_OF_RANGE.
 * UINT64_MAX and INT64_MIN are valid results, never error sentinels. O(capacity).
 */
int32_t hps_cstr_parse_u64(const char *input, size_t input_capacity,
                           uint8_t radix, uint64_t *result);
int32_t hps_cstr_parse_i64(const char *input, size_t input_capacity,
                           int64_t *result);

/* Uppercase digits, no radix prefix; signed decimal includes '-' only as needed.
 * Radix errors => INVALID_ARGUMENT; capacity insufficient => NO_SPACE.
 * O(64), at most 65 temporary C-string characters including NUL.
 */
int32_t hps_cstr_render_u64(char *output, size_t output_capacity,
                            uint64_t value, uint8_t radix);
int32_t hps_cstr_render_i64(char *output, size_t output_capacity,
                            int64_t value);

/* Encode complete byte arrays. NULL data is valid only at zero length. Hex uses
 * uppercase digits; prefixed format is 0xAB,0xCD; grouped binary uses spaces;
 * decimal brackets is [1, 2]. TEXT preserves byte representations and rejects
 * zero bytes; it performs no ASCII/UTF conversion. Non-TEXT payloads must not
 * overlap output. TEXT supports overlap via memmove. O(length), no scratch array.
 * Size overflow => OUT_OF_RANGE; insufficient output => NO_SPACE.
 */
int32_t hps_cstr_encode_bytes(char *output, size_t output_capacity,
                              const uint8_t *data, size_t length,
                              uint8_t format);

/* Decode HEX_COMPACT, BINARY_COMPACT or TEXT only. Hex accepts an odd leading
 * nibble as one byte (ABC => 0A BC), retaining the legacy convention. Binary
 * requires complete eight-bit groups. Empty input succeeds with zero bytes.
 * Output/count metadata must be disjoint from input; TEXT payload overlap is
 * supported via memmove. NULL output is valid only with zero capacity and empty
 * result. Invalid syntax => INVALID_ARGUMENT; insufficient output => NO_SPACE.
 * Validate every input digit before writing; O(capacity), no partial output.
 */
int32_t hps_cstr_decode_bytes(uint8_t *output, size_t output_capacity,
                              const char *input, size_t input_capacity,
                              uint8_t format, size_t *written);

/* Nonempty binary input; odd inverse_count flips bits, even count copies.
 * Exact in-place operation is supported; other overlap is a caller violation.
 * Validate the full input before mutation. O(capacity), no scratch array.
 */
int32_t hps_cstr_invert_binary(char *output, size_t output_capacity,
                               const char *input,
                               size_t input_capacity,
                               uint64_t inverse_count);

/* flag must be 0/1. Borrow immutable execution-character-set "false"/"true",
 * valid for program lifetime. NULL output/other flag => INVALID_ARGUMENT. O(1).
 */
int32_t hps_cstr_boolalpha(uint8_t flag, const char **result);

/* Legacy truthy spelling: skip only leading ASCII space/tab/LF/CR/FF/VT, then
 * match the entire suffix against true/yes/on/1, case-insensitively for basic
 * Latin letters. Every other terminated input (including empty, false/no/off,
 * unknown text and trailing whitespace) succeeds with flag zero. This is not a
 * strict true/false validator. ASCII-compatible whitespace encoding is required;
 * no locale/Unicode whitespace or POSIX dependency. Required arguments and zero
 * capacity => INVALID_ARGUMENT; missing NUL => NOT_TERMINATED; failures preserve
 * result. All extent/lifetime/disjointness rules above apply. O(input_capacity),
 * fixed scalar/view state, no payload temporary array. */
int32_t hps_cstr_parse_bool(const char *input, size_t input_capacity,
                            uint8_t *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_CSTR_CONVERT_H */
