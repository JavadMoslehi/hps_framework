#include "hps/bytes/endian.h"

/**
 * @file src/bytes/endian.c
 * @brief Explicit octet encoding with checked reads and all-or-nothing appends.
 */

/* slice proves each field's extent before access. Each octet is widened before
 * shifting; shifts are below the operand width and the occupied bit lanes are
 * disjoint. The u16 composite is <=UINT16_MAX before its final narrowing cast.
 * Append masks prove each narrowed octet is <=UINT8_MAX. */

hps_byte_status hps_byte_view_read_u16_le(const hps_byte_view view,
                                        const size_t offset, uint16_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 2U, &field);
        if (status == HPS_BYTE_OK) {
            *result = (uint16_t)((uint32_t)field.data[0] |
                                ((uint32_t)field.data[1] << 8U));
        }
    }
    return status;
}

hps_byte_status hps_byte_view_read_u16_be(const hps_byte_view view,
                                        const size_t offset, uint16_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 2U, &field);
        if (status == HPS_BYTE_OK) {
            *result = (uint16_t)(((uint32_t)field.data[0] << 8U) |
                                (uint32_t)field.data[1]);
        }
    }
    return status;
}

hps_byte_status hps_byte_view_read_u32_le(const hps_byte_view view,
                                        const size_t offset, uint32_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 4U, &field);
        if (status == HPS_BYTE_OK) {
            *result = (uint32_t)field.data[0] |
                      ((uint32_t)field.data[1] << 8U) |
                      ((uint32_t)field.data[2] << 16U) |
                      ((uint32_t)field.data[3] << 24U);
        }
    }
    return status;
}

hps_byte_status hps_byte_view_read_u32_be(const hps_byte_view view,
                                        const size_t offset, uint32_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 4U, &field);
        if (status == HPS_BYTE_OK) {
            *result = ((uint32_t)field.data[0] << 24U) |
                      ((uint32_t)field.data[1] << 16U) |
                      ((uint32_t)field.data[2] << 8U) |
                      (uint32_t)field.data[3];
        }
    }
    return status;
}

hps_byte_status hps_byte_view_read_u64_le(const hps_byte_view view,
                                        const size_t offset, uint64_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 8U, &field);
        if (status == HPS_BYTE_OK) {
            *result = (uint64_t)field.data[0] |
                      ((uint64_t)field.data[1] << 8U) |
                      ((uint64_t)field.data[2] << 16U) |
                      ((uint64_t)field.data[3] << 24U) |
                      ((uint64_t)field.data[4] << 32U) |
                      ((uint64_t)field.data[5] << 40U) |
                      ((uint64_t)field.data[6] << 48U) |
                      ((uint64_t)field.data[7] << 56U);
        }
    }
    return status;
}

hps_byte_status hps_byte_view_read_u64_be(const hps_byte_view view,
                                        const size_t offset, uint64_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 8U, &field);
        if (status == HPS_BYTE_OK) {
            *result = ((uint64_t)field.data[0] << 56U) |
                      ((uint64_t)field.data[1] << 48U) |
                      ((uint64_t)field.data[2] << 40U) |
                      ((uint64_t)field.data[3] << 32U) |
                      ((uint64_t)field.data[4] << 24U) |
                      ((uint64_t)field.data[5] << 16U) |
                      ((uint64_t)field.data[6] << 8U) |
                      (uint64_t)field.data[7];
        }
    }
    return status;
}

hps_byte_status hps_byte_buffer_append_u16_le(hps_byte_buffer *buffer,
                                            const uint16_t value)
{
    const uint8_t encoded[2] = {
        (uint8_t)(value & UINT16_C(0xFF)),
        (uint8_t)((value >> 8U) & UINT16_C(0xFF))
    };
    const hps_byte_view field = {encoded, 2U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u16_be(hps_byte_buffer *buffer,
                                            const uint16_t value)
{
    const uint8_t encoded[2] = {
        (uint8_t)((value >> 8U) & UINT16_C(0xFF)),
        (uint8_t)(value & UINT16_C(0xFF))
    };
    const hps_byte_view field = {encoded, 2U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u32_le(hps_byte_buffer *buffer,
                                            const uint32_t value)
{
    const uint8_t encoded[4] = {
        (uint8_t)(value & UINT32_C(0xFF)),
        (uint8_t)((value >> 8U) & UINT32_C(0xFF)),
        (uint8_t)((value >> 16U) & UINT32_C(0xFF)),
        (uint8_t)((value >> 24U) & UINT32_C(0xFF))
    };
    const hps_byte_view field = {encoded, 4U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u32_be(hps_byte_buffer *buffer,
                                            const uint32_t value)
{
    const uint8_t encoded[4] = {
        (uint8_t)((value >> 24U) & UINT32_C(0xFF)),
        (uint8_t)((value >> 16U) & UINT32_C(0xFF)),
        (uint8_t)((value >> 8U) & UINT32_C(0xFF)),
        (uint8_t)(value & UINT32_C(0xFF))
    };
    const hps_byte_view field = {encoded, 4U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u64_le(hps_byte_buffer *buffer,
                                            const uint64_t value)
{
    const uint8_t encoded[8] = {
        (uint8_t)(value & UINT64_C(0xFF)),
        (uint8_t)((value >> 8U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 16U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 24U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 32U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 40U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 48U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 56U) & UINT64_C(0xFF))
    };
    const hps_byte_view field = {encoded, 8U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u64_be(hps_byte_buffer *buffer,
                                            const uint64_t value)
{
    const uint8_t encoded[8] = {
        (uint8_t)((value >> 56U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 48U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 40U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 32U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 24U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 16U) & UINT64_C(0xFF)),
        (uint8_t)((value >> 8U) & UINT64_C(0xFF)),
        (uint8_t)(value & UINT64_C(0xFF))
    };
    const hps_byte_view field = {encoded, 8U};
    return hps_byte_buffer_append_bytes(buffer, field);
}
