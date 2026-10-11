
#include <stdint.h>
#include <stddef.h>

/* ========================================================================= */
/*                          DECODE (Bytes -> Integers)                       */
/* ========================================================================= */

static inline uint16_t hps_decode_u16_le(const uint8_t src[2])
{
    return (uint16_t)((uint32_t)src[0] |
                     ((uint32_t)src[1] << 8U));
}

static inline uint16_t hps_decode_u16_be(const uint8_t src[2])
{
    return (uint16_t)(((uint32_t)src[0] << 8U) |
                       (uint32_t)src[1]);
}

static inline uint32_t hps_decode_u32_le(const uint8_t src[4])
{
    return  (uint32_t)src[0] |
           ((uint32_t)src[1] << 8U) |
           ((uint32_t)src[2] << 16U) |
           ((uint32_t)src[3] << 24U);
}

static inline uint32_t hps_decode_u32_be(const uint8_t src[4])
{
    return ((uint32_t)src[0] << 24U) |
           ((uint32_t)src[1] << 16U) |
           ((uint32_t)src[2] << 8U) |
            (uint32_t)src[3];
}

static inline uint64_t hps_decode_u64_le(const uint8_t src[8])
{
    return  (uint64_t)src[0] |
           ((uint64_t)src[1] << 8U) |
           ((uint64_t)src[2] << 16U) |
           ((uint64_t)src[3] << 24U) |
           ((uint64_t)src[4] << 32U) |
           ((uint64_t)src[5] << 40U) |
           ((uint64_t)src[6] << 48U) |
           ((uint64_t)src[7] << 56U);
}

static inline uint64_t hps_decode_u64_be(const uint8_t src[8])
{
    return ((uint64_t)src[0] << 56U) |
           ((uint64_t)src[1] << 48U) |
           ((uint64_t)src[2] << 40U) |
           ((uint64_t)src[3] << 32U) |
           ((uint64_t)src[4] << 24U) |
           ((uint64_t)src[5] << 16U) |
           ((uint64_t)src[6] << 8U) |
            (uint64_t)src[7];
}

/* ========================================================================= */
/*                          ENCODE (Integers -> Bytes)                       */
/* ========================================================================= */

static inline void hps_encode_u16_le(uint8_t dst[2], const uint16_t value)
{
    dst[0] = (uint8_t)(value & UINT16_C(0xFF));
    dst[1] = (uint8_t)((value >> 8U) & UINT16_C(0xFF));
}

static inline void hps_encode_u16_be(uint8_t dst[2], const uint16_t value)
{
    dst[0] = (uint8_t)((value >> 8U) & UINT16_C(0xFF));
    dst[1] = (uint8_t)(value & UINT16_C(0xFF));
}

static inline void hps_encode_u32_le(uint8_t dst[4], const uint32_t value)
{
    dst[0] = (uint8_t)(value & UINT32_C(0xFF));
    dst[1] = (uint8_t)((value >> 8U) & UINT32_C(0xFF));
    dst[2] = (uint8_t)((value >> 16U) & UINT32_C(0xFF));
    dst[3] = (uint8_t)((value >> 24U) & UINT32_C(0xFF));
}

static inline void hps_encode_u32_be(uint8_t dst[4], const uint32_t value)
{
    dst[0] = (uint8_t)((value >> 24U) & UINT32_C(0xFF));
    dst[1] = (uint8_t)((value >> 16U) & UINT32_C(0xFF));
    dst[2] = (uint8_t)((value >> 8U) & UINT32_C(0xFF));
    dst[3] = (uint8_t)(value & UINT32_C(0xFF));
}

static inline void hps_encode_u64_le(uint8_t dst[8], const uint64_t value)
{
    dst[0] = (uint8_t)(value & UINT64_C(0xFF));
    dst[1] = (uint8_t)((value >> 8U) & UINT64_C(0xFF));
    dst[2] = (uint8_t)((value >> 16U) & UINT64_C(0xFF));
    dst[3] = (uint8_t)((value >> 24U) & UINT64_C(0xFF));
    dst[4] = (uint8_t)((value >> 32U) & UINT64_C(0xFF));
    dst[5] = (uint8_t)((value >> 40U) & UINT64_C(0xFF));
    dst[6] = (uint8_t)((value >> 48U) & UINT64_C(0xFF));
    dst[7] = (uint8_t)((value >> 56U) & UINT64_C(0xFF));
}

static inline void hps_encode_u64_be(uint8_t dst[8], const uint64_t value)
{
    dst[0] = (uint8_t)((value >> 56U) & UINT64_C(0xFF));
    dst[1] = (uint8_t)((value >> 48U) & UINT64_C(0xFF));
    dst[2] = (uint8_t)((value >> 40U) & UINT64_C(0xFF));
    dst[3] = (uint8_t)((value >> 32U) & UINT64_C(0xFF));
    dst[4] = (uint8_t)((value >> 24U) & UINT64_C(0xFF));
    dst[5] = (uint8_t)((value >> 16U) & UINT64_C(0xFF));
    dst[6] = (uint8_t)((value >> 8U) & UINT64_C(0xFF));
    dst[7] = (uint8_t)(value & UINT64_C(0xFF));
}
