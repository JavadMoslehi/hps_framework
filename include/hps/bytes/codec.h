#ifndef HPS_BYTES_CODEC_H
#define HPS_BYTES_CODEC_H

/**
 * @file hps/bytes/codec.h
 * @brief Fixed-width unsigned integer conversion with explicit byte order.
 */

#include "hps/base/hps_stdint.h"
#include "hps/bytes/order.h"

typedef uint8_t hps_codec_type;
#define HPS_CODEC_TYPE_ENCODE ((hps_codec_type)UINT8_C(0))
#define HPS_CODEC_TYPE_DECODE ((hps_codec_type)UINT8_C(1))

#ifdef __cplusplus
extern "C" {
#endif

/* Shared contract: exact-width uint8_t/uint16_t/uint32_t/uint64_t must exist.
 * The caller owns all storage and guarantees its extent and lifetime during
 * the call; array parameters do not validate capacity. Byte storage needs no
 * uint16_t/uint32_t/uint64_t alignment; uint8_t elements are accessed. No pointer is
 * retained and no allocation, global state, blocking or synchronization is used.
 * Caller serializes access against conflicting storage access or release.
 * Each call performs fixed work bounded by eight byte reads or writes, with no
 * local payload array; target stack and runtime helpers remain unverified.
 */

/* Read exactly 2, 4 or 8 initialized, readable bytes from non-NULL src.
 * order selects little or big endian. NULL src or an unsupported order returns
 * zero without accessing storage; a valid zero value has the same result.
 * No input is modified. Use the checked byte-view APIs when extent validation
 * or a distinguishable error is required.
 */
uint16_t hps_codec_bytes_to_u16(const uint8_t src[2], hps_byte_order order);
uint32_t hps_codec_bytes_to_u32(const uint8_t src[4], hps_byte_order order);
uint64_t hps_codec_bytes_to_u64(const uint8_t src[8], hps_byte_order order);

/* Write exactly 2, 4 or 8 bytes to writable non-NULL dst; prior initialization
 * is unnecessary and bytes outside the field are preserved. All uintN_t values
 * are supported. NULL dst or an unsupported order causes no write. value and
 * order are passed by value, with no separate output/input alias restriction.
 * Use the checked byte-buffer APIs when capacity validation is required.
 */
void hps_codec_u16_to_bytes(uint8_t dst[2], uint16_t value, hps_byte_order order);
void hps_codec_u32_to_bytes(uint8_t dst[4], uint32_t value, hps_byte_order order);
void hps_codec_u64_to_bytes(uint8_t dst[8], uint64_t value, hps_byte_order order);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_BYTES_CODEC_H */
