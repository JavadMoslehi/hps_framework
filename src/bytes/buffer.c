#include "hps/bytes/hps_buffer.h"
#include <string.h>

/**
 * @file src/bytes/buffer.c
 * @brief Fixed-capacity byte operations; memmove handles overlapping payloads.
 */

static int32_t buffer_check(const hps_byte_buffer *buffer)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (buffer != NULL) {
        status = HPS_BYTE_INVALID_STATE;
        if ((buffer->length <= buffer->capacity) &&
            ((buffer->data != NULL) || (buffer->capacity == 0U))) {
            status = HPS_BYTE_OK;
        }
    }
    return status;
}

/* offset/size ranges are checked by callers before forming destination pointers.
 * Standard memmove is the sole runtime library dependency; qualify its target
 * implementation/linked symbols for the whole-product no-heap requirement. */
static void buffer_copy(uint8_t *destination, const uint8_t *source,
                        const size_t length)
{
    (void)memmove(destination, source, length);
}

int32_t hps_byte_buffer_init(hps_byte_buffer *buffer,
                             uint8_t *storage, const size_t capacity)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if ((buffer != NULL) && ((storage != NULL) || (capacity == 0U))) {
        buffer->data = storage;
        buffer->length = 0U;
        buffer->capacity = capacity;
        status = HPS_BYTE_OK;
    }
    return status;
}

int32_t hps_byte_buffer_assign_bytes(hps_byte_buffer *buffer,
                                     const hps_byte_view source)
{
    int32_t status = buffer_check(buffer);
    if (status == HPS_BYTE_OK) {
        if ((source.data == NULL) && (source.length != 0U)) {
            status = HPS_BYTE_INVALID_ARGUMENT;
        } else if (source.length > buffer->capacity) {
            status = HPS_BYTE_NO_SPACE;
        } else {
            if (source.length != 0U) {
                buffer_copy(buffer->data, source.data, source.length);
            }
            buffer->length = source.length;
        }
    }
    return status;
}

int32_t hps_byte_buffer_append_bytes(hps_byte_buffer *buffer,
                                     const hps_byte_view source)
{
    int32_t status = buffer_check(buffer);
    if (status == HPS_BYTE_OK) {
        if ((source.data == NULL) && (source.length != 0U)) {
            status = HPS_BYTE_INVALID_ARGUMENT;
        } else if (source.length > (buffer->capacity - buffer->length)) {
            status = HPS_BYTE_NO_SPACE;
        } else {
            if (source.length != 0U) {
                buffer_copy(&buffer->data[buffer->length], source.data,
                            source.length);
                buffer->length += source.length;
            }
        }
    }
    return status;
}

int32_t hps_byte_buffer_append_u8(hps_byte_buffer *buffer,
                                  const uint8_t value)
{
    int32_t status = buffer_check(buffer);
    if (status == HPS_BYTE_OK) {
        if (buffer->length == buffer->capacity) {
            status = HPS_BYTE_NO_SPACE;
        } else {
            buffer->data[buffer->length] = value;
            buffer->length += 1U;
        }
    }
    return status;
}

int32_t hps_byte_buffer_pop_u8(hps_byte_buffer *buffer, uint8_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;

    if (result != NULL) {
        status = buffer_check(buffer);
        if (status == HPS_BYTE_OK) {
            if (buffer->length == 0U) {
                status = HPS_BYTE_OUT_OF_RANGE;
            } else {
                const size_t last = buffer->length - 1U;
                *result = buffer->data[last];
                buffer->length = last;
            }
        }
    }
    return status;
}

int32_t hps_byte_buffer_truncate(hps_byte_buffer *buffer,
                                 const size_t length)
{
    int32_t status = buffer_check(buffer);
    if (status == HPS_BYTE_OK) {
        if (length > buffer->length) {
            status = HPS_BYTE_OUT_OF_RANGE;
        } else {
            buffer->length = length;
        }
    }
    return status;
}

int32_t hps_byte_buffer_clear(hps_byte_buffer *buffer)
{
    return hps_byte_buffer_truncate(buffer, 0U);
}

int32_t hps_byte_buffer_as_view(const hps_byte_buffer *buffer,
                                hps_byte_view *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = buffer_check(buffer);
        if (status == HPS_BYTE_OK) {
            result->data = (buffer->length == 0U) ? NULL : buffer->data;
            result->length = buffer->length;
        }
    }
    return status;
}
