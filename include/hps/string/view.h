#ifndef HPS_STRING_VIEW_H
#define HPS_STRING_VIEW_H

/**
 * @file hps/string/view.h
 * @brief Borrowed text views, bounded searches and exact C-string copying.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/status.h"

/* Strings own status module 3, reasons 1..5. Representation is hps_status_t. */
typedef hps_status_t hps_string_status;
#define HPS_STRING_OK HPS_STATUS_OK
#define HPS_STRING_INVALID_ARGUMENT ((hps_string_status)-INT64_C(196609))
#define HPS_STRING_OUT_OF_RANGE ((hps_string_status)-INT64_C(196610))
#define HPS_STRING_NO_SPACE ((hps_string_status)-INT64_C(196611))
#define HPS_STRING_INVALID_STATE ((hps_string_status)-INT64_C(196612))
#define HPS_STRING_NOT_TERMINATED ((hps_string_status)-INT64_C(196613))
#define HPS_STRING_NPOS SIZE_MAX

typedef struct {
    const char *data;
    size_t length;
} hps_string_view;

#ifdef __cplusplus
extern "C" {
#endif

/* Views borrow length initialized readable characters in one live object.
 * NULL is valid only at zero length; length must be below NPOS (reserved sentinel).
 * Empty outputs are normalized to {NULL, 0}.
 * Text need not be NUL-terminated and may contain embedded NUL. Encoding is not
 * interpreted; only trim_whitespace recognizes the six basic whitespace chars.
 * Caller owns storage and serializes conflicting access/release. No allocation,
 * OS, mutable global state, blocking, Unicode or secret-time guarantee.
 * Descriptor/scalar outputs must be disjoint from payloads and each other.
 * Input descriptors are passed by value, so a result may replace its input view.
 * All errors preserve outputs. Pointer extents/lifetimes remain preconditions.
 * Counted character storage has a proposed scoped policy disposition in
 * governance/reviews/string-storage-2026-10-13.md; no qualification is asserted.
 */

/* O(1); NULL with positive length, length == NPOS or NULL result => INVALID_ARGUMENT. */
hps_string_status hps_string_view_init(hps_string_view *result,
                                        const char *data, size_t length);

/* Read at most capacity characters; capacity is a real readable bound including
 * NUL. NULL data/result or zero capacity => INVALID_ARGUMENT; missing NUL within
 * capacity => NOT_TERMINATED. Successful view excludes NUL. O(capacity). */
hps_string_status hps_string_view_init_cstr(hps_string_view *result,
                                             const char *data, size_t capacity);

/* Exact slice: offset <= length and count <= length-offset; no count clamping.
 * Bad bounds => OUT_OF_RANGE. Malformed view/NULL result => INVALID_ARGUMENT.
 * O(1); no pointer arithmetic for empty slices, including NULL/zero views. */
hps_string_status hps_string_view_slice(hps_string_view view, size_t offset,
                                         size_t count, hps_string_view *result);

/* Write a 0/1 flag, or -1/0/1 comparison in unsigned-byte lexicographic order.
 * Malformed inputs/NULL result => INVALID_ARGUMENT. O(minimum length). */
hps_string_status hps_string_view_equal(hps_string_view left,
                                         hps_string_view right, uint8_t *result);
hps_string_status hps_string_view_compare(hps_string_view left,
                                           hps_string_view right, int32_t *result);
hps_string_status hps_string_view_starts_with(hps_string_view view,
                                               hps_string_view prefix,
                                               uint8_t *result);
hps_string_status hps_string_view_ends_with(hps_string_view view,
                                             hps_string_view suffix,
                                             uint8_t *result);

/* Successful search writes an index or NPOS (no match); absence is not an error.
 * find requires offset <= view.length, else OUT_OF_RANGE. An empty needle matches
 * offset. rfind searches backwards from min(position, last possible start);
 * NPOS means the end; empty needle matches min(position, view.length).
 * NULL/empty views and needles follow the same semantics without NULL arithmetic.
 * Malformed view/needle or NULL output => INVALID_ARGUMENT.
 * Worst-case O(view.length * needle.length), bounded by the declared extents. */
hps_string_status hps_string_view_find(hps_string_view view,
                                        hps_string_view needle, size_t offset,
                                        size_t *result);
hps_string_status hps_string_view_rfind(hps_string_view view,
                                         hps_string_view needle, size_t position,
                                         size_t *result);
hps_string_status hps_string_view_contains(hps_string_view view,
                                            hps_string_view needle,
                                            uint8_t *result);

/* Borrow the remaining window after removing members of chars from both ends.
 * Empty chars removes nothing. O(view.length * chars.length), no payload writes.
 * trim_whitespace uses space, tab, newline, CR, vertical tab and form feed. */
hps_string_status hps_string_view_trim(hps_string_view view,
                                        hps_string_view chars,
                                        hps_string_view *result);
hps_string_status hps_string_view_trim_whitespace(hps_string_view view,
                                                   hps_string_view *result);

/* Exact C-string export, no truncation. Writable capacity includes NUL; at least
 * view.length+1 is needed, checked by subtraction/comparison before addition.
 * NULL output/malformed view or embedded NUL => INVALID_ARGUMENT; insufficient
 * capacity => NO_SPACE (checked before scanning payload for embedded NUL).
 * Payload overlap is supported via memmove. O(view.length); bytes after the new
 * terminator are preserved. Caller proves storage extent and disjoint metadata. */
hps_string_status hps_string_view_copy_cstr(hps_string_view view, char *output,
                                             size_t capacity);

/* Extended legacy view families retain counted text/NPOS semantics. Character
 * values/results are uint8_t octets. All failed calls preserve outputs. */
hps_string_status hps_string_view_at(hps_string_view view, size_t index, uint8_t *result);
hps_string_status hps_string_view_front(hps_string_view view, uint8_t *result);
hps_string_status hps_string_view_back(hps_string_view view, uint8_t *result);
hps_string_status hps_string_view_data(hps_string_view view, const char **result);
hps_string_status hps_string_view_length(hps_string_view view, size_t *result);
hps_string_status hps_string_view_is_empty(hps_string_view view, uint8_t *result);
uint8_t hps_string_view_is_valid(hps_string_view view);
hps_string_status hps_string_view_left(hps_string_view view, size_t count, hps_string_view *result);
hps_string_status hps_string_view_right(hps_string_view view, size_t count, hps_string_view *result);
hps_string_status hps_string_view_remove_prefix(hps_string_view *view, size_t count);
hps_string_status hps_string_view_remove_suffix(hps_string_view *view, size_t count);
hps_string_status hps_string_view_trim_prefix_char(hps_string_view *view, uint8_t value);
hps_string_status hps_string_view_trim_suffix_char(hps_string_view *view, uint8_t value);
hps_string_status hps_string_view_trim_char(hps_string_view view, uint8_t value, hps_string_view *result);
/* Exact raw copy writes count characters without NUL. No clamping/truncation;
 * offset/count beyond the view => OUT_OF_RANGE, capacity<count => NO_SPACE.
 * NULL output is permitted only for zero capacity/count. Payload overlap allowed. */
hps_string_status hps_string_view_copy(hps_string_view view, char *output, size_t capacity,
                                      size_t count, size_t offset, size_t *copied);
hps_string_status hps_string_view_find_char(hps_string_view view, uint8_t value, size_t offset, size_t *result);
hps_string_status hps_string_view_rfind_char(hps_string_view view, uint8_t value, size_t position, size_t *result);
hps_string_status hps_string_view_contains_char(hps_string_view view, uint8_t value, uint8_t *result);
hps_string_status hps_string_view_find_first_of(hps_string_view view, hps_string_view chars, size_t offset, size_t *result);
hps_string_status hps_string_view_find_first_not_of(hps_string_view view, hps_string_view chars, size_t offset, size_t *result);
hps_string_status hps_string_view_find_last_of(hps_string_view view, hps_string_view chars, size_t position, size_t *result);
hps_string_status hps_string_view_find_last_not_of(hps_string_view view, hps_string_view chars, size_t position, size_t *result);
/* C-string conveniences require capacity INCLUDING NUL for every input. */
hps_string_status hps_string_view_equals_cstr(hps_string_view view, const char *text, size_t capacity, uint8_t *result);
hps_string_status hps_string_view_starts_with_cstr(hps_string_view view, const char *text, size_t capacity, uint8_t *result);
hps_string_status hps_string_view_ends_with_cstr(hps_string_view view, const char *text, size_t capacity, uint8_t *result);
hps_string_status hps_string_view_contains_cstr(hps_string_view view, const char *text, size_t capacity, uint8_t *result);
hps_string_status hps_string_view_find_cstr(hps_string_view view, const char *text, size_t capacity, size_t offset, size_t *result);
hps_string_status hps_string_view_rfind_cstr(hps_string_view view, const char *text, size_t capacity, size_t position, size_t *result);
/* Splitting uses nonoverlapping delimiters; substring delimiter must be nonempty.
 * keep_empty must be 0/1. cursor starts at zero and becomes NPOS at exhaustion.
 * Success writes found=0/1; at exhaustion part stays unchanged. Invalid cursors
 * (above length except NPOS) => OUT_OF_RANGE. Metadata and output arrays must be
 * disjoint from source/payloads and each other. Split list preflights every piece
 * before writes: capacity exhaustion => NO_SPACE with list/count unchanged.
 * NULL list is allowed only at zero capacity. Work O(length*delimiter.length)
 * (two passes for list); fixed metadata, no allocation. Normalized empty parts. */
hps_string_status hps_string_view_split_next(hps_string_view view, hps_string_view delimiter,
                                            size_t *cursor, hps_string_view *part,
                                            uint8_t keep_empty, uint8_t *found);
hps_string_status hps_string_view_split_next_char(hps_string_view view, uint8_t delimiter,
                                                 size_t *cursor, hps_string_view *part,
                                                 uint8_t keep_empty, uint8_t *found);
hps_string_status hps_string_view_split(hps_string_view view, hps_string_view delimiter,
                                       hps_string_view *list, size_t capacity,
                                       uint8_t keep_empty, size_t *count);
hps_string_status hps_string_view_split_char(hps_string_view view, uint8_t delimiter,
                                            hps_string_view *list, size_t capacity,
                                            uint8_t keep_empty, size_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_STRING_VIEW_H */
