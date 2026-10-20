#include "hps/bytes/codec.h"
#include "hps/base/attributes.h"

/**
 * @file src/bytes/codec.c
 * @brief Fixed-width integer codecs over caller-proven byte extents.
 */

#include "hps/base/hps_stddef.h"



/* Widen before shifting into disjoint bit lanes. The u16 composite fits before
 * narrowing; encoding masks bound each byte by UINT8_MAX. Width-specific helpers
 * keep the operand types and fixed 2/4/8-byte storage requirements explicit. */
static HPS_INLINE uint16_t hps_decode_u16_le(const uint8_t src[2])
{
    return (uint16_t)((uint32_t)src[0] |
                     ((uint32_t)src[1] << 8U));
}

static HPS_INLINE uint16_t hps_decode_u16_be(const uint8_t src[2])
{
    return (uint16_t)(((uint32_t)src[0] << 8U) |
                       (uint32_t)src[1]);
}

uint16_t hps_codec_bytes_to_u16(const uint8_t src[2], hps_byte_order order)
{
    uint16_t result = UINT16_C(0);
    if (src != NULL) {
        switch (order) {
        case HPS_BYTE_ORDER_BIG_ENDIAN:
            result = hps_decode_u16_be(src);
            break;
        case HPS_BYTE_ORDER_LITTLE_ENDIAN:
            result = hps_decode_u16_le(src);
            break;
        default:
            break;
        }
    }
    return result;
}

static HPS_INLINE uint32_t hps_decode_u32_le(const uint8_t src[4])
{
    return  (uint32_t)(src[0] << 0U)|
           ((uint32_t)src[1] << 8U) |
           ((uint32_t)src[2] << 16U) |
           ((uint32_t)src[3] << 24U);
}

static HPS_INLINE uint32_t hps_decode_u32_be(const uint8_t src[4])
{
    return ((uint32_t)src[0] << 24U) |
           ((uint32_t)src[1] << 16U) |
           ((uint32_t)src[2] << 8U) |
            ((uint32_t)src[3]<< 0U);
}

uint32_t hps_codec_bytes_to_u32(const uint8_t src[4], hps_byte_order order)
{
    uint32_t result = UINT32_C(0);
    if (src != NULL) {
        switch (order) {
        case HPS_BYTE_ORDER_BIG_ENDIAN:
            result = hps_decode_u32_be(src);
            break;
        case HPS_BYTE_ORDER_LITTLE_ENDIAN:
            result = hps_decode_u32_le(src);
            break;
        default:
            break;
        }
    }
    return result;
}

static HPS_INLINE uint64_t hps_decode_u64_le(const uint8_t src[8])
{
    return  ((uint64_t)src[0] << 0U)|
           ((uint64_t)src[1] << 8U) |
           ((uint64_t)src[2] << 16U) |
           ((uint64_t)src[3] << 24U) |
           ((uint64_t)src[4] << 32U) |
           ((uint64_t)src[5] << 40U) |
           ((uint64_t)src[6] << 48U) |
           ((uint64_t)src[7] << 56U);
}

static HPS_INLINE uint64_t hps_decode_u64_be(const uint8_t src[8])
{
    return ((uint64_t)src[0] << 56U) |
           ((uint64_t)src[1] << 48U) |
           ((uint64_t)src[2] << 40U) |
           ((uint64_t)src[3] << 32U) |
           ((uint64_t)src[4] << 24U) |
           ((uint64_t)src[5] << 16U) |
           ((uint64_t)src[6] << 8U) |
            ((uint64_t)src[7]<< 0U);
}

uint64_t hps_codec_bytes_to_u64(const uint8_t src[8], hps_byte_order order)
{
    uint64_t result = UINT64_C(0);
    if (src != NULL) {
        switch (order) {
        case HPS_BYTE_ORDER_BIG_ENDIAN:
            result = hps_decode_u64_be(src);
            break;
        case HPS_BYTE_ORDER_LITTLE_ENDIAN:
            result = hps_decode_u64_le(src);
            break;
        default:
            break;
        }
    }
    return result;
}

/* ========================================================================= */
/*                          ENCODE (Integers -> Bytes)                       */
/* ========================================================================= */

static HPS_INLINE void hps_encode_u16_le(uint8_t dst[2], const uint16_t value)
{
    dst[0] = (uint8_t)((value>> 0U) & UINT16_C(0xFF));
    dst[1] = (uint8_t)((value >> 8U) & UINT16_C(0xFF));
}

static HPS_INLINE void hps_encode_u16_be(uint8_t dst[2], const uint16_t value)
{
    dst[0] = (uint8_t)((value >> 8U) & UINT16_C(0xFF));
    dst[1] = (uint8_t)((value>> 0U) & UINT16_C(0xFF));
}

void hps_codec_u16_to_bytes(uint8_t dst[2], uint16_t value, hps_byte_order order)
{
    if (dst != NULL) {
        switch (order) {
        case HPS_BYTE_ORDER_BIG_ENDIAN:
            hps_encode_u16_be(dst, value);
            break;
        case HPS_BYTE_ORDER_LITTLE_ENDIAN:
            hps_encode_u16_le(dst, value);
            break;
        default:
            break;
        }
    }
}

static HPS_INLINE void hps_encode_u32_le(uint8_t dst[4], const uint32_t value)
{
    dst[0] = (uint8_t)((value>> 0U) & UINT32_C(0xFF));
    dst[1] = (uint8_t)((value >> 8U) & UINT32_C(0xFF));
    dst[2] = (uint8_t)((value >> 16U) & UINT32_C(0xFF));
    dst[3] = (uint8_t)((value >> 24U) & UINT32_C(0xFF));
}

static HPS_INLINE void hps_encode_u32_be(uint8_t dst[4], const uint32_t value)
{
    dst[0] = (uint8_t)((value >> 24U) & UINT32_C(0xFF));
    dst[1] = (uint8_t)((value >> 16U) & UINT32_C(0xFF));
    dst[2] = (uint8_t)((value >> 8U) & UINT32_C(0xFF));
    dst[3] = (uint8_t)((value>> 0U) & UINT32_C(0xFF));
}

void hps_codec_u32_to_bytes(uint8_t dst[4], uint32_t value, hps_byte_order order)
{
    if (dst != NULL) {
        switch (order) {
        case HPS_BYTE_ORDER_BIG_ENDIAN:
            hps_encode_u32_be(dst, value);
            break;
        case HPS_BYTE_ORDER_LITTLE_ENDIAN:
            hps_encode_u32_le(dst, value);
            break;
        default:
            break;
        }
    }
}

static HPS_INLINE void hps_encode_u64_le(uint8_t dst[8], const uint64_t value)
{
    dst[0] = (uint8_t)((value >> 0U) & UINT64_C(0xFF));
    dst[1] = (uint8_t)((value >> 8U) & UINT64_C(0xFF));
    dst[2] = (uint8_t)((value >> 16U) & UINT64_C(0xFF));
    dst[3] = (uint8_t)((value >> 24U) & UINT64_C(0xFF));
    dst[4] = (uint8_t)((value >> 32U) & UINT64_C(0xFF));
    dst[5] = (uint8_t)((value >> 40U) & UINT64_C(0xFF));
    dst[6] = (uint8_t)((value >> 48U) & UINT64_C(0xFF));
    dst[7] = (uint8_t)((value >> 56U) & UINT64_C(0xFF));
}

static HPS_INLINE void hps_encode_u64_be(uint8_t dst[8], const uint64_t value)
{
    dst[0] = (uint8_t)((value >> 56U) & UINT64_C(0xFF));
    dst[1] = (uint8_t)((value >> 48U) & UINT64_C(0xFF));
    dst[2] = (uint8_t)((value >> 40U) & UINT64_C(0xFF));
    dst[3] = (uint8_t)((value >> 32U) & UINT64_C(0xFF));
    dst[4] = (uint8_t)((value >> 24U) & UINT64_C(0xFF));
    dst[5] = (uint8_t)((value >> 16U) & UINT64_C(0xFF));
    dst[6] = (uint8_t)((value >> 8U) & UINT64_C(0xFF));
    dst[7] = (uint8_t)((value >> 0U) & UINT64_C(0xFF));
}

void hps_codec_u64_to_bytes(uint8_t dst[8], uint64_t value, hps_byte_order order)
{
    if (dst != NULL) {
        switch (order) {
        case HPS_BYTE_ORDER_BIG_ENDIAN:
            hps_encode_u64_be(dst, value);
            break;
        case HPS_BYTE_ORDER_LITTLE_ENDIAN:
            hps_encode_u64_le(dst, value);
            break;
        default:
            break;
        }
    }
}
