#include "../test_suites.h"
#include "hps/bytes/buffer.h"
#include "hps/bytes/codec.h"
#include "hps/bytes/endian.h"
#include "hps/bytes/view.h"
#include <assert.h>

/**
 * @file tests/bytes/test_bytes.c
 * @brief Prepared byte-view, buffer, overlap-model, codec and endian tests.
 */

/* Check the public result representation and local codes independently of the
 * operations that produce them. Function pointers check declared result types. */
static void test_results(void)
{
    int32_t (*initialize)(hps_byte_view *result, const uint8_t *data,
                          size_t length) =
        hps_byte_view_init;
    int32_t (*clear)(hps_byte_buffer *buffer) = hps_byte_buffer_clear;
    hps_byte_view empty = {NULL, 0U};
    hps_byte_buffer buffer = {NULL, 0U, 0U};
    int32_t result = initialize(&empty, NULL, 0U);
    assert(sizeof(hps_byte_view_init(&empty, NULL, 0U)) == sizeof(int32_t));
    assert(sizeof(HPS_BYTE_INVALID_ARGUMENT) == sizeof(int32_t));
    assert(HPS_BYTE_OK == INT32_C(0));
    assert(HPS_BYTE_INVALID_ARGUMENT == -INT32_C(1));
    assert(HPS_BYTE_OUT_OF_RANGE == -INT32_C(2));
    assert(HPS_BYTE_NO_SPACE == -INT32_C(3));
    assert(HPS_BYTE_INVALID_STATE == -INT32_C(4));
    assert(result == INT32_C(0));
    result = clear(&buffer);
    assert(result == INT32_C(0));
    result = initialize(NULL, NULL, 0U);
    assert(result == -INT32_C(1));
}

static void test_views(void)
{
    const uint8_t data[4] = {0U, 7U, 0U, 255U};
    hps_byte_view view = {NULL, 0U};
    hps_byte_view slice = {data, 4U};
    int32_t status = hps_byte_view_init(&view, data, 4U);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_view_slice(view, 1U, 2U, &slice);
    assert(status == HPS_BYTE_OK && slice.data == &data[1] && slice.length == 2U);
    uint8_t value = 91U;
    status = hps_byte_view_read_u8(slice, 1U, &value);
    assert(status == HPS_BYTE_OK && value == 0U);
    status = hps_byte_view_read_u8(slice, 2U, &value);
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
    status = hps_byte_view_read_u8(view, 0U, NULL);
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
    status = hps_byte_view_read_u8(malformed, 0U, &value);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && value == 0U);
    status = hps_byte_view_slice(view, 1U, 2U, &view);
    assert(status == HPS_BYTE_OK && view.data == &data[1] && view.length == 2U);
    status = hps_byte_view_init(&slice, data, 0U);
    assert(status == HPS_BYTE_OK && slice.data == NULL);
    status = hps_byte_view_slice(slice, 0U, 0U, &slice);
    assert(status == HPS_BYTE_OK && slice.data == NULL);
    status = hps_byte_view_read_u8(slice, 0U, &value);
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
    int32_t status = hps_byte_buffer_init(&buffer, data, 4U);
    assert(status == HPS_BYTE_OK && buffer.length == 0U && data[3] == 91U);
    const hps_byte_view initial = {data, 3U};
    status = hps_byte_buffer_assign_bytes(&buffer, initial);
    assert(status == HPS_BYTE_OK);
    const hps_byte_view too_many = {data, 2U};
    status = hps_byte_buffer_append_bytes(&buffer, too_many);
    assert(status == HPS_BYTE_NO_SPACE);
    assert_snapshot(&buffer, data);
    status = hps_byte_buffer_truncate(&buffer, SIZE_MAX);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert_snapshot(&buffer, data);
    status = hps_byte_buffer_init(&buffer, NULL, 1U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_snapshot(&buffer, data);
    const hps_byte_view malformed = {NULL, SIZE_MAX};
    status = hps_byte_buffer_assign_bytes(&buffer, malformed);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_snapshot(&buffer, data);
    status = hps_byte_buffer_append_bytes(&buffer, malformed);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_snapshot(&buffer, data);
    const uint8_t larger[5] = {7U, 7U, 7U, 7U, 7U};
    const hps_byte_view oversized = {larger, 5U};
    status = hps_byte_buffer_assign_bytes(&buffer, oversized);
    assert(status == HPS_BYTE_NO_SPACE);
    assert_snapshot(&buffer, data);
    hps_byte_view borrowed = {NULL, 0U};
    status = hps_byte_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_BYTE_OK && borrowed.data == data && borrowed.length == 3U);
    status = hps_byte_buffer_append_u8(&buffer, 4U);
    assert(status == HPS_BYTE_OK && buffer.length == 4U && data[3] == 4U);
    status = hps_byte_buffer_append_u8(&buffer, 9U);
    assert(status == HPS_BYTE_NO_SPACE && data[3] == 4U && buffer.length == 4U);
    const hps_byte_view empty = {NULL, 0U};
    status = hps_byte_buffer_append_bytes(&buffer, empty);
    assert(status == HPS_BYTE_OK && buffer.length == 4U);
    status = hps_byte_buffer_truncate(&buffer, 2U);
    assert(status == HPS_BYTE_OK && buffer.length == 2U && data[3] == 4U);
    status = hps_byte_buffer_clear(&buffer);
    assert(status == HPS_BYTE_OK && buffer.length == 0U && data[0] == 1U);
    status = hps_byte_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_BYTE_OK && borrowed.data == NULL && borrowed.length == 0U);
    status = hps_byte_buffer_assign_bytes(&buffer, empty);
    assert(status == HPS_BYTE_OK && data[3] == 4U);

    /* Test-only malformed metadata; no impossible extent is dereferenced. */
    buffer.length = SIZE_MAX;
    status = hps_byte_buffer_append_bytes(&buffer, empty);
    assert(status == HPS_BYTE_INVALID_STATE && buffer.length == SIZE_MAX);
    status = hps_byte_buffer_assign_bytes(&buffer, empty);
    assert(status == HPS_BYTE_INVALID_STATE);
    status = hps_byte_buffer_append_u8(&buffer, 1U);
    assert(status == HPS_BYTE_INVALID_STATE);
    status = hps_byte_buffer_clear(&buffer);
    assert(status == HPS_BYTE_INVALID_STATE);
    borrowed.data = larger;
    borrowed.length = 5U;
    status = hps_byte_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_BYTE_INVALID_STATE && borrowed.data == larger);
    status = hps_byte_buffer_init(&buffer, NULL, 0U);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_buffer_append_bytes(&buffer, empty);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_buffer_assign_bytes(&buffer, empty);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_buffer_append_u8(&buffer, 1U);
    assert(status == HPS_BYTE_NO_SPACE);
    status = hps_byte_buffer_as_view(&buffer, &borrowed);
    assert(status == HPS_BYTE_OK && borrowed.data == NULL);
    status = hps_byte_buffer_init(NULL, NULL, 0U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_buffer_assign_bytes(NULL, empty);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_buffer_append_bytes(NULL, empty);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_buffer_append_u8(NULL, 1U);
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
                    int32_t status = hps_byte_buffer_init(
                        &buffer, &data[destination], 16U - destination);
                    assert(status == HPS_BYTE_OK);
                    const hps_byte_view prefix = {&data[destination], initial};
                    status = hps_byte_buffer_assign_bytes(&buffer, prefix);
                    assert(status == HPS_BYTE_OK);
                    for (size_t index = 0U; index < count; index += 1U) {
                        expected[destination + initial + index] = (uint8_t)(source + index);
                    }
                    const hps_byte_view input = {&data[source], count};
                    status = hps_byte_buffer_append_bytes(&buffer, input);
                    assert(status == HPS_BYTE_OK && buffer.length == initial + count);
                    for (size_t index = 0U; index < 16U; index += 1U) {
                        assert(data[index] == expected[index]);
                        data[index] = (uint8_t)index;
                        expected[index] = (uint8_t)index;
                    }
                    for (size_t index = 0U; index < count; index += 1U) {
                        expected[destination + index] = (uint8_t)(source + index);
                    }
                    status = hps_byte_buffer_assign_bytes(&buffer, input);
                    assert(status == HPS_BYTE_OK && buffer.length == count);
                    for (size_t index = 0U; index < 16U; index += 1U) {
                        assert(data[index] == expected[index]);
                    }
                }
            }
        }
    }
}

/* Explicit wire fixtures are independent of host byte order and API round trips. */
static const uint8_t test_zero_wire[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
static const uint8_t test_ones_wire[8] = {
    0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU
};
static const uint8_t wire_u16_le[4] = {0xA5U, 0x23U, 0x81U, 0x5AU};
static const uint8_t wire_u16_be[4] = {0xA5U, 0x81U, 0x23U, 0x5AU};
static const uint8_t wire_u32_le[6] = {0xA5U, 0x67U, 0x45U, 0x23U, 0x81U, 0x5AU};
static const uint8_t wire_u32_be[6] = {0xA5U, 0x81U, 0x23U, 0x45U, 0x67U, 0x5AU};
static const uint8_t wire_u64_le[10] = {
    0xA5U, 0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x81U, 0x5AU
};
static const uint8_t wire_u64_be[10] = {
    0xA5U, 0x81U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU, 0x5AU
};

/* Callers provide ten-byte storage and width 0/2/4/8. Width zero checks that
 * an invalid-order call preserved every sentinel, without reading expected. */
static void assert_codec_output(const uint8_t *data, size_t width,
                                const uint8_t *expected)
{
    assert(data[0] == 0xD7U);
    for (size_t index = 0U; index < width; index += 1U) {
        assert(data[index + 1U] == expected[index]);
    }
    for (size_t index = width + 1U; index < 10U; index += 1U) {
        assert(data[index] == 0xD7U);
    }
}

static void fill_codec_sentinels(uint8_t data[10])
{
    for (size_t index = 0U; index < 10U; index += 1U) {
        data[index] = 0xD7U;
    }
}

static void test_codec(void)
{
    const hps_byte_order orders[2] = {
        HPS_BYTE_ORDER_LITTLE_ENDIAN, HPS_BYTE_ORDER_BIG_ENDIAN
    };
    const uint8_t *const u16_wire[2] = {&wire_u16_le[1], &wire_u16_be[1]};
    const uint8_t *const u32_wire[2] = {&wire_u32_le[1], &wire_u32_be[1]};
    const uint8_t *const u64_wire[2] = {&wire_u64_le[1], &wire_u64_be[1]};
    const hps_byte_order invalid_order = UINT8_C(255);
    uint8_t data[10];
    uint16_t u16 = UINT16_C(0);
    uint32_t u32 = UINT32_C(0);
    uint64_t u64 = UINT64_C(0);

    for (size_t fixture = 0U; fixture < 2U; fixture += 1U) {
        u16 = hps_codec_bytes_to_u16(u16_wire[fixture], orders[fixture]);
        u32 = hps_codec_bytes_to_u32(u32_wire[fixture], orders[fixture]);
        u64 = hps_codec_bytes_to_u64(u64_wire[fixture], orders[fixture]);
        assert(u16 == UINT16_C(0x8123));
        assert(u32 == UINT32_C(0x81234567));
        assert(u64 == UINT64_C(0x8123456789ABCDEF));

        u16 = hps_codec_bytes_to_u16(test_zero_wire, orders[fixture]);
        u32 = hps_codec_bytes_to_u32(test_zero_wire, orders[fixture]);
        u64 = hps_codec_bytes_to_u64(test_zero_wire, orders[fixture]);
        assert(u16 == UINT16_C(0) && u32 == UINT32_C(0) && u64 == UINT64_C(0));
        u16 = hps_codec_bytes_to_u16(test_ones_wire, orders[fixture]);
        u32 = hps_codec_bytes_to_u32(test_ones_wire, orders[fixture]);
        u64 = hps_codec_bytes_to_u64(test_ones_wire, orders[fixture]);
        assert(u16 == UINT16_MAX && u32 == UINT32_MAX && u64 == UINT64_MAX);

        u16 = hps_codec_bytes_to_u16(NULL, orders[fixture]);
        u32 = hps_codec_bytes_to_u32(NULL, orders[fixture]);
        u64 = hps_codec_bytes_to_u64(NULL, orders[fixture]);
        assert(u16 == UINT16_C(0) && u32 == UINT32_C(0) && u64 == UINT64_C(0));
        hps_codec_u16_to_bytes(NULL, UINT16_C(0x8123), orders[fixture]);
        hps_codec_u32_to_bytes(NULL, UINT32_C(0x81234567), orders[fixture]);
        hps_codec_u64_to_bytes(NULL, UINT64_C(0x8123456789ABCDEF), orders[fixture]);

        fill_codec_sentinels(data);
        hps_codec_u16_to_bytes(&data[1], UINT16_C(0x8123), orders[fixture]);
        assert_codec_output(data, 2U, u16_wire[fixture]);
        fill_codec_sentinels(data);
        hps_codec_u32_to_bytes(&data[1], UINT32_C(0x81234567), orders[fixture]);
        assert_codec_output(data, 4U, u32_wire[fixture]);
        fill_codec_sentinels(data);
        hps_codec_u64_to_bytes(&data[1], UINT64_C(0x8123456789ABCDEF), orders[fixture]);
        assert_codec_output(data, 8U, u64_wire[fixture]);
    }

    u16 = hps_codec_bytes_to_u16(&wire_u16_le[1], invalid_order);
    u32 = hps_codec_bytes_to_u32(&wire_u32_le[1], invalid_order);
    u64 = hps_codec_bytes_to_u64(&wire_u64_le[1], invalid_order);
    assert(u16 == UINT16_C(0) && u32 == UINT32_C(0) && u64 == UINT64_C(0));
    fill_codec_sentinels(data);
    hps_codec_u16_to_bytes(&data[1], UINT16_C(0x8123), invalid_order);
    assert_codec_output(data, 0U, NULL);
    hps_codec_u32_to_bytes(&data[1], UINT32_C(0x81234567), invalid_order);
    assert_codec_output(data, 0U, NULL);
    hps_codec_u64_to_bytes(&data[1], UINT64_C(0x8123456789ABCDEF), invalid_order);
    assert_codec_output(data, 0U, NULL);
}

static void assert_endian_snapshot(const uint8_t *data, const uint8_t *snapshot)
{
    for (size_t index = 0U; index < 11U; index += 1U) {
        assert(data[index] == snapshot[index]);
    }
}

/* Test callers use width 2/4/8, spare <=1 and eleven-byte storage. */
static void assert_endian_append(const hps_byte_buffer *buffer, const uint8_t *data,
                                 size_t width, size_t spare, const uint8_t *expected)
{
    assert(buffer->data == &data[1]);
    assert(buffer->length == width + 1U && buffer->capacity == width + 1U + spare);
    assert(data[0] == 0xD7U && data[1] == 0xA5U);
    for (size_t index = 0U; index < width; index += 1U) {
        assert(data[index + 2U] == expected[index]);
    }
    for (size_t index = width + 2U; index < 11U; index += 1U) {
        assert(data[index] == 0xD7U);
    }
}

static void test_endian_read_u16(
    int32_t (*read)(hps_byte_view view, size_t offset, uint16_t *result),
    const uint8_t *wire, uint16_t expected)
{
    const hps_byte_view view = {wire, 4U};
    uint16_t value = UINT16_C(0);
    int32_t status = read(view, 1U, &value);
    assert(status == HPS_BYTE_OK && value == expected);

    const hps_byte_view exact = {wire, 3U};
    status = read(exact, 1U, &value);
    assert(status == HPS_BYTE_OK && value == expected);
    const hps_byte_view zeros = {test_zero_wire, 2U};
    status = read(zeros, 0U, &value);
    assert(status == HPS_BYTE_OK && value == UINT16_C(0));
    const hps_byte_view ones = {test_ones_wire, 2U};
    status = read(ones, 0U, &value);
    assert(status == HPS_BYTE_OK && value == UINT16_MAX);

    const uint16_t sentinel = UINT16_C(0x5AA5);
    value = sentinel;
    for (size_t length = 0U; length < 2U; length += 1U) {
        const hps_byte_view truncated = {&wire[1], length};
        status = read(truncated, 0U, &value);
        assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
        const hps_byte_view prefixed_truncated = {wire, length + 1U};
        status = read(prefixed_truncated, 1U, &value);
        assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    }
    status = read(view, view.length, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    status = read(view, view.length + 1U, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    status = read(view, SIZE_MAX, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    const hps_byte_view empty = {NULL, 0U};
    status = read(empty, 0U, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    const hps_byte_view malformed = {NULL, SIZE_MAX};
    status = read(malformed, 0U, &value);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && value == sentinel);
    status = read(view, 0U, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = read(view, SIZE_MAX, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
}

static void test_endian_read_u32(
    int32_t (*read)(hps_byte_view view, size_t offset, uint32_t *result),
    const uint8_t *wire, uint32_t expected)
{
    const hps_byte_view view = {wire, 6U};
    uint32_t value = UINT32_C(0);
    int32_t status = read(view, 1U, &value);
    assert(status == HPS_BYTE_OK && value == expected);

    const hps_byte_view exact = {wire, 5U};
    status = read(exact, 1U, &value);
    assert(status == HPS_BYTE_OK && value == expected);
    const hps_byte_view zeros = {test_zero_wire, 4U};
    status = read(zeros, 0U, &value);
    assert(status == HPS_BYTE_OK && value == UINT32_C(0));
    const hps_byte_view ones = {test_ones_wire, 4U};
    status = read(ones, 0U, &value);
    assert(status == HPS_BYTE_OK && value == UINT32_MAX);

    const uint32_t sentinel = UINT32_C(0x5AA5);
    value = sentinel;
    for (size_t length = 0U; length < 4U; length += 1U) {
        const hps_byte_view truncated = {&wire[1], length};
        status = read(truncated, 0U, &value);
        assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
        const hps_byte_view prefixed_truncated = {wire, length + 1U};
        status = read(prefixed_truncated, 1U, &value);
        assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    }
    status = read(view, view.length, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    status = read(view, view.length + 1U, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    status = read(view, SIZE_MAX, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    const hps_byte_view empty = {NULL, 0U};
    status = read(empty, 0U, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    const hps_byte_view malformed = {NULL, SIZE_MAX};
    status = read(malformed, 0U, &value);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && value == sentinel);
    status = read(view, 0U, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = read(view, SIZE_MAX, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
}

static void test_endian_read_u64(
    int32_t (*read)(hps_byte_view view, size_t offset, uint64_t *result),
    const uint8_t *wire, uint64_t expected)
{
    const hps_byte_view view = {wire, 10U};
    uint64_t value = UINT64_C(0);
    int32_t status = read(view, 1U, &value);
    assert(status == HPS_BYTE_OK && value == expected);

    const hps_byte_view exact = {wire, 9U};
    status = read(exact, 1U, &value);
    assert(status == HPS_BYTE_OK && value == expected);
    const hps_byte_view zeros = {test_zero_wire, 8U};
    status = read(zeros, 0U, &value);
    assert(status == HPS_BYTE_OK && value == UINT64_C(0));
    const hps_byte_view ones = {test_ones_wire, 8U};
    status = read(ones, 0U, &value);
    assert(status == HPS_BYTE_OK && value == UINT64_MAX);

    const uint64_t sentinel = UINT64_C(0x5AA5);
    value = sentinel;
    for (size_t length = 0U; length < 8U; length += 1U) {
        const hps_byte_view truncated = {&wire[1], length};
        status = read(truncated, 0U, &value);
        assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
        const hps_byte_view prefixed_truncated = {wire, length + 1U};
        status = read(prefixed_truncated, 1U, &value);
        assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    }
    status = read(view, view.length, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    status = read(view, view.length + 1U, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    status = read(view, SIZE_MAX, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    const hps_byte_view empty = {NULL, 0U};
    status = read(empty, 0U, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    const hps_byte_view malformed = {NULL, SIZE_MAX};
    status = read(malformed, 0U, &value);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && value == sentinel);
    status = read(view, 0U, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = read(view, SIZE_MAX, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
}

static void test_endian_append_u16(
    int32_t (*append)(hps_byte_buffer *buffer, uint16_t value),
    const uint8_t *wire, uint16_t expected)
{
    const uint16_t values[3] = {expected, UINT16_C(0), UINT16_MAX};
    const uint8_t *expected_wire[3] = {&wire[1], test_zero_wire, test_ones_wire};
    uint8_t data[11];
    uint8_t snapshot[11];
    hps_byte_buffer buffer = {NULL, 0U, 0U};
    int32_t status = HPS_BYTE_OK;
    for (size_t fixture = 0U; fixture < 3U; fixture += 1U) {
        for (size_t spare = 0U; spare <= 1U; spare += 1U) {
            for (size_t index = 0U; index < 11U; index += 1U) {
                data[index] = 0xD7U;
            }
            status = hps_byte_buffer_init(&buffer, &data[1], 3U + spare);
            assert(status == HPS_BYTE_OK);
            status = hps_byte_buffer_append_u8(&buffer, 0xA5U);
            assert(status == HPS_BYTE_OK);
            status = append(&buffer, values[fixture]);
            assert(status == HPS_BYTE_OK);
            assert_endian_append(&buffer, data, 2U, spare, expected_wire[fixture]);
        }

        for (size_t index = 0U; index < 11U; index += 1U) {
            data[index] = 0xD7U;
        }
        status = hps_byte_buffer_init(&buffer, &data[1], 2U);
        assert(status == HPS_BYTE_OK);
        status = hps_byte_buffer_append_u8(&buffer, 0xA5U);
        assert(status == HPS_BYTE_OK);
        for (size_t index = 0U; index < 11U; index += 1U) {
            snapshot[index] = data[index];
        }
        status = append(&buffer, values[fixture]);
        assert(status == HPS_BYTE_NO_SPACE);
        assert(buffer.data == &data[1] && buffer.length == 1U);
        assert(buffer.capacity == 2U);
        assert_endian_snapshot(data, snapshot);
    }

    status = append(NULL, expected);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    buffer.length = SIZE_MAX;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == &data[1] && buffer.length == SIZE_MAX);
    assert(buffer.capacity == 2U);
    assert_endian_snapshot(data, snapshot);

    buffer.data = NULL;
    buffer.length = 0U;
    buffer.capacity = SIZE_MAX;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == NULL && buffer.length == 0U && buffer.capacity == SIZE_MAX);
    assert_endian_snapshot(data, snapshot);

    buffer.capacity = 0U;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_NO_SPACE);
    assert(buffer.data == NULL && buffer.length == 0U && buffer.capacity == 0U);
    assert_endian_snapshot(data, snapshot);
    buffer.length = 1U;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == NULL && buffer.length == 1U && buffer.capacity == 0U);
    assert_endian_snapshot(data, snapshot);
}

static void test_endian_append_u32(
    int32_t (*append)(hps_byte_buffer *buffer, uint32_t value),
    const uint8_t *wire, uint32_t expected)
{
    const uint32_t values[3] = {expected, UINT32_C(0), UINT32_MAX};
    const uint8_t *expected_wire[3] = {&wire[1], test_zero_wire, test_ones_wire};
    uint8_t data[11];
    uint8_t snapshot[11];
    hps_byte_buffer buffer = {NULL, 0U, 0U};
    int32_t status = HPS_BYTE_OK;
    for (size_t fixture = 0U; fixture < 3U; fixture += 1U) {
        for (size_t spare = 0U; spare <= 1U; spare += 1U) {
            for (size_t index = 0U; index < 11U; index += 1U) {
                data[index] = 0xD7U;
            }
            status = hps_byte_buffer_init(&buffer, &data[1], 5U + spare);
            assert(status == HPS_BYTE_OK);
            status = hps_byte_buffer_append_u8(&buffer, 0xA5U);
            assert(status == HPS_BYTE_OK);
            status = append(&buffer, values[fixture]);
            assert(status == HPS_BYTE_OK);
            assert_endian_append(&buffer, data, 4U, spare, expected_wire[fixture]);
        }

        for (size_t index = 0U; index < 11U; index += 1U) {
            data[index] = 0xD7U;
        }
        status = hps_byte_buffer_init(&buffer, &data[1], 4U);
        assert(status == HPS_BYTE_OK);
        status = hps_byte_buffer_append_u8(&buffer, 0xA5U);
        assert(status == HPS_BYTE_OK);
        for (size_t index = 0U; index < 11U; index += 1U) {
            snapshot[index] = data[index];
        }
        status = append(&buffer, values[fixture]);
        assert(status == HPS_BYTE_NO_SPACE);
        assert(buffer.data == &data[1] && buffer.length == 1U);
        assert(buffer.capacity == 4U);
        assert_endian_snapshot(data, snapshot);
    }

    status = append(NULL, expected);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    buffer.length = SIZE_MAX;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == &data[1] && buffer.length == SIZE_MAX);
    assert(buffer.capacity == 4U);
    assert_endian_snapshot(data, snapshot);

    buffer.data = NULL;
    buffer.length = 0U;
    buffer.capacity = SIZE_MAX;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == NULL && buffer.length == 0U && buffer.capacity == SIZE_MAX);
    assert_endian_snapshot(data, snapshot);

    buffer.capacity = 0U;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_NO_SPACE);
    assert(buffer.data == NULL && buffer.length == 0U && buffer.capacity == 0U);
    assert_endian_snapshot(data, snapshot);
    buffer.length = 1U;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == NULL && buffer.length == 1U && buffer.capacity == 0U);
    assert_endian_snapshot(data, snapshot);
}

static void test_endian_append_u64(
    int32_t (*append)(hps_byte_buffer *buffer, uint64_t value),
    const uint8_t *wire, uint64_t expected)
{
    const uint64_t values[3] = {expected, UINT64_C(0), UINT64_MAX};
    const uint8_t *expected_wire[3] = {&wire[1], test_zero_wire, test_ones_wire};
    uint8_t data[11];
    uint8_t snapshot[11];
    hps_byte_buffer buffer = {NULL, 0U, 0U};
    int32_t status = HPS_BYTE_OK;
    for (size_t fixture = 0U; fixture < 3U; fixture += 1U) {
        for (size_t spare = 0U; spare <= 1U; spare += 1U) {
            for (size_t index = 0U; index < 11U; index += 1U) {
                data[index] = 0xD7U;
            }
            status = hps_byte_buffer_init(&buffer, &data[1], 9U + spare);
            assert(status == HPS_BYTE_OK);
            status = hps_byte_buffer_append_u8(&buffer, 0xA5U);
            assert(status == HPS_BYTE_OK);
            status = append(&buffer, values[fixture]);
            assert(status == HPS_BYTE_OK);
            assert_endian_append(&buffer, data, 8U, spare, expected_wire[fixture]);
        }

        for (size_t index = 0U; index < 11U; index += 1U) {
            data[index] = 0xD7U;
        }
        status = hps_byte_buffer_init(&buffer, &data[1], 8U);
        assert(status == HPS_BYTE_OK);
        status = hps_byte_buffer_append_u8(&buffer, 0xA5U);
        assert(status == HPS_BYTE_OK);
        for (size_t index = 0U; index < 11U; index += 1U) {
            snapshot[index] = data[index];
        }
        status = append(&buffer, values[fixture]);
        assert(status == HPS_BYTE_NO_SPACE);
        assert(buffer.data == &data[1] && buffer.length == 1U);
        assert(buffer.capacity == 8U);
        assert_endian_snapshot(data, snapshot);
    }

    status = append(NULL, expected);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    buffer.length = SIZE_MAX;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == &data[1] && buffer.length == SIZE_MAX);
    assert(buffer.capacity == 8U);
    assert_endian_snapshot(data, snapshot);

    buffer.data = NULL;
    buffer.length = 0U;
    buffer.capacity = SIZE_MAX;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == NULL && buffer.length == 0U && buffer.capacity == SIZE_MAX);
    assert_endian_snapshot(data, snapshot);

    buffer.capacity = 0U;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_NO_SPACE);
    assert(buffer.data == NULL && buffer.length == 0U && buffer.capacity == 0U);
    assert_endian_snapshot(data, snapshot);
    buffer.length = 1U;
    status = append(&buffer, expected);
    assert(status == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == NULL && buffer.length == 1U && buffer.capacity == 0U);
    assert_endian_snapshot(data, snapshot);
}

static void test_endian(void)
{
    test_endian_read_u16(hps_byte_view_read_u16_le, wire_u16_le, UINT16_C(0x8123));
    test_endian_read_u16(hps_byte_view_read_u16_be, wire_u16_be, UINT16_C(0x8123));
    test_endian_read_u32(hps_byte_view_read_u32_le, wire_u32_le, UINT32_C(0x81234567));
    test_endian_read_u32(hps_byte_view_read_u32_be, wire_u32_be, UINT32_C(0x81234567));
    test_endian_read_u64(hps_byte_view_read_u64_le, wire_u64_le,
                         UINT64_C(0x8123456789ABCDEF));
    test_endian_read_u64(hps_byte_view_read_u64_be, wire_u64_be,
                         UINT64_C(0x8123456789ABCDEF));
    test_endian_append_u16(hps_byte_buffer_append_u16_le, wire_u16_le, UINT16_C(0x8123));
    test_endian_append_u16(hps_byte_buffer_append_u16_be, wire_u16_be, UINT16_C(0x8123));
    test_endian_append_u32(hps_byte_buffer_append_u32_le, wire_u32_le, UINT32_C(0x81234567));
    test_endian_append_u32(hps_byte_buffer_append_u32_be, wire_u32_be, UINT32_C(0x81234567));
    test_endian_append_u64(hps_byte_buffer_append_u64_le, wire_u64_le,
                           UINT64_C(0x8123456789ABCDEF));
    test_endian_append_u64(hps_byte_buffer_append_u64_be, wire_u64_be,
                           UINT64_C(0x8123456789ABCDEF));
}

int test_bytes(void)
{
    test_results();
    test_views();
    test_buffers();
    test_overlap_model();
    test_codec();
    test_endian();
    return 0;
}
