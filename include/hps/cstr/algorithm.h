#ifndef HPS_CSTR_ALGORITHM_H
#define HPS_CSTR_ALGORITHM_H

/**
 * @file hps/cstr/algorithm.h
 * @brief Bounded validation, tokenization, lexical paths and diagnostic rendering.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/string/view.h"

#define HPS_CSTR_CLASS_BINARY ((uint8_t)0U)
#define HPS_CSTR_CLASS_DECIMAL ((uint8_t)1U)
#define HPS_CSTR_CLASS_OCTAL ((uint8_t)2U)
#define HPS_CSTR_CLASS_HEX ((uint8_t)3U)
#define HPS_CSTR_CLASS_ALPHA ((uint8_t)4U)
#define HPS_CSTR_CLASS_ALNUM ((uint8_t)5U)

#ifdef __cplusplus
extern "C" {
#endif

/* Raw C-string inputs use real readable capacities including NUL. NULL/zero
 * capacity => INVALID_ARGUMENT; missing NUL => NOT_TERMINATED. Counted inputs
 * borrow initialized characters and must contain no NUL. Caller owns storage,
 * lifetime and serialization. Outputs/metadata are disjoint from source payloads
 * and one another; no output/input payload overlap is supported here. Failures
 * preserve every output. Output capacities include NUL, with no truncation.
 * No allocation, OS/filesystem access, locale, Unicode, synchronization or erasure.
 */

/* Nonempty ASCII/basic Latin character classes; decimal also covers the legacy
 * numeric predicate. Empty input => successful flag 0; flags are exactly 0/1.
 * Unknown class => INVALID_ARGUMENT. O(input_capacity), no integer range limit.
 */
int32_t hps_cstr_validate(const char *input, size_t input_capacity,
                          uint8_t character_class, uint8_t *result);

/* Nonempty delimiter; nonoverlapping matches, preserving consecutive/trailing
 * empty fields. Empty input has zero fields consistently. Borrowed empty fields
 * normalize to {NULL,0}. split_at out of range => OUT_OF_RANGE. split requires
 * complete array capacity, else NO_SPACE; no partial metadata is written.
 * Tokens borrow input until mutation/release. Worst O(input length * delimiter
 * length), two passes for split. No hidden scratch array.
 */
int32_t hps_cstr_split_count(const char *input, size_t input_capacity,
                             const char *delimiter,
                             size_t delimiter_capacity, size_t *result);
int32_t hps_cstr_split_at(const char *input, size_t input_capacity,
                          const char *delimiter,
                          size_t delimiter_capacity, size_t index,
                          hps_string_view *result);
int32_t hps_cstr_split(const char *input, size_t input_capacity,
                       const char *delimiter,
                       size_t delimiter_capacity,
                       hps_string_view *tokens, size_t token_capacity,
                       size_t *written);

/* Join count borrowed views, inserting delimiter between them. NULL tokens is
 * valid only for zero count. Empty delimiter is allowed. Validate all extents,
 * embedded NUL and checked total length before writing. O(total input length).
 */
int32_t hps_cstr_join(char *output, size_t output_capacity,
                      const hps_string_view *tokens, size_t count,
                      const char *delimiter,
                      size_t delimiter_capacity);

/* Insert nonempty separator between groups aligned from the right: 1234567,
 * group_length3, separator"," => 1,234,567. group_length must be positive.
 * O(input length + output length); no scratch storage.
 */
int32_t hps_cstr_group(char *output, size_t output_capacity,
                       const char *input, size_t input_capacity,
                       size_t group_length, const char *separator,
                       size_t separator_capacity);

/* Pure lexical filename after the last '/' or '\\'; no filesystem operations.
 * remove_extension is 0/1; strip final dot suffix except a leading dot.
 * Empty paths/trailing separators yield an empty view. O(input_capacity).
 */
int32_t hps_cstr_filename(const char *path, size_t path_capacity,
                          uint8_t remove_extension,
                          hps_string_view *result);

/* Borrow smaller/larger text in unsigned-byte lexical order. maximum is 0/1;
 * equal values select right, retaining legacy tie behavior. NULL input is an
 * argument error instead of the legacy nullable shortcut. O(input capacities).
 */
int32_t hps_cstr_select(const char *left, size_t left_capacity,
                        const char *right, size_t right_capacity,
                        uint8_t maximum, hps_string_view *result);

/* Render title + repeated fill + value, using complete fill chunks up to width
 * payload characters. Preserve complete title/value; their combined length must
 * fit width, else NO_SPACE. Empty fill concatenates them. capacity > width is
 * required. O(input capacities + width); no padding with incomplete fill chunks.
 */
int32_t hps_cstr_title_value(char *output, size_t output_capacity,
                             const char *title, size_t title_capacity,
                             const char *fill, size_t fill_capacity,
                             const char *value, size_t value_capacity,
                             size_t width);

/* Complete deterministic diagnostics, no pointers/locale/printf. Integer output
 * is "field: decimal\n"; byte dump is "field: printable | HEX: AB CD\n".
 * Nonprintable ASCII octets render '.', printable octets map to execution text.
 * NULL data only with zero length. O(field capacity + length), fixed integer
 * scratch <=21 characters; byte dump has no scratch payload array.
 */
int32_t hps_cstr_dump_i64(char *output, size_t output_capacity,
                          const char *field, size_t field_capacity,
                          int64_t value);
int32_t hps_cstr_dump_bytes(char *output, size_t output_capacity,
                            const char *field, size_t field_capacity,
                            const uint8_t *data, size_t length);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_CSTR_ALGORITHM_H */
