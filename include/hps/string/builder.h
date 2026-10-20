#ifndef HPS_STRING_BUILDER_H
#define HPS_STRING_BUILDER_H

/**
 * @file hps/string/builder.h
 * @brief Caller-owned bounded string editing, composition and JSON escaping.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/string/buffer.h"

typedef hps_string_buffer hps_string_builder;

#ifdef __cplusplus
extern "C" {
#endif

/* Builder borrows caller storage; capacity INCLUDING NUL never grows. All buffer
 * state/lifetime/metadata-disjointness rules apply. No heap/new/delete/take_cstr.
 * reserve checks usable capacity and returns NO_SPACE instead of allocating.
 * Every error preserves builder payload/descriptor, scalar results AND scratch.
 * Scratch designates writable capacity characters in a live separate object,
 * disjoint from builder, all source views and metadata; actual extent/disjointness
 * are caller preconditions. Scratch contents need no initialization. Transactional
 * edits preflight every validation/size before writing scratch; commit then uses
 * overlap-safe buffer assignment. Scratch is unspecified on success.
 * ASCII folding/JSON escape require an ASCII-compatible execution charset.
 * Arbitrary non-ASCII octets are preserved; no UTF-8 validation/normalization.
 * Work bounded by source lengths, match count and capacities; no timing claim. */
int32_t hps_string_builder_init(hps_string_builder *builder, char *storage, size_t capacity);
int32_t hps_string_builder_clear(hps_string_builder *builder);
int32_t hps_string_builder_as_view(const hps_string_builder *builder, hps_string_view *result);
int32_t hps_string_builder_reserve(const hps_string_builder *builder, size_t usable_capacity);
int32_t hps_string_builder_swap(hps_string_builder *left, hps_string_builder *right);
int32_t hps_string_builder_assign_view(hps_string_builder *builder, hps_string_view source);
int32_t hps_string_builder_append_view(hps_string_builder *builder, hps_string_view source);
/* Character writes require non-NUL ASCII value (1..127). count/resize are exact;
 * NO_SPACE leaves all text unchanged, including a shrinking resize with bad fill. */
int32_t hps_string_builder_append_char(hps_string_builder *builder, uint8_t value);
int32_t hps_string_builder_append_repeat_char(hps_string_builder *builder, uint8_t value, size_t count);
int32_t hps_string_builder_resize(hps_string_builder *builder, size_t length, uint8_t fill);
int32_t hps_string_builder_append_newline(hps_string_builder *builder);
int32_t hps_string_builder_append_line(hps_string_builder *builder, hps_string_view source,
                                       char *scratch, size_t scratch_capacity);
int32_t hps_string_builder_prepend_view(hps_string_builder *builder, hps_string_view source,
                                        char *scratch, size_t scratch_capacity);
int32_t hps_string_builder_insert_view(hps_string_builder *builder, size_t offset, hps_string_view source,
                                       char *scratch, size_t scratch_capacity);
int32_t hps_string_builder_erase(hps_string_builder *builder, size_t offset, size_t count);
/* Exact ranges; count is not clamped. Scratch handles sources aliasing builder.
 * Pattern must be nonempty for replacement search. Matches are nonoverlapping
 * in the ORIGINAL text, with replacements not searched again. Missing match
 * succeeds with changed/count zero and does not touch scratch. */
int32_t hps_string_builder_replace_range_view(hps_string_builder *builder, size_t offset, size_t count,
                                              hps_string_view replacement, char *scratch, size_t scratch_capacity);
int32_t hps_string_builder_replace_first(hps_string_builder *builder, hps_string_view pattern,
                                         hps_string_view replacement, char *scratch, size_t scratch_capacity,
                                         uint8_t *changed);
int32_t hps_string_builder_replace_all(hps_string_builder *builder, hps_string_view pattern,
                                       hps_string_view replacement, char *scratch, size_t scratch_capacity,
                                       size_t *count);
int32_t hps_string_builder_replace_char_all(hps_string_builder *builder, uint8_t old_value,
                                            uint8_t new_value, size_t *count);
int32_t hps_string_builder_trim_ascii_whitespace(hps_string_builder *builder);
int32_t hps_string_builder_trim_char(hps_string_builder *builder, uint8_t value);
int32_t hps_string_builder_to_upper_ascii(hps_string_builder *builder);
int32_t hps_string_builder_to_lower_ascii(hps_string_builder *builder);
/* quoted adds literal quotes (not escape); JSON variants escape quotes,
 * backslashes, standard controls and remaining 0..31 as lowercase hex \u00xx.
 * JSON accepts embedded NUL in the counted input; quoted/plain line reject it.
 * Output-size arithmetic is checked character by character before any mutation. */
int32_t hps_string_builder_append_quoted(hps_string_builder *builder, hps_string_view source,
                                         char *scratch, size_t scratch_capacity);
int32_t hps_string_builder_append_json_escaped(hps_string_builder *builder, hps_string_view source,
                                               char *scratch, size_t scratch_capacity);
int32_t hps_string_builder_append_json_quoted(hps_string_builder *builder, hps_string_view source,
                                              char *scratch, size_t scratch_capacity);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_STRING_BUILDER_H */
