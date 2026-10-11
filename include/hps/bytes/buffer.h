#ifndef HPS_BYTES_BUFFER_H
#define HPS_BYTES_BUFFER_H

/**
 * @file hps/bytes/buffer.h
 * @brief Borrowed fixed-capacity mutable byte buffers with checked operations.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/bytes/view.h"

typedef struct {
    uint8_t *data;
    size_t length;
    size_t capacity;
} hps_byte_buffer;

#ifdef __cplusplus
extern "C" {
#endif

/* Module contract: buffer refers to capacity writable uint8_t elements in one
 * live caller-owned array; length <= capacity and active bytes are initialized.
 * NULL data is valid only with zero capacity. Do not mutate descriptor fields
 * directly. Descriptor/output objects must not overlap backing/source payloads.
 * Call init before other APIs. A buffer borrows storage, never allocates or frees
 * it and never calls the pool. No NUL/encoding/typed-object reinterpretation.
 * Serialize access against mutation/release; no locks or concurrency guarantees.
 * All failures preserve descriptor, payload and outputs. Validation cannot prove
 * pointer validity, actual array extent or stale lifetime. Automatic storage is
 * bounded; no retries, callbacks, recursion or variable-length arrays.
 */

/* Binds storage with length zero, without reading/clearing bytes. Can rebind a
 * descriptor after invalidating all old views. INVALID_ARGUMENT on NULL buffer
 * or NULL storage with positive capacity. O(1). */
hps_byte_status hps_byte_buffer_init(hps_byte_buffer *buffer,
                                     uint8_t *storage, size_t capacity);

/* Copies source bytes into existing storage, replacing active bytes/length.
 * Overlapping payloads use memmove semantics.
 * Source length <= capacity, else NO_SPACE.
 * INVALID_ARGUMENT on NULL buffer/malformed source; INVALID_STATE on malformed
 * initialized buffer. O(source.length), no byte copy for an empty source. */
hps_byte_status hps_byte_buffer_assign_bytes(hps_byte_buffer *buffer,
                                            hps_byte_view source);

/* Copies source bytes after the active prefix, including an overlapping/self
 * view, with memmove semantics.
 * source.length <= capacity-length, else NO_SPACE; no partial append.
 * Same argument/state errors as assign_bytes. O(source.length). */
hps_byte_status hps_byte_buffer_append_bytes(hps_byte_buffer *buffer,
                                            hps_byte_view source);

/* Appends one byte; NO_SPACE when full. INVALID_ARGUMENT on NULL buffer;
 * INVALID_STATE on malformed state. O(1). */
hps_byte_status hps_byte_buffer_append_u8(hps_byte_buffer *buffer, uint8_t value);

/* Shrinks length only; requested length > current length gives OUT_OF_RANGE.
 * No byte erase; init-required argument/state errors as above. O(1). */
hps_byte_status hps_byte_buffer_truncate(hps_byte_buffer *buffer, size_t length);

/* Sets length zero without erasure; same argument/state errors. O(1). */
hps_byte_status hps_byte_buffer_clear(hps_byte_buffer *buffer);

/* Writes a view of initialized active bytes, normalizing empty to {NULL, 0}.
 * result points to one writable view descriptor, disjoint from buffer descriptor.
 * Mutation, rebind or backing storage release invalidates borrowed views under
 * this contract. NULL arguments => INVALID_ARGUMENT; bad state => INVALID_STATE.
 * O(1), no payload access. */
hps_byte_status hps_byte_buffer_as_view(const hps_byte_buffer *buffer,
                                        hps_byte_view *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_BYTES_BUFFER_H */
