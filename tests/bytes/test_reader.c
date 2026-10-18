#include "../test_suites.h"
#include "hps/bytes/reader.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @file tests/bytes/test_reader.c
 * @brief Prepared sequential-reader contract and adversarial boundary tests.
 */

/* Direct metadata writes below are test setup/fault injection; applications use
 * init and reader APIs. Synthetic-extent probes exercise arithmetic outside the
 * valid-array contract, without forming or dereferencing payload pointers. */

static void assert_reader_snapshot(const hps_byte_reader *reader,
                                   hps_byte_reader snapshot)
{
    assert(reader->view.data == snapshot.view.data);
    assert(reader->view.length == snapshot.view.length);
    assert(reader->position == snapshot.position);
}

static void assert_reader_payload(const uint8_t *data, const uint8_t *expected,
                                  size_t length)
{
    for (size_t index = 0U; index < length; index += 1U) {
        assert(data[index] == expected[index]);
    }
}

static void test_reader_sequence(void)
{
    const uint8_t expected[33] = {
        0xA5U, 0xAAU, 0xBBU, 0xDDU,
        0x34U, 0x12U, 0x56U, 0x78U,
        0xEFU, 0xCDU, 0xABU, 0x89U,
        0x10U, 0x32U, 0x54U, 0x76U,
        0x08U, 0x07U, 0x06U, 0x05U, 0x04U, 0x03U, 0x02U, 0x01U,
        0x89U, 0xABU, 0xCDU, 0xEFU, 0x01U, 0x23U, 0x45U, 0x67U,
        0xFFU
    };
    uint8_t data[33];
    for (size_t index = 0U; index < 33U; index += 1U) {
        data[index] = expected[index];
    }
    const hps_byte_view view = {data, 33U};
    hps_byte_reader reader = {{NULL, 0U}, 0U};
    int32_t status = hps_byte_reader_init(&reader, view);
    assert(status == HPS_BYTE_OK);
    assert(reader.view.data == data && reader.view.length == 33U);
    assert(reader.position == 0U);
    size_t remaining = SIZE_MAX;
    status = hps_byte_reader_remaining(&reader, &remaining);
    assert(status == HPS_BYTE_OK && remaining == 33U);

    uint8_t byte = 0U;
    status = hps_byte_reader_read_u8(&reader, &byte);
    assert(status == HPS_BYTE_OK && byte == 0xA5U && reader.position == 1U);
    hps_byte_view taken = {NULL, 0U};
    status = hps_byte_reader_take_view(&reader, 2U, &taken);
    assert(status == HPS_BYTE_OK && reader.position == 3U);
    assert(taken.data == &data[1] && taken.length == 2U);
    assert(taken.data[0] == 0xAAU && taken.data[1] == 0xBBU);
    status = hps_byte_reader_skip(&reader, 1U);
    assert(status == HPS_BYTE_OK && reader.position == 4U);

    uint16_t value16 = UINT16_C(0);
    status = hps_byte_reader_read_u16_le(&reader, &value16);
    assert(status == HPS_BYTE_OK && value16 == UINT16_C(0x1234));
    assert(reader.position == 6U);
    status = hps_byte_reader_read_u16_be(&reader, &value16);
    assert(status == HPS_BYTE_OK && value16 == UINT16_C(0x5678));
    assert(reader.position == 8U);
    uint32_t value32 = UINT32_C(0);
    status = hps_byte_reader_read_u32_le(&reader, &value32);
    assert(status == HPS_BYTE_OK && value32 == UINT32_C(0x89ABCDEF));
    assert(reader.position == 12U);
    status = hps_byte_reader_read_u32_be(&reader, &value32);
    assert(status == HPS_BYTE_OK && value32 == UINT32_C(0x10325476));
    assert(reader.position == 16U);
    uint64_t value64 = UINT64_C(0);
    status = hps_byte_reader_read_u64_le(&reader, &value64);
    assert(status == HPS_BYTE_OK && value64 == UINT64_C(0x0102030405060708));
    assert(reader.position == 24U);
    status = hps_byte_reader_read_u64_be(&reader, &value64);
    assert(status == HPS_BYTE_OK && value64 == UINT64_C(0x89ABCDEF01234567));
    assert(reader.position == 32U);
    status = hps_byte_reader_read_u8(&reader, &byte);
    assert(status == HPS_BYTE_OK && byte == 0xFFU && reader.position == 33U);
    assert(reader.view.data == data && reader.view.length == 33U);
    status = hps_byte_reader_remaining(&reader, &remaining);
    assert(status == HPS_BYTE_OK && remaining == 0U);

    const hps_byte_reader eof = reader;
    const hps_byte_view taken_snapshot = taken;
    status = hps_byte_reader_take_view(&reader, 1U, &taken);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert(taken.data == taken_snapshot.data && taken.length == taken_snapshot.length);
    assert_reader_snapshot(&reader, eof);
    status = hps_byte_reader_skip(&reader, 1U);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert_reader_snapshot(&reader, eof);
    status = hps_byte_reader_read_u8(&reader, &byte);
    assert(status == HPS_BYTE_OUT_OF_RANGE && byte == 0xFFU);
    assert_reader_snapshot(&reader, eof);
    status = hps_byte_reader_take_view(&reader, 0U, &taken);
    assert(status == HPS_BYTE_OK && taken.data == NULL && taken.length == 0U);
    assert_reader_snapshot(&reader, eof);
    status = hps_byte_reader_skip(&reader, 0U);
    assert(status == HPS_BYTE_OK);
    assert_reader_snapshot(&reader, eof);
    assert_reader_payload(data, expected, 33U);
}

static void test_reader_independent_cursors(void)
{
    const uint8_t data[4] = {0x81U, 0x22U, 0x33U, 0x44U};
    const hps_byte_view view = {data, 4U};
    hps_byte_reader first = {{NULL, 0U}, 0U};
    hps_byte_reader second = {{NULL, 0U}, 0U};
    int32_t status = hps_byte_reader_init(&first, view);
    assert(status == HPS_BYTE_OK);
    status = hps_byte_reader_init(&second, view);
    assert(status == HPS_BYTE_OK);
    const hps_byte_reader second_start = second;
    uint8_t byte = 0U;
    status = hps_byte_reader_read_u8(&first, &byte);
    assert(status == HPS_BYTE_OK && byte == 0x81U && first.position == 1U);
    assert_reader_snapshot(&second, second_start);
    const hps_byte_reader first_after_byte = first;
    uint32_t word = UINT32_C(0);
    status = hps_byte_reader_read_u32_be(&second, &word);
    assert(status == HPS_BYTE_OK && word == UINT32_C(0x81223344));
    assert(second.position == 4U);
    assert_reader_snapshot(&first, first_after_byte);
    hps_byte_view taken = {NULL, 0U};
    status = hps_byte_reader_take_view(&first, 3U, &taken);
    assert(status == HPS_BYTE_OK && first.position == 4U);
    assert(taken.data == &data[1] && taken.length == 3U);
    assert(first.view.data == data && second.view.data == data);
    assert(first.view.length == 4U && second.view.length == 4U);
}

static void test_reader_initialization_and_empty(void)
{
    const uint8_t data[3] = {0x10U, 0x20U, 0x30U};
    const hps_byte_view view = {data, 3U};
    hps_byte_reader reader = {view, 2U};
    const hps_byte_reader original = reader;
    const hps_byte_view malformed = {NULL, SIZE_MAX};
    int32_t status = hps_byte_reader_init(&reader, malformed);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_reader_snapshot(&reader, original);
    status = hps_byte_reader_init(NULL, view);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_reader_init(NULL, malformed);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);

    /* Init writes fresh state without requiring valid old reader metadata. */
    reader.view.data = NULL;
    reader.view.length = SIZE_MAX;
    reader.position = SIZE_MAX;
    const hps_byte_reader corrupted = reader;
    status = hps_byte_reader_init(&reader, malformed);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_reader_snapshot(&reader, corrupted);
    status = hps_byte_reader_init(&reader, view);
    assert(status == HPS_BYTE_OK);
    assert(reader.view.data == data && reader.view.length == 3U);
    assert(reader.position == 0U);
    hps_byte_view taken = {data, 1U};
    status = hps_byte_reader_take_view(&reader, 0U, &taken);
    assert(status == HPS_BYTE_OK && taken.data == NULL && taken.length == 0U);
    assert(reader.position == 0U);
    status = hps_byte_reader_skip(&reader, 3U);
    assert(status == HPS_BYTE_OK && reader.position == 3U);
    status = hps_byte_reader_init(&reader, view);
    assert(status == HPS_BYTE_OK && reader.position == 0U);

    const hps_byte_view noncanonical_empty = {data, 0U};
    status = hps_byte_reader_init(&reader, noncanonical_empty);
    assert(status == HPS_BYTE_OK);
    assert(reader.view.data == NULL && reader.view.length == 0U);
    assert(reader.position == 0U);
    size_t remaining = SIZE_MAX;
    status = hps_byte_reader_remaining(&reader, &remaining);
    assert(status == HPS_BYTE_OK && remaining == 0U);
    status = hps_byte_reader_skip(&reader, 0U);
    assert(status == HPS_BYTE_OK);
    taken.data = data;
    taken.length = 1U;
    status = hps_byte_reader_take_view(&reader, 0U, &taken);
    assert(status == HPS_BYTE_OK && taken.data == NULL && taken.length == 0U);
    const hps_byte_reader empty = reader;
    uint8_t byte = 0xA5U;
    status = hps_byte_reader_read_u8(&reader, &byte);
    assert(status == HPS_BYTE_OUT_OF_RANGE && byte == 0xA5U);
    assert_reader_snapshot(&reader, empty);
    status = hps_byte_reader_skip(&reader, 1U);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert_reader_snapshot(&reader, empty);
    status = hps_byte_reader_take_view(&reader, 1U, &taken);
    assert(status == HPS_BYTE_OUT_OF_RANGE && taken.data == NULL && taken.length == 0U);
    assert_reader_snapshot(&reader, empty);
    const hps_byte_view canonical_empty = {NULL, 0U};
    status = hps_byte_reader_init(&reader, canonical_empty);
    assert(status == HPS_BYTE_OK);
    assert_reader_snapshot(&reader, empty);
}

static void test_reader_arguments_and_state(void)
{
    uint8_t data[3] = {0x11U, 0x22U, 0x33U};
    const uint8_t expected[3] = {0x11U, 0x22U, 0x33U};
    hps_byte_reader reader = {{data, 3U}, 1U};
    const hps_byte_reader original = reader;
    size_t remaining = SIZE_MAX;
    uint8_t byte = 0xA5U;
    hps_byte_view taken = {data, 1U};
    int32_t status = hps_byte_reader_remaining(NULL, &remaining);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && remaining == SIZE_MAX);
    status = hps_byte_reader_remaining(&reader, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_reader_skip(NULL, 0U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_reader_take_view(NULL, 0U, &taken);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && taken.data == data && taken.length == 1U);
    status = hps_byte_reader_take_view(&reader, 0U, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_reader_take_view(NULL, SIZE_MAX, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    status = hps_byte_reader_read_u8(NULL, &byte);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && byte == 0xA5U);
    status = hps_byte_reader_read_u8(&reader, NULL);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert_reader_snapshot(&reader, original);

    status = hps_byte_reader_skip(&reader, 3U);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert_reader_snapshot(&reader, original);
    status = hps_byte_reader_skip(&reader, SIZE_MAX);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert_reader_snapshot(&reader, original);
    status = hps_byte_reader_take_view(&reader, SIZE_MAX, &taken);
    assert(status == HPS_BYTE_OUT_OF_RANGE && taken.data == data && taken.length == 1U);
    assert_reader_snapshot(&reader, original);
    status = hps_byte_reader_take_view(&reader, 3U, &taken);
    assert(status == HPS_BYTE_OUT_OF_RANGE && taken.data == data && taken.length == 1U);
    assert_reader_snapshot(&reader, original);
    /* A rejected request leaves the reader usable for a smaller valid request. */
    status = hps_byte_reader_remaining(&reader, &remaining);
    assert(status == HPS_BYTE_OK && remaining == 2U);
    status = hps_byte_reader_take_view(&reader, 2U, &taken);
    assert(status == HPS_BYTE_OK && reader.position == 3U);
    assert(taken.data == &data[1] && taken.length == 2U);
    remaining = SIZE_MAX;
    taken.data = data;
    taken.length = 1U;

    const hps_byte_reader malformed[4] = {
        {{NULL, 1U}, 0U},
        {{data, 3U}, 4U},
        {{NULL, 0U}, 1U},
        {{data, 3U}, SIZE_MAX}
    };
    for (size_t index = 0U; index < 4U; index += 1U) {
        reader = malformed[index];
        status = hps_byte_reader_remaining(&reader, &remaining);
        assert(status == HPS_BYTE_INVALID_STATE && remaining == SIZE_MAX);
        status = hps_byte_reader_skip(&reader, 0U);
        assert(status == HPS_BYTE_INVALID_STATE);
        status = hps_byte_reader_skip(&reader, SIZE_MAX);
        assert(status == HPS_BYTE_INVALID_STATE);
        status = hps_byte_reader_take_view(&reader, 0U, &taken);
        assert(status == HPS_BYTE_INVALID_STATE && taken.data == data && taken.length == 1U);
        status = hps_byte_reader_take_view(&reader, SIZE_MAX, &taken);
        assert(status == HPS_BYTE_INVALID_STATE && taken.data == data && taken.length == 1U);
        status = hps_byte_reader_read_u8(&reader, &byte);
        assert(status == HPS_BYTE_INVALID_STATE && byte == 0xA5U);
        status = hps_byte_reader_remaining(&reader, NULL);
        assert(status == HPS_BYTE_INVALID_ARGUMENT);
        status = hps_byte_reader_take_view(&reader, 0U, NULL);
        assert(status == HPS_BYTE_INVALID_ARGUMENT);
        status = hps_byte_reader_read_u8(&reader, NULL);
        assert(status == HPS_BYTE_INVALID_ARGUMENT);
        assert_reader_snapshot(&reader, malformed[index]);
    }
    assert_reader_payload(data, expected, 3U);
}

static void test_reader_size_bounds(void)
{
    const uint8_t data[1] = {0x42U};
    /* Test-only synthetic extent: descriptor-only calls never form a payload
     * pointer or access bytes. This does not assert a real SIZE_MAX-byte array. */
    hps_byte_reader reader = {{data, SIZE_MAX}, SIZE_MAX - 1U};
    size_t remaining = SIZE_MAX;
    int32_t status = hps_byte_reader_remaining(&reader, &remaining);
    assert(status == HPS_BYTE_OK && remaining == 1U);
    const hps_byte_reader before = reader;
    status = hps_byte_reader_skip(&reader, SIZE_MAX);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert_reader_snapshot(&reader, before);
    status = hps_byte_reader_skip(&reader, 1U);
    assert(status == HPS_BYTE_OK && reader.position == SIZE_MAX);
    status = hps_byte_reader_remaining(&reader, &remaining);
    assert(status == HPS_BYTE_OK && remaining == 0U);
    const hps_byte_reader eof = reader;
    status = hps_byte_reader_skip(&reader, 1U);
    assert(status == HPS_BYTE_OUT_OF_RANGE);
    assert_reader_snapshot(&reader, eof);
    status = hps_byte_reader_skip(&reader, 0U);
    assert(status == HPS_BYTE_OK);
    assert_reader_snapshot(&reader, eof);
    reader.position = 0U;
    status = hps_byte_reader_remaining(&reader, &remaining);
    assert(status == HPS_BYTE_OK && remaining == SIZE_MAX);
    status = hps_byte_reader_skip(&reader, SIZE_MAX);
    assert(status == HPS_BYTE_OK && reader.position == SIZE_MAX);
    assert(reader.view.data == data && reader.view.length == SIZE_MAX);
    assert(data[0] == 0x42U);
}

/* Each helper receives one explicit width/order fixture with a prefix and suffix.
 * All wire copying/indexing is bounded by ten bytes; effects stay outside assert. */
static void test_reader_u16(
    int32_t (*read)(hps_byte_reader *reader, uint16_t *result),
    const uint8_t *wire, uint16_t expected)
{
    uint8_t data[4];
    for (size_t index = 0U; index < 4U; index += 1U) {
        data[index] = wire[index];
    }
    hps_byte_reader reader = {{data, 4U}, 1U};
    uint16_t value = UINT16_C(0);
    int32_t status = read(&reader, &value);
    assert(status == HPS_BYTE_OK && value == expected && reader.position == 3U);
    assert(reader.view.data == data && reader.view.length == 4U);
    reader.view.length = 3U;
    reader.position = 1U;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OK && value == expected && reader.position == 3U);

    const uint16_t sentinel = UINT16_C(0x5AA5);
    value = sentinel;
    for (size_t available = 0U; available < 2U; available += 1U) {
        for (size_t position = 0U; position <= 1U; position += 1U) {
            reader.view.data = &data[1U - position];
            reader.view.length = available + position;
            reader.position = position;
            const hps_byte_reader snapshot = reader;
            status = read(&reader, &value);
            assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
            assert_reader_snapshot(&reader, snapshot);
            assert_reader_payload(data, wire, 4U);
        }
    }
    reader.view.data = data;
    reader.view.length = 4U;
    reader.position = 4U;
    const hps_byte_reader eof = reader;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    assert_reader_snapshot(&reader, eof);
    reader.view.data = NULL;
    reader.view.length = 0U;
    reader.position = 0U;
    const hps_byte_reader empty = reader;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    assert_reader_snapshot(&reader, empty);
    status = read(NULL, &value);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && value == sentinel);
    const hps_byte_reader states[4] = {
        {{data, 4U}, 1U},
        {{NULL, 4U}, 0U},
        {{data, 4U}, SIZE_MAX},
        {{NULL, 0U}, 1U}
    };
    for (size_t index = 0U; index < 4U; index += 1U) {
        reader = states[index];
        status = read(&reader, NULL);
        assert(status == HPS_BYTE_INVALID_ARGUMENT);
        assert_reader_snapshot(&reader, states[index]);
        if (index != 0U) {
            status = read(&reader, &value);
            assert(status == HPS_BYTE_INVALID_STATE && value == sentinel);
            assert_reader_snapshot(&reader, states[index]);
        }
    }
    assert_reader_payload(data, wire, 4U);
}

static void test_reader_u32(
    int32_t (*read)(hps_byte_reader *reader, uint32_t *result),
    const uint8_t *wire, uint32_t expected)
{
    uint8_t data[6];
    for (size_t index = 0U; index < 6U; index += 1U) {
        data[index] = wire[index];
    }
    hps_byte_reader reader = {{data, 6U}, 1U};
    uint32_t value = UINT32_C(0);
    int32_t status = read(&reader, &value);
    assert(status == HPS_BYTE_OK && value == expected && reader.position == 5U);
    assert(reader.view.data == data && reader.view.length == 6U);
    reader.view.length = 5U;
    reader.position = 1U;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OK && value == expected && reader.position == 5U);

    const uint32_t sentinel = UINT32_C(0x5AA5C33C);
    value = sentinel;
    for (size_t available = 0U; available < 4U; available += 1U) {
        for (size_t position = 0U; position <= 1U; position += 1U) {
            reader.view.data = &data[1U - position];
            reader.view.length = available + position;
            reader.position = position;
            const hps_byte_reader snapshot = reader;
            status = read(&reader, &value);
            assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
            assert_reader_snapshot(&reader, snapshot);
            assert_reader_payload(data, wire, 6U);
        }
    }
    reader.view.data = data;
    reader.view.length = 6U;
    reader.position = 6U;
    const hps_byte_reader eof = reader;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    assert_reader_snapshot(&reader, eof);
    reader.view.data = NULL;
    reader.view.length = 0U;
    reader.position = 0U;
    const hps_byte_reader empty = reader;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    assert_reader_snapshot(&reader, empty);
    status = read(NULL, &value);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && value == sentinel);
    const hps_byte_reader states[4] = {
        {{data, 6U}, 1U},
        {{NULL, 6U}, 0U},
        {{data, 6U}, SIZE_MAX},
        {{NULL, 0U}, 1U}
    };
    for (size_t index = 0U; index < 4U; index += 1U) {
        reader = states[index];
        status = read(&reader, NULL);
        assert(status == HPS_BYTE_INVALID_ARGUMENT);
        assert_reader_snapshot(&reader, states[index]);
        if (index != 0U) {
            status = read(&reader, &value);
            assert(status == HPS_BYTE_INVALID_STATE && value == sentinel);
            assert_reader_snapshot(&reader, states[index]);
        }
    }
    assert_reader_payload(data, wire, 6U);
}

static void test_reader_u64(
    int32_t (*read)(hps_byte_reader *reader, uint64_t *result),
    const uint8_t *wire, uint64_t expected)
{
    uint8_t data[10];
    for (size_t index = 0U; index < 10U; index += 1U) {
        data[index] = wire[index];
    }
    hps_byte_reader reader = {{data, 10U}, 1U};
    uint64_t value = UINT64_C(0);
    int32_t status = read(&reader, &value);
    assert(status == HPS_BYTE_OK && value == expected && reader.position == 9U);
    assert(reader.view.data == data && reader.view.length == 10U);
    reader.view.length = 9U;
    reader.position = 1U;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OK && value == expected && reader.position == 9U);

    const uint64_t sentinel = UINT64_C(0x5AA5C33CF00F9669);
    value = sentinel;
    for (size_t available = 0U; available < 8U; available += 1U) {
        for (size_t position = 0U; position <= 1U; position += 1U) {
            reader.view.data = &data[1U - position];
            reader.view.length = available + position;
            reader.position = position;
            const hps_byte_reader snapshot = reader;
            status = read(&reader, &value);
            assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
            assert_reader_snapshot(&reader, snapshot);
            assert_reader_payload(data, wire, 10U);
        }
    }
    reader.view.data = data;
    reader.view.length = 10U;
    reader.position = 10U;
    const hps_byte_reader eof = reader;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    assert_reader_snapshot(&reader, eof);
    reader.view.data = NULL;
    reader.view.length = 0U;
    reader.position = 0U;
    const hps_byte_reader empty = reader;
    status = read(&reader, &value);
    assert(status == HPS_BYTE_OUT_OF_RANGE && value == sentinel);
    assert_reader_snapshot(&reader, empty);
    status = read(NULL, &value);
    assert(status == HPS_BYTE_INVALID_ARGUMENT && value == sentinel);
    const hps_byte_reader states[4] = {
        {{data, 10U}, 1U},
        {{NULL, 10U}, 0U},
        {{data, 10U}, SIZE_MAX},
        {{NULL, 0U}, 1U}
    };
    for (size_t index = 0U; index < 4U; index += 1U) {
        reader = states[index];
        status = read(&reader, NULL);
        assert(status == HPS_BYTE_INVALID_ARGUMENT);
        assert_reader_snapshot(&reader, states[index]);
        if (index != 0U) {
            status = read(&reader, &value);
            assert(status == HPS_BYTE_INVALID_STATE && value == sentinel);
            assert_reader_snapshot(&reader, states[index]);
        }
    }
    assert_reader_payload(data, wire, 10U);
}

static void test_reader_numbers(void)
{
    const uint8_t le16[4] = {0xD7U, 0x34U, 0x12U, 0xE9U};
    const uint8_t be16[4] = {0xD7U, 0x12U, 0x34U, 0xE9U};
    const uint8_t le32[6] = {0xD7U, 0xEFU, 0xCDU, 0xABU, 0x89U, 0xE9U};
    const uint8_t be32[6] = {0xD7U, 0x89U, 0xABU, 0xCDU, 0xEFU, 0xE9U};
    const uint8_t le64[10] = {
        0xD7U, 0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U, 0xE9U
    };
    const uint8_t be64[10] = {
        0xD7U, 0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU, 0xE9U
    };
    test_reader_u16(hps_byte_reader_read_u16_le, le16, UINT16_C(0x1234));
    test_reader_u16(hps_byte_reader_read_u16_be, be16, UINT16_C(0x1234));
    test_reader_u32(hps_byte_reader_read_u32_le, le32, UINT32_C(0x89ABCDEF));
    test_reader_u32(hps_byte_reader_read_u32_be, be32, UINT32_C(0x89ABCDEF));
    test_reader_u64(hps_byte_reader_read_u64_le, le64, UINT64_C(0x0123456789ABCDEF));
    test_reader_u64(hps_byte_reader_read_u64_be, be64, UINT64_C(0x0123456789ABCDEF));
}

int32_t test_reader(void)
{
    test_reader_sequence();
    test_reader_independent_cursors();
    test_reader_initialization_and_empty();
    test_reader_arguments_and_state();
    test_reader_size_bounds();
    test_reader_numbers();
    return 0;
}
