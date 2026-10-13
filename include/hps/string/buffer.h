#ifndef HPS_STRING_BUFFER_H
#define HPS_STRING_BUFFER_H

/**
 * @file hps/string/buffer.h
 * @brief Caller-owned fixed-capacity C-string storage with atomic failures.
 */

#include <stddef.h>
#include "hps/string/view.h"

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} hps_string_buffer;

#ifdef __cplusplus
extern "C" {
#endif

/* Capacity is the total number of writable characters INCLUDING NUL, at least
 * one. Initialized buffers satisfy length < capacity, data[length] == NUL and
 * no earlier NUL. Caller owns/live storage, may not forge descriptor extents, and
 * serializes all access. No allocation, growth, pool dependency or synchronization.
 * Descriptor/output metadata must not overlap payloads or each other. Every
 * failure preserves descriptor, payload and outputs. Copies support overlapping
 * source/destination payloads; source characters must all be initialized.
 * Source views containing NUL cannot be assigned/appended as a C string.
 * Checks scan the active prefix to detect broken termination/length invariants;
 * modifying returned text/descriptor through another alias can invalidate state.
 * Actual stack/libc dependencies require target evidence, not an O() claim.
 */

/* Bind capacity storage as empty, writing only storage[0]. NULL arguments or
 * zero capacity => INVALID_ARGUMENT. O(1); no storage initialization needed. */
hps_string_status hps_string_buffer_init(hps_string_buffer *buffer,
                                          char *storage, size_t capacity);

/* Adopt an existing terminated string within capacity, without payload writes.
 * Same argument errors; missing NUL => NOT_TERMINATED. O(capacity). */
hps_string_status hps_string_buffer_init_cstr(hps_string_buffer *buffer,
                                               char *storage, size_t capacity);

/* Validate buffer first, then source shape, then capacity, then embedded NUL.
 * NULL buffer/malformed source/embedded NUL => INVALID_ARGUMENT;
 * broken buffer invariants => INVALID_STATE; insufficient space => NO_SPACE.
 * Copy whole source with memmove, committing length/NUL only on success.
 * assign replaces active text; append preserves the old prefix. Self/subview
 * overlap is supported. O(old length + source.length); tail after NUL preserved. */
hps_string_status hps_string_buffer_assign(hps_string_buffer *buffer,
                                            hps_string_view source);
hps_string_status hps_string_buffer_append(hps_string_buffer *buffer,
                                            hps_string_view source);

/* Erase count at offset; offset <= length and count <= length-offset, otherwise
 * OUT_OF_RANGE. Move tail INCLUDING NUL once. Zero count is a successful no-op.
 * O(old length); no erasure guarantee for discarded text. */
hps_string_status hps_string_buffer_erase(hps_string_buffer *buffer,
                                           size_t offset, size_t count);

/* Shrink only; length > current length => OUT_OF_RANGE. Clear is truncate(0).
 * O(old length) validation, then one NUL write. Discarded bytes are not wiped. */
hps_string_status hps_string_buffer_truncate(hps_string_buffer *buffer,
                                              size_t length);
hps_string_status hps_string_buffer_clear(hps_string_buffer *buffer);

/* Borrow active text as a normalized view, or a const C string (including empty
 * storage's terminator). Mutation/rebinding/release invalidates borrowed results.
 * NULL output => INVALID_ARGUMENT. Bad state => INVALID_STATE.
 * remaining reports capacity-length-1 payload space. O(length) validation. */
hps_string_status hps_string_buffer_as_view(const hps_string_buffer *buffer,
                                             hps_string_view *result);
hps_string_status hps_string_buffer_cstr(const hps_string_buffer *buffer,
                                          const char **result);
hps_string_status hps_string_buffer_remaining(const hps_string_buffer *buffer,
                                               size_t *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_STRING_BUFFER_H */
