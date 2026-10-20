#ifndef HPS_BYTES_READER_H
#define HPS_BYTES_READER_H

/**
 * @file hps/bytes/reader.h
 * @brief Borrowed sequential byte reader with a checked, failure-preserved cursor.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/bytes/view.h"

typedef struct {
    hps_byte_view view;
    size_t position;
} hps_byte_reader;

#ifdef __cplusplus
extern "C" {
#endif

/* Module contract: initialize before use; position <= view.length and nonempty
 * view storage covers view.length initialized, readable uint8_t elements in one
 * live caller-owned array. Reader metadata must not overlap backing payload.
 * Every scalar/view output designates one writable object of its declared type
 * and must not overlap ANY part of the reader or its backing payload.
 * Do not mutate reader fields directly. Bounds/state checks do not prove real
 * array extent, initialization or lifetime, and do not validate arbitrary pointers.
 * Readers and returned views borrow storage; backing mutation/rebind/release
 * invalidates them under the view/buffer lifetime contract. Caller serializes
 * access; separate readers may share immutable live bytes with independent cursors.
 * No allocation, pool/OS dependency, payload copy, global state or synchronization.
 * No protocol/schema/authentication, secret erasure or constant-time guarantee.
 * All failures preserve reader metadata, backing bytes and outputs.
 */

/* Initializes with position zero; empty input is normalized to {NULL, 0}.
 * NULL reader or NULL view.data with nonzero length => INVALID_ARGUMENT.
 * Input view is passed by value, so init(reader, reader->view) is allowed for an
 * already initialized reader; success restarts it, failure preserves it. O(1).
 */
int32_t hps_byte_reader_init(hps_byte_reader *reader, hps_byte_view view);

/* For all following APIs, NULL reader or required output => INVALID_ARGUMENT
 * (NULL output checked first). Non-NULL initialized metadata with position >
 * view.length or NULL view.data/nonzero length => INVALID_STATE. No recovery is
 * promised for indeterminate/uninitialized metadata. O(1) validation.
 */

/* Writes view.length-position without moving the cursor. */
int32_t hps_byte_reader_remaining(const hps_byte_reader *reader,
                                  size_t *result);

/* Skips length bytes of the existing view without reading payload. Requires
 * length <= view.length-position; otherwise OUT_OF_RANGE. Zero is valid at EOF.
 * On success position advances by length. O(1), no pointer arithmetic.
 */
int32_t hps_byte_reader_skip(hps_byte_reader *reader, size_t length);

/* Writes a borrowed slice of length bytes at the current position, then advances
 * by length. Same range contract as skip. Empty result is {NULL, 0}, including
 * at EOF/empty input, without pointer arithmetic. O(1), no payload access/copy.
 */
int32_t hps_byte_reader_take_view(hps_byte_reader *reader, size_t length,
                                  hps_byte_view *result);

/* Reads 1/2/4/8 bytes at the current position and advances only on success.
 * Insufficient remaining bytes => OUT_OF_RANGE with unchanged cursor/output.
 * u16/u32/u64 require the corresponding exact-width types and use explicit LE/BE
 * decoding from endian.c; no wider object-pointer casts or native-order assumption.
 * O(1), at most eight byte reads. No explicit libc call; compiler runtime helpers,
 * stack consumption and target latency still require qualification.
 */
int32_t hps_byte_reader_read_u8(hps_byte_reader *reader, uint8_t *result);
int32_t hps_byte_reader_read_u16_le(hps_byte_reader *reader,
                                    uint16_t *result);
int32_t hps_byte_reader_read_u16_be(hps_byte_reader *reader,
                                    uint16_t *result);
int32_t hps_byte_reader_read_u32_le(hps_byte_reader *reader,
                                    uint32_t *result);
int32_t hps_byte_reader_read_u32_be(hps_byte_reader *reader,
                                    uint32_t *result);
int32_t hps_byte_reader_read_u64_le(hps_byte_reader *reader,
                                    uint64_t *result);
int32_t hps_byte_reader_read_u64_be(hps_byte_reader *reader,
                                    uint64_t *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_BYTES_READER_H */
