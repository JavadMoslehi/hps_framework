#include "hps/bytes/view.h"

/**
 * @file src/bytes/view.c
 * @brief Checked construction, slicing, byte reads and equality of borrowed bytes.
 */

static hps_byte_status view_check(const hps_byte_view view)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if ((view.data != NULL) || (view.length == 0U)) {
        status = HPS_BYTE_OK;
    }
    return status;
}

hps_byte_status hps_byte_view_init(hps_byte_view *result,
                                   const uint8_t *data, const size_t length)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if ((result != NULL) && ((data != NULL) || (length == 0U))) {
        result->data = (length == 0U) ? NULL : data;
        result->length = length;
        status = HPS_BYTE_OK;
    }
    return status;
}

hps_byte_status hps_byte_view_slice(const hps_byte_view view, const size_t offset,
                                    const size_t length, hps_byte_view *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = view_check(view);
        if (status == HPS_BYTE_OK) {
            status = HPS_BYTE_OUT_OF_RANGE;
            if (offset <= view.length) {
                if (length <= (view.length - offset)) {
                    result->data = (length == 0U) ? NULL : &view.data[offset];
                    result->length = length;
                    status = HPS_BYTE_OK;
                }
            }
        }
    }
    return status;
}

hps_byte_status hps_byte_view_read_u8(const hps_byte_view view, const size_t index,
                                    uint8_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = view_check(view);
        if (status == HPS_BYTE_OK) {
            if (index >= view.length) {
                status = HPS_BYTE_OUT_OF_RANGE;
            } else {
                *result = view.data[index];
            }
        }
    }
    return status;
}

hps_byte_status hps_byte_view_equal(const hps_byte_view left,
                                    const hps_byte_view right, uint8_t *result)
{
    hps_byte_status status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = view_check(left);
        if (status == HPS_BYTE_OK) {
            status = view_check(right);
            if (status == HPS_BYTE_OK) {
                uint8_t equal = 0U;
                if (left.length == right.length) {
                    size_t index = 0U;
                    equal = 1U;
                    while ((index < left.length) && (equal == 1U)) {
                        if (left.data[index] != right.data[index]) {
                            equal = 0U;
                        }
                        index += 1U;
                    }
                }
                *result = equal;
            }
        }
    }
    return status;
}
