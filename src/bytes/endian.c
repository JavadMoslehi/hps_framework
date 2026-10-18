#include "hps/bytes/endian.h"
#include "hps/bytes/codec.h"

/**
 * @file src/bytes/endian.c
 * @brief Explicit octet encoding with checked reads and all-or-nothing appends.
 */

/* slice proves each field's extent before access. Each octet is widened before
 * shifting; shifts are below the operand width and the occupied bit lanes are
 * disjoint. The u16 composite is <=UINT16_MAX before its final narrowing cast.
 * Append masks prove each narrowed octet is <=UINT8_MAX. */

/* ========================================================================= */
/*                               READ WRAPPERS                               */
/* ========================================================================= */

int32_t hps_byte_view_read_u16_le(const hps_byte_view view,
                                  const size_t offset, uint16_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 2U, &field);
        if (status == HPS_BYTE_OK) {
            *result = hps_codec_bytes_to_u16(field.data, HPS_BYTE_ORDER_LITTLE_ENDIAN);
        }
    }
    return status;
}

int32_t hps_byte_view_read_u16_be(const hps_byte_view view,
                                  const size_t offset, uint16_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 2U, &field);
        if (status == HPS_BYTE_OK) {
            *result = hps_codec_bytes_to_u16(field.data, HPS_BYTE_ORDER_BIG_ENDIAN);
        }
    }
    return status;
}

int32_t hps_byte_view_read_u32_le(const hps_byte_view view,
                                  const size_t offset, uint32_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 4U, &field);
        if (status == HPS_BYTE_OK) {
            *result = hps_codec_bytes_to_u32(field.data, HPS_BYTE_ORDER_LITTLE_ENDIAN);
        }
    }
    return status;
}

int32_t hps_byte_view_read_u32_be(const hps_byte_view view,
                                  const size_t offset, uint32_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 4U, &field);
        if (status == HPS_BYTE_OK) {
            *result = hps_codec_bytes_to_u32(field.data, HPS_BYTE_ORDER_BIG_ENDIAN);
        }
    }
    return status;
}

int32_t hps_byte_view_read_u64_le(const hps_byte_view view,
                                  const size_t offset, uint64_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 8U, &field);
        if (status == HPS_BYTE_OK) {
            *result = hps_codec_bytes_to_u64(field.data, HPS_BYTE_ORDER_LITTLE_ENDIAN);
        }
    }
    return status;
}

int32_t hps_byte_view_read_u64_be(const hps_byte_view view,
                                  const size_t offset, uint64_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_byte_view field = {NULL, 0U};
        status = hps_byte_view_slice(view, offset, 8U, &field);
        if (status == HPS_BYTE_OK) {
            *result = hps_codec_bytes_to_u64(field.data, HPS_BYTE_ORDER_BIG_ENDIAN);
        }
    }
    return status;
}

/* ========================================================================= */
/*                              APPEND WRAPPERS                              */
/* ========================================================================= */

int32_t hps_byte_buffer_append_u16_le(hps_byte_buffer *buffer,
                                      const uint16_t value)
{
    uint8_t encoded[2];
    hps_codec_u16_to_bytes(encoded, value, HPS_BYTE_ORDER_LITTLE_ENDIAN);
    const hps_byte_view field = {encoded, 2U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

int32_t hps_byte_buffer_append_u16_be(hps_byte_buffer *buffer,
                                      const uint16_t value)
{
    uint8_t encoded[2];
    hps_codec_u16_to_bytes(encoded, value, HPS_BYTE_ORDER_BIG_ENDIAN);
    const hps_byte_view field = {encoded, 2U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

int32_t hps_byte_buffer_append_u32_le(hps_byte_buffer *buffer,
                                      const uint32_t value)
{
    uint8_t encoded[4];
    hps_codec_u32_to_bytes(encoded, value, HPS_BYTE_ORDER_LITTLE_ENDIAN);
    const hps_byte_view field = {encoded, 4U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

int32_t hps_byte_buffer_append_u32_be(hps_byte_buffer *buffer,
                                      const uint32_t value)
{
    uint8_t encoded[4];
    hps_codec_u32_to_bytes(encoded, value, HPS_BYTE_ORDER_BIG_ENDIAN);
    const hps_byte_view field = {encoded, 4U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

int32_t hps_byte_buffer_append_u64_le(hps_byte_buffer *buffer,
                                      const uint64_t value)
{
    uint8_t encoded[8];
    hps_codec_u64_to_bytes(encoded, value, HPS_BYTE_ORDER_LITTLE_ENDIAN);
    const hps_byte_view field = {encoded, 8U};
    return hps_byte_buffer_append_bytes(buffer, field);
}

int32_t hps_byte_buffer_append_u64_be(hps_byte_buffer *buffer,
                                      const uint64_t value)
{
    uint8_t encoded[8];
    hps_codec_u64_to_bytes(encoded, value, HPS_BYTE_ORDER_BIG_ENDIAN);
    const hps_byte_view field = {encoded, 8U};
    return hps_byte_buffer_append_bytes(buffer, field);
}
