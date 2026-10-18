#ifndef HPS_CSTR_CORE_H
#define HPS_CSTR_CORE_H

/**
 * @file hps/cstr/core.h
 * @brief Bounded C-string inspection, copying and in-place text transformations.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/string/buffer.h"

#define HPS_CSTR_KEEP_CASE ((uint8_t)0U)
#define HPS_CSTR_LOWER_CASE ((uint8_t)1U)
#define HPS_CSTR_UPPER_CASE ((uint8_t)2U)
#define HPS_CSTR_REMOVE_SET ((uint8_t)0U)
#define HPS_CSTR_KEEP_DIGITS ((uint8_t)1U)
#define HPS_CSTR_REMOVE_DIGITS ((uint8_t)2U)

#ifdef __cplusplus
extern "C" {
#endif

/* All capacities are real accessible extents including NUL. NULL C-string data,
 * zero capacity or NULL outputs are invalid; missing NUL is NOT_TERMINATED,
 * except is_terminated which succeeds with flag zero. Every failure preserves
 * caller state/output. Buffer operations validate the existing string invariants
 * before mutation; a malformed buffer is INVALID_STATE. Caller owns live storage,
 * serializes access, and keeps metadata/output objects disjoint from payloads.
 * Views follow hps/string/view.h, with NPOS reserved and empty normalized.
 * No heap, OS, locale, Unicode interpretation, synchronization or secret wiping.
 * Work is bounded by capacities/lengths; automatic scratch is at most two chars
 * plus fixed metadata. Target stack/libc budgets remain qualification work.
 */

int32_t hps_cstr_length(const char *data, size_t capacity,
                        size_t *result);
int32_t hps_cstr_is_empty(const char *data, size_t capacity,
                          uint8_t *result);
int32_t hps_cstr_is_terminated(const char *data, size_t capacity,
                               uint8_t *result);
int32_t hps_cstr_has_capacity_for(const char *data, size_t capacity,
                                  size_t additional, uint8_t *result);
/* Plan the truncating copy length without copying; zero output capacity => zero.
 * Input still must be a bounded C string. O(input_capacity). */
int32_t hps_cstr_copy_length(const char *data, size_t capacity,
                             size_t output_capacity, size_t *result);
/* Clear all bound storage, or only length..capacity including the current NUL.
 * These ordinary stores provide no security erasure guarantee. O(capacity). */
int32_t hps_cstr_clear(hps_string_buffer *buffer);
int32_t hps_cstr_clear_tail(hps_string_buffer *buffer);

/* Compare at most limit characters (NPOS means whole strings), normalized to
 * -1/0/1 with unsigned-byte ordering. ignore_case must be zero or one; one folds
 * the basic Latin letters through immutable upper/lower tables. O(capacities). */
int32_t hps_cstr_compare(const char *left, size_t left_capacity,
                         const char *right, size_t right_capacity,
                         size_t limit, uint8_t ignore_case,
                         int32_t *result);
/* Exact range copy followed by optional case folding; count NPOS means suffix.
 * Invalid mode => INVALID_ARGUMENT; invalid range => OUT_OF_RANGE. Copy supports
 * overlapping source/output via memmove. O(input_capacity + output.length). */
int32_t hps_cstr_copy_range(hps_string_buffer *output,
                            const char *input, size_t input_capacity,
                            size_t offset, size_t count, uint8_t mode);
/* Empty input is false, matching the legacy consists-of predicate. The allowed
 * set is counted and may contain NUL. O(input_capacity + input.length*set.length). */
int32_t hps_cstr_consists_of(const char *data, size_t capacity,
                             hps_string_view allowed, uint8_t *result);

/* Bounded substring searches reuse view semantics. rfind position is the largest
 * starting index to consider; NPOS selects the last match. Empty needle matches.
 * Character views must contain exactly one character; NUL is a valid searched
 * character, but C-string payloads exclude their terminator, so it has no match.
 * O(input_capacity + needle_capacity + input.length*needle.length). */
int32_t hps_cstr_find(const char *data, size_t capacity,
                      const char *needle, size_t needle_capacity,
                      size_t offset, size_t *result);
int32_t hps_cstr_rfind(const char *data, size_t capacity,
                       const char *needle, size_t needle_capacity,
                       size_t position, size_t *result);
int32_t hps_cstr_find_char(const char *data, size_t capacity,
                           hps_string_view character, size_t offset,
                           size_t *result);
int32_t hps_cstr_rfind_char(const char *data, size_t capacity,
                            hps_string_view character, size_t position,
                            size_t *result);

/* Reverse exact offset/count (NPOS means suffix); deletion uses the existing
 * hps_string_buffer_erase. No payload change on bad bounds. O(buffer.length). */
int32_t hps_cstr_reverse(hps_string_buffer *buffer, size_t offset,
                         size_t count);
/* old/new each designate one non-NUL character. Their payloads may overlap the
 * buffer: both are snapshotted before replacement. O(buffer.length). */
int32_t hps_cstr_replace_char(hps_string_buffer *buffer,
                              hps_string_view old_character,
                              hps_string_view new_character);
/* Compact in place. REMOVE_SET removes all members of chars; KEEP_DIGITS removes
 * nondigits; REMOVE_DIGITS removes digits. chars must be an empty valid view in
 * digit modes. REMOVE_SET's set must be disjoint from the entire buffer storage.
 * O(buffer.length*chars.length), or O(buffer.length) for digit modes. */
int32_t hps_cstr_filter(hps_string_buffer *buffer,
                        hps_string_view chars, uint8_t mode);
/* Remove nonoverlapping occurrences from left to right. Empty needle is a no-op.
 * needle storage must be disjoint from the entire buffer storage. O(length*needle).
 * No scratch payload and no second pass over already removed text. */
int32_t hps_cstr_remove_substring(hps_string_buffer *buffer,
                                  const char *needle,
                                  size_t needle_capacity);

/* Trim independent character sets, or repeated exact left/right substrings.
 * Empty sets/patterns remove nothing. All bounds are computed before mutation,
 * allowing set/pattern payloads to overlap buffer storage. O(length*set/pattern).
 * Whitespace trim uses hps_string_view_trim_whitespace + buffer_assign. */
int32_t hps_cstr_trim(hps_string_buffer *buffer,
                      hps_string_view left_chars,
                      hps_string_view right_chars);
int32_t hps_cstr_trim_substrings(hps_string_buffer *buffer,
                                 hps_string_view left_pattern,
                                 hps_string_view right_pattern);

/* Pattern payloads must be disjoint from the entire buffer storage and contain
 * no NUL. Empty patterns produce no bytes, regardless of repeat count. Products
 * and total growth are proven against capacity before writes; NO_SPACE preserves
 * everything. pad adds repeated left/right patterns to existing text. repeat
 * replaces text with pattern repeated count times. O(old length + output length).
 * One-character patterns provide all legacy character pad/repeat operations. */
int32_t hps_cstr_pad(hps_string_buffer *buffer,
                     hps_string_view left_pattern, size_t left_count,
                     hps_string_view right_pattern, size_t right_count);
int32_t hps_cstr_repeat(hps_string_buffer *buffer,
                        hps_string_view pattern, size_t count);
/* Grow to target length by repeating pattern, shortening the last repetition
 * when needed. left is zero/right or one/left; otherwise INVALID_ARGUMENT.
 * target <= current length is a successful no-op. Empty pattern is invalid when
 * growth is needed. Same disjoint pattern and failure rules as pad. */
int32_t hps_cstr_pad_to(hps_string_buffer *buffer,
                        hps_string_view pattern, size_t target,
                        uint8_t left);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_CSTR_CORE_H */
