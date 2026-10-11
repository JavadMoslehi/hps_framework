#ifndef HPS_BYTES_ENDIAN_H
#define HPS_BYTES_ENDIAN_H

/**
 * @file hps/bytes/endian.h
 * @brief Checked unsigned integer reads and appends in explicit byte order.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/bytes/view.h"
#include "hps/bytes/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    HPS_ENDIAN_LITTLE = 0,
    HPS_ENDIAN_BIG    = 1
} hps_endian_t;

/* Shared contract: exact-width uint8_t/uint16_t/uint32_t/uint64_t must exist.
 * The view/buffer ownership, extent, initialization and lifetime contracts apply.
 * LE places the least significant octet first; BE places it last. Operations
 * access uint8_t elements directly, with no native-order/alignment dependency or
 * typed-object casts. No cursor, allocation, pool state or synchronization.
 * Caller serializes access against backing-storage mutation/release.
 * All failures leave payloads, descriptors and scalar outputs unchanged.
 */

/* Reads exactly 2, 4 or 8 initialized bytes at offset into one writable result
 * of the declared type. result must not overlap the view descriptor or backing
 * payload. NULL result or NULL view.data with nonzero length => INVALID_ARGUMENT.
 * offset > length or field width > length-offset => OUT_OF_RANGE; bounds are
 * checked before pointer formation/access. No cursor or descriptor is modified.
 * O(1), bounded by eight byte reads; no explicit libc call in the read path.
 * Compiler-generated runtime helpers and actual stack need target qualification.
 */
hps_byte_status hps_byte_view_read_u16_le(hps_byte_view view, size_t offset,
                                        uint16_t *result);
hps_byte_status hps_byte_view_read_u16_be(hps_byte_view view, size_t offset,
                                        uint16_t *result);
hps_byte_status hps_byte_view_read_u32_le(hps_byte_view view, size_t offset,
                                        uint32_t *result);
hps_byte_status hps_byte_view_read_u32_be(hps_byte_view view, size_t offset,
                                        uint32_t *result);
hps_byte_status hps_byte_view_read_u64_le(hps_byte_view view, size_t offset,
                                        uint64_t *result);
hps_byte_status hps_byte_view_read_u64_be(hps_byte_view view, size_t offset,
                                        uint64_t *result);

/* Appends exactly 2, 4 or 8 bytes to an initialized buffer; active prefix and
 * unused bytes after the field are preserved. NULL buffer => INVALID_ARGUMENT;
 * invalid buffer state => INVALID_STATE; insufficient remaining capacity =>
 * NO_SPACE with no partial append. All uintN_t values are supported.
 * Delegates once to buffer append_bytes (memmove).
 * O(1), at most eight bytes encoded/copied, at most eight local payload bytes
 * plus a local view and call state; actual stack/runtime helpers need target
 * qualification. The encoded local array does not escape the synchronous call.
 */
hps_byte_status hps_byte_buffer_append_u16_le(hps_byte_buffer *buffer,
                                            uint16_t value);
hps_byte_status hps_byte_buffer_append_u16_be(hps_byte_buffer *buffer,
                                            uint16_t value);
hps_byte_status hps_byte_buffer_append_u32_le(hps_byte_buffer *buffer,
                                            uint32_t value);
hps_byte_status hps_byte_buffer_append_u32_be(hps_byte_buffer *buffer,
                                            uint32_t value);
hps_byte_status hps_byte_buffer_append_u64_le(hps_byte_buffer *buffer,
                                            uint64_t value);
hps_byte_status hps_byte_buffer_append_u64_be(hps_byte_buffer *buffer,
                                            uint64_t value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_BYTES_ENDIAN_H */
