#include "hps/bytes/endian.h"

/**
 * @file src/bytes/endian.c
 * @brief Explicit octet encoding with checked reads and all-or-nothing appends.
 */

/* slice proves each field's extent before access. Each octet is widened before
 * shifting; shifts are below the operand width and the occupied bit lanes are
 * disjoint. The u16 composite is <=UINT16_MAX before its final narrowing cast.
 * Append masks prove each narrowed octet is <=UINT8_MAX. */
#include "hps/bytes/endian.h"

/* ========================================================================= */
/*                               READ WRAPPERS                               */
/* ========================================================================= */

hps_byte_status hps_byte_view_read_u16_le(const hps_byte_view view,
                                         const size_t offset, uint16_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 2U, &field);
        if (status == HPS_BYTE_OK) {
            *result = hps_decode_u16_le(field.data);
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
            *result = hps_decode_u16_be(field.data);
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
            *result = hps_decode_u32_le(field.data);
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
            *result = hps_decode_u32_be(field.data);
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
            *result = hps_decode_u64_le(field.data);
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
            *result = hps_decode_u64_be(field.data);
        }
    }
    return status;
}

/* ========================================================================= */
/*                              APPEND WRAPPERS                              */
/* ========================================================================= */

hps_byte_status hps_byte_buffer_append_u16_le(hps_byte_buffer *buffer,
                                             const uint16_t value)
{
    uint8_t encoded[2];
    hps_encode_u16_le(encoded, value);
    const hps_byte_view field = {encoded, 2U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u16_be(hps_byte_buffer *buffer,
                                             const uint16_t value)
{
    uint8_t encoded[2];
    hps_encode_u16_be(encoded, value);
    const hps_byte_view field = {encoded, 2U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u32_le(hps_byte_buffer *buffer,
                                             const uint32_t value)
{
    uint8_t encoded[4];
    hps_encode_u32_le(encoded, value);
    const hps_byte_view field = {encoded, 4U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u32_be(hps_byte_buffer *buffer,
                                             const uint32_t value)
{
    uint8_t encoded[4];
    hps_encode_u32_be(encoded, value);
    const hps_byte_view field = {encoded, 4U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u64_le(hps_byte_buffer *buffer,
                                             const uint64_t value)
{
    uint8_t encoded[8];
    hps_encode_u64_le(encoded, value);
    const hps_byte_view field = {encoded, 8U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

hps_byte_status hps_byte_buffer_append_u64_be(hps_byte_buffer *buffer,
                                             const uint64_t value)
{
    uint8_t encoded[8];
    hps_encode_u64_be(encoded, value);
    const hps_byte_view field = {encoded, 8U};
    return hps_byte_buffer_append_bytes(buffer, field);
}
