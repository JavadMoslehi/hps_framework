#include "hps/bytes/buffer.h"
#include "hps/bytes/view.h"
#include <assert.h>

/**
 * @file tests/bytes/test_bytes.c
 * @brief Prepared boundary, failure-preservation and overlap-model tests.
 */

static void test_views(void)
{
    const uint8_t data[4] = {0U, 7U, 0U, 255U};
    hps_byte_view view = {NULL, 0U};
    hps_byte_view slice = {data, 4U};
    hps_byte_status status = hps_byte_view_init(&view, data, 4U);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_view_slice(view, 1U, 2U, &slice);
    assert(status == HPS_BYTE_OK && slice.data == &data[1] && slice.length == 2U);
    uint8_t value = 91U;
    status = hps_byte_view_at(slice, 1U, &value);
    assert(status == HPS_BYTE_OK && value == 0U);
    status = hps_byte_view_at(slice, 2U, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == 0U);
    status = hps_byte_view_slice(view, SIZE_MAX, 1U, &slice);
    assert(status == HPS_BYTE_OUT_OF_RANGE && slice.data == &data[1]);
    status = hps_byte_view_slice(view, 1U, SIZE_MAX, &slice);
    assert(status == HPS_BYTE_OUT_OF_RANGE && slice.length == 2U);
    status = hps_byte_view_slice(view, 4U, 0U, &slice);
    assert(status == HPS_BYTE_OK && slice.data == NULL && slice.length == 0U);
    status = hps_byte_view_slice(view, 5U, 0U, &slice);
    assert(status == HPS_BYTE_OUT_OF_RANGE && slice.data == NULL);
    status = hps_byte_view_init(&slice, NULL, 1U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && slice.data == NULL);
    status = hps_byte_view_init(NULL, data, 4U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_view_slice(view, 0U, 1U, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_view_at(view, 0U, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    uint8_t equal = 99U;
    status = hps_byte_view_equal(view, view, &equal);
    assert(status == HPS_BYTE_OK && equal == 1U);
    status = hps_byte_view_equal(view, slice, &equal);
    assert(status == HPS_BYTE_OK && equal == 0U);
    const uint8_t changed[4] = {0U, 7U, 0U, 254U};
    const hps_byte_view different = {changed, 4U};
    status = hps_byte_view_equal(view, different, &equal);
    assert(status == HPS_BYTE_OK && equal == 0U);
    status = hps_byte_view_equal(slice, slice, &equal);
    assert(status == HPS_BYTE_OK && equal == 1U);
    const hps_byte_view malformed = {NULL, SIZE_MAX};
    status = hps_byte_view_equal(view, malformed, &equal);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && equal == 1U);
    status = hps_byte_view_equal(malformed, view, &equal);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && equal == 1U);
    status = hps_byte_view_equal(view, view, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_view_slice(malformed, 0U, 0U, &slice);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && slice.data == NULL);
    status = hps_byte_view_at(malformed, 0U, &value);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && value == 0U);
    status = hps_byte_view_slice(view, 1U, 2U, &view);
    assert(status == HPS_BYTE_OK && view.data == &data[1] && view.length == 2U);
    status = hps_byte_view_init(&slice, data, 0U);
    assert(status == HPS_BYTE_OK && slice.data == NULL);
    status = hps_byte_view_slice(slice, 0U, 0U, &slice);
    assert(status == HPS_BYTE_OK && slice.data == NULL);
    status = hps_byte_view_at(slice, 0U, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
}

static void assert_snapshot(const hps_byte_buffer *buffer, const uint8_t *data)
{
    assert(buffer->length == 3U && buffer->capacity == 4U);
    assert(buffer->data == data);
    assert(data[0] == 1U && data[1] == 2U && data[2] == 3U && data[3] == 91U);
}

static void test_buffers(void)
{
    uint8_t data[4] = {1U, 2U, 3U, 91U};
    hps_byte_buffer buffer = {NULL, 0U, 0U};
    hps_byte_status status = hps_byte_buffer_init(&buffer, data, 4U);
    assert(status == HPS_BYTE_OK && buffer.length == 0U && data[3] == 91U);
    const hps_byte_view initial = {data, 3U};
    status = hps_byte_buffer_assign(&buffer, initial);
    assert(status == HPS_BYTE_OK);
    const hps_byte_view too_many = {data, 2U};
    status = hps_byte_buffer_append(&buffer, too_many);
    assert(status == HPS_BYTE_NO_SPACE);
    assert_snapshot(&buffer, data);
    status = hps_byte_buffer_truncate(&buffer, SIZE_MAX);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert_snapshot(&buffer, data);
    status = hps_byte_buffer_init(&buffer, NULL, 1U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_snapshot(&buffer, data);
    const hps_byte_view malformed = {NULL, SIZE_MAX};
    status = hps_byte_buffer_assign(&buffer, malformed);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_snapshot(&buffer, data);
    status = hps_byte_buffer_append(&buffer, malformed);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_snapshot(&buffer, data);
    const uint8_t larger[5] = {7U, 7U, 7U, 7U, 7U};
    const hps_byte_view oversized = {larger, 5U};
    status = hps_byte_buffer_assign(&buffer, oversized);
    assert(status == HPS_BYTE_NO_SPACE);
    assert_snapshot(&buffer, data);
    hps_byte_view borrowed = {NULL, 0U};
    status = hps_byte_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_BYTE_OK && borrowed.data == data && borrowed.length == 3U);
    status = hps_byte_buffer_append_byte(&buffer, 4U);
    assert(status == HPS_BYTE_OK && buffer.length == 4U && data[3] == 4U);
    status = hps_byte_buffer_append_byte(&buffer, 9U);
    assert(status == HPS_BYTE_NO_SPACE && data[3] == 4U && buffer.length == 4U);
    const hps_byte_view empty = {NULL, 0U};
    status = hps_byte_buffer_append(&buffer, empty);
    assert(status == HPS_BYTE_OK && buffer.length == 4U);
    status = hps_byte_buffer_truncate(&buffer, 2U);
    assert(status == HPS_BYTE_OK && buffer.length == 2U && data[3] == 4U);
    status = hps_byte_buffer_clear(&buffer);
    assert(status == HPS_BYTE_OK && buffer.length == 0U && data[0] == 1U);
    status = hps_byte_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_BYTE_OK && borrowed.data == NULL && borrowed.length == 0U);
    status = hps_byte_buffer_assign(&buffer, empty);
    assert(status == HPS_BYTE_OK && data[3] == 4U);

    /* Test-only malformed metadata; no impossible extent is dereferenced. */
    buffer.length = SIZE_MAX;
    status = hps_byte_buffer_append(&buffer, empty);
    assert(status == HPS_BYTE_INVALID_STATE && buffer.length == SIZE_MAX);
    status = hps_byte_buffer_assign(&buffer, empty);
    assert(status == HPS_BYTE_INVALID_STATE);
    status = hps_byte_buffer_append_byte(&buffer, 1U);
    assert(status == HPS_BYTE_INVALID_STATE);
    status = hps_byte_buffer_clear(&buffer);
    assert(status == HPS_BYTE_INVALID_STATE);
    borrowed.data = larger;
    borrowed.length = 5U;
    status = hps_byte_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_BYTE_INVALID_STATE && borrowed.data == larger);
    status = hps_byte_buffer_init(&buffer, NULL, 0U);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_buffer_append(&buffer, empty);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_buffer_assign(&buffer, empty);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_buffer_append_byte(&buffer, 1U);
    assert(status == HPS_BYTE_NO_SPACE);
    status = hps_byte_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_BYTE_OK && borrowed.data == NULL);
    status = hps_byte_buffer_init(NULL, NULL, 0U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_buffer_assign(NULL, empty);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_buffer_append(NULL, empty);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_buffer_append_byte(NULL, 1U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_buffer_clear(NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_buffer_as_view(NULL, &borrowed);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && borrowed.data == NULL);
    status = hps_byte_buffer_as_view(&buffer, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    buffer.capacity = 1U;
    status = hps_byte_buffer_clear(&buffer);
    assert(status == HPS_BYTE_INVALID_STATE);
}

/* Independent snapshot oracle covers forward/backward overlap, equal addresses,
 * disjoint ranges and empty copies. Numerical bounds: all indexes <=15. */
static void test_overlap_model(void)
{
    uint8_t data[16];
    uint8_t expected[16];
    for (size_t destination = 0U; destination <= 4U; destination += 1U) {
        for (size_t initial = 0U; initial <= 4U; initial += 1U) {
            for (size_t source = 0U; source <= 8U; source += 1U) {
                for (size_t count = 0U; count <= 8U; count += 1U) {
                    for (size_t index = 0U; index < 16U; index += 1U) {
                        data[index] = (uint8_t)index;
                        expected[index] = (uint8_t)index;
                    }
                    hps_byte_buffer buffer = {NULL, 0U, 0U};
                    hps_byte_status status = hps_byte_buffer_init(
                        &buffer, &data[destination], 16U - destination);
                    assert(status == HPS_BYTE_OK);
                    const hps_byte_view prefix = {&data[destination], initial};
                    status = hps_byte_buffer_assign(&buffer, prefix);
                    assert(status == HPS_BYTE_OK);
                    for (size_t index = 0U; index < count; index += 1U) {
                        expected[destination + initial + index] = (uint8_t)(source + index);
                    }
                    const hps_byte_view input = {&data[source], count};
                    status = hps_byte_buffer_append(&buffer, input);
                    assert(status == HPS_BYTE_OK && buffer.length == initial + count);
                    for (size_t index = 0U; index < 16U; index += 1U) {
                        assert(data[index] == expected[index]);
                        data[index] = (uint8_t)index;
                        expected[index] = (uint8_t)index;
                    }
                    for (size_t index = 0U; index < count; index += 1U) {
                        expected[destination + index] = (uint8_t)(source + index);
                    }
                    status = hps_byte_buffer_assign(&buffer, input);
                    assert(status == HPS_BYTE_OK && buffer.length == count);
                    for (size_t index = 0U; index < 16U; index += 1U) {
                        assert(data[index] == expected[index]);
                    }
                }
            }
        }
    }
}

int main(void)
{
    test_views();
    test_buffers();
    test_overlap_model();
    return 0;
}
