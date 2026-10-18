#include "hps/string/buffer.h"

#include <string.h>
#include "internal.h"

/**
 * @file src/string/buffer.c
 * @brief Bounded caller-owned C strings with checked overlap-safe copies.
 */

static int32_t buffer_check(const hps_string_buffer *buffer)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (buffer != NULL) {
        status = HPS_STRING_INVALID_STATE;
        if ((buffer->data != NULL) && (buffer->capacity > 0U) &&
            (buffer->length < buffer->capacity)) {
            if (buffer->data[buffer->length] == '\0') {
                const hps_string_view active = {
                    buffer->data, buffer->length
                };
                if (hps_string_has_nul(active) == 0U) {
                    status = HPS_STRING_OK;
                }
            }
        }
    }
    return status;
}

/* Assign and append have the same shape/space/NUL checks and copy contract.
 * Choose the write offset only after validating the initialized descriptor.
 * Caller-selected mode is private and always exactly zero or one. */
static int32_t buffer_write(hps_string_buffer *buffer,
                            const hps_string_view source,
                            const uint8_t append)
{
    int32_t status = buffer_check(buffer);
    if (status == HPS_STRING_OK) {
        const size_t offset = (append == 1U) ? buffer->length : 0U;
        status = hps_string_check_view(source);
        if (status == HPS_STRING_OK) {
            /* offset <= length < capacity reserves one character for NUL.
             * This proves both the subtraction and later offset+length. */
            if (source.length > (buffer->capacity - offset - 1U)) {
                status = HPS_STRING_NO_SPACE;
            } else if (hps_string_has_nul(source) != 0U) {
                status = HPS_STRING_INVALID_ARGUMENT;
            } else {
                const size_t new_length = offset + source.length;
                if (source.length != 0U) {
                    (void)memmove(&buffer->data[offset], source.data,
                                  source.length);
                }
                buffer->data[new_length] = '\0';
                buffer->length = new_length;
            }
        }
    }
    return status;
}

int32_t hps_string_buffer_init(hps_string_buffer *buffer,
                               char *storage, const size_t capacity)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((buffer != NULL) && (storage != NULL) && (capacity > 0U)) {
        storage[0] = '\0';
        buffer->data = storage;
        buffer->length = 0U;
        buffer->capacity = capacity;
        status = HPS_STRING_OK;
    }
    return status;
}

int32_t hps_string_buffer_init_cstr(hps_string_buffer *buffer,
                                    char *storage,
                                    const size_t capacity)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (buffer != NULL) {
        hps_string_view active = {NULL, 0U};
        status = hps_string_view_init_cstr(&active, storage, capacity);
        if (status == HPS_STRING_OK) {
            buffer->data = storage;
            buffer->length = active.length;
            buffer->capacity = capacity;
        }
    }
    return status;
}

int32_t hps_string_buffer_assign(hps_string_buffer *buffer,
                                 const hps_string_view source)
{
    return buffer_write(buffer, source, 0U);
}

int32_t hps_string_buffer_append(hps_string_buffer *buffer,
                                 const hps_string_view source)
{
    return buffer_write(buffer, source, 1U);
}

int32_t hps_string_buffer_erase(hps_string_buffer *buffer,
                                const size_t offset,
                                const size_t count)
{
    int32_t status = buffer_check(buffer);
    if (status == HPS_STRING_OK) {
        if (offset > buffer->length) {
            status = HPS_STRING_OUT_OF_RANGE;
        } else if (count > (buffer->length - offset)) {
            status = HPS_STRING_OUT_OF_RANGE;
        } else if (count != 0U) {
            const size_t tail_offset = offset + count;
            /* tail_offset <= length < capacity. Including the NUL cannot
             * overflow: length is strictly less than capacity <= SIZE_MAX. */
            const size_t tail_count = buffer->length - tail_offset + 1U;
            (void)memmove(&buffer->data[offset], &buffer->data[tail_offset],
                          tail_count);
            buffer->length -= count;
        } else {
            /* Empty erase preserves every payload character. */
        }
    }
    return status;
}

int32_t hps_string_buffer_truncate(hps_string_buffer *buffer,
                                   const size_t length)
{
    int32_t status = buffer_check(buffer);
    if (status == HPS_STRING_OK) {
        if (length > buffer->length) {
            status = HPS_STRING_OUT_OF_RANGE;
        } else {
            buffer->data[length] = '\0';
            buffer->length = length;
        }
    }
    return status;
}

int32_t hps_string_buffer_clear(hps_string_buffer *buffer)
{
    return hps_string_buffer_truncate(buffer, 0U);
}

int32_t hps_string_buffer_as_view(const hps_string_buffer *buffer,
                                  hps_string_view *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = buffer_check(buffer);
        if (status == HPS_STRING_OK) {
            result->data = (buffer->length == 0U) ? NULL : buffer->data;
            result->length = buffer->length;
        }
    }
    return status;
}

int32_t hps_string_buffer_cstr(const hps_string_buffer *buffer,
                               const char **result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = buffer_check(buffer);
        if (status == HPS_STRING_OK) {
            *result = buffer->data;
        }
    }
    return status;
}

int32_t hps_string_buffer_remaining(const hps_string_buffer *buffer,
                                    size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = buffer_check(buffer);
        if (status == HPS_STRING_OK) {
            *result = buffer->capacity - buffer->length - 1U;
        }
    }
    return status;
}
