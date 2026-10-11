#include "hps/bytes/reader.h"
#include "hps/bytes/endian.h"

/**
 * @file src/bytes/reader.c
 * @brief Checked cursor operations over borrowed bytes; no payload scratch array.
 */

static hps_byte_status reader_check(const hps_byte_reader *reader)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (reader != NULL) {
        status = HPS_BYTE_INVALID_STATE;
        if ((reader->position <= reader->view.length) &&
            ((reader->view.data != NULL) || (reader->view.length == 0U))) {
            status = HPS_BYTE_OK;
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_init(hps_byte_reader *reader,
                                    const hps_byte_view view)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (reader != NULL) {
        hps_byte_view canonical = {NULL, 0U};
        status = hps_byte_view_init(&canonical, view.data, view.length);
        if (status == HPS_BYTE_OK) {
            reader->view = canonical;
            reader->position = 0U;
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_remaining(const hps_byte_reader *reader,
                                         size_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            *result = reader->view.length - reader->position;
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_skip(hps_byte_reader *reader, const size_t length)
{
    hps_byte_status status = reader_check(reader);
    if (status == HPS_BYTE_OK) {
        if (length > (reader->view.length - reader->position)) {
            status = HPS_BYTE_OUT_OF_RANGE;
        } else {
            reader->position += length;
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_take_view(hps_byte_reader *reader,
                                        const size_t length,
                                        hps_byte_view *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            status = hps_byte_view_slice(reader->view, reader->position,
                                         length, result);
            if (status == HPS_BYTE_OK) {
                /* Successful slice proves length <= view.length-position. */
                reader->position += length;
            }
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_read_u8(hps_byte_reader *reader, uint8_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            status = hps_byte_view_read_u8(reader->view, reader->position, result);
            if (status == HPS_BYTE_OK) {
                /* Successful read_u8 proves position < view.length <= SIZE_MAX. */
                reader->position += 1U;
            }
        }
    }
    return status;
}

/* Each successful endian read proves width <= view.length-position before the
 * cursor addition. Output/reader/payload separation is a caller precondition,
 * so writing result cannot change the metadata used to commit the cursor.
 */
hps_byte_status hps_byte_reader_read_u16_le(hps_byte_reader *reader,
                                          uint16_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            status = hps_byte_view_read_u16_le(reader->view, reader->position,
                                              result);
            if (status == HPS_BYTE_OK) {
                reader->position += 2U;
            }
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_read_u16_be(hps_byte_reader *reader,
                                          uint16_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            status = hps_byte_view_read_u16_be(reader->view, reader->position,
                                              result);
            if (status == HPS_BYTE_OK) {
                reader->position += 2U;
            }
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_read_u32_le(hps_byte_reader *reader,
                                          uint32_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            status = hps_byte_view_read_u32_le(reader->view, reader->position,
                                              result);
            if (status == HPS_BYTE_OK) {
                reader->position += 4U;
            }
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_read_u32_be(hps_byte_reader *reader,
                                          uint32_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            status = hps_byte_view_read_u32_be(reader->view, reader->position,
                                              result);
            if (status == HPS_BYTE_OK) {
                reader->position += 4U;
            }
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_read_u64_le(hps_byte_reader *reader,
                                          uint64_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            status = hps_byte_view_read_u64_le(reader->view, reader->position,
                                              result);
            if (status == HPS_BYTE_OK) {
                reader->position += 8U;
            }
        }
    }
    return status;
}

hps_byte_status hps_byte_reader_read_u64_be(hps_byte_reader *reader,
                                          uint64_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = reader_check(reader);
        if (status == HPS_BYTE_OK) {
            status = hps_byte_view_read_u64_be(reader->view, reader->position,
                                              result);
            if (status == HPS_BYTE_OK) {
                reader->position += 8U;
            }
        }
    }
    return status;
}
