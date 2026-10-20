#include "../test_suites.h"
#include "hps/bytes/view.h"
#include "hps/bytes/view.h"
#include "hps/bytes/buffer.h"
#include "hps/bytes/buffer.h"

#include <assert.h>
#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include <string.h>

/**
 * @file tests/bytes/test_byte_algorithms.c
 * @brief Independent binary search/order models and checked pop/alias boundaries.
 */

static void test_binary_examples_and_overlapping_inputs(void)
{
    const uint8_t storage[7] = {0U, 255U, 0U, 128U, 0U, 255U, 0U};
    const uint8_t other_storage[3] = {0U, 255U, 1U};
    const hps_byte_view view = {storage, 7U};
    const hps_byte_view repeated = {storage, 3U};
    const hps_byte_view high = {&storage[1], 1U};
    const hps_byte_view other = {other_storage, 3U};
    const hps_byte_view empty = {NULL, 0U};
    size_t index = 71U;
    int32_t order = INT32_C(71);
    uint8_t matches = UINT8_C(71);

    assert(hps_byte_view_find(view, repeated, &index) == HPS_BYTE_OK);
    assert(index == 0U);
    assert(hps_byte_view_rfind(view, repeated, &index) == HPS_BYTE_OK);
    assert(index == 4U);
    assert(hps_byte_view_find(view, other, &index) == HPS_BYTE_OK);
    assert(index == HPS_BYTE_NPOS);
    assert(hps_byte_view_rfind(view, other, &index) == HPS_BYTE_OK);
    assert(index == HPS_BYTE_NPOS);
    assert(hps_byte_view_find(view, view, &index) == HPS_BYTE_OK && index == 0U);
    assert(hps_byte_view_rfind(view, view, &index) == HPS_BYTE_OK && index == 0U);
    assert(hps_byte_view_compare(repeated, other, &order) == HPS_BYTE_OK);
    assert(order == -INT32_C(1));
    assert(hps_byte_view_compare(other, repeated, &order) == HPS_BYTE_OK);
    assert(order == INT32_C(1));
    assert(hps_byte_view_compare(view, view, &order) == HPS_BYTE_OK);
    assert(order == INT32_C(0));
    assert(hps_byte_view_compare(repeated, view, &order) == HPS_BYTE_OK);
    assert(order == -INT32_C(1));
    assert(hps_byte_view_compare(high, other, &order) == HPS_BYTE_OK);
    assert(order == INT32_C(1));
    assert(hps_byte_view_starts_with(view, repeated, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(1));
    assert(hps_byte_view_ends_with(view, repeated, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(1));
    assert(hps_byte_view_starts_with(view, high, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(0));
    assert(hps_byte_view_ends_with(view, high, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(0));
    assert(hps_byte_view_starts_with(view, empty, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(1));
    assert(hps_byte_view_ends_with(empty, empty, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(1));
    assert(hps_byte_view_find(view, empty, &index) == HPS_BYTE_OK);
    assert(index == HPS_BYTE_NPOS);
    assert(hps_byte_view_rfind(empty, empty, &index) == HPS_BYTE_OK);
    assert(index == HPS_BYTE_NPOS);
}

/* The independent model visits every haystack position forwards, accumulates
 * first AND last matches and never exits a byte comparison early. */
static void expected_matches(const uint8_t *haystack, const size_t haystack_length,
                              const uint8_t *needle, const size_t needle_length,
                              size_t *first, size_t *last)
{
    *first = HPS_BYTE_NPOS;
    *last = HPS_BYTE_NPOS;
    for (size_t candidate = 0U; candidate < haystack_length; candidate += 1U) {
        if ((needle_length != 0U) &&
            (needle_length <= haystack_length - candidate)) {
            uint8_t same = UINT8_C(1);
            for (size_t offset = 0U; offset < needle_length; offset += 1U) {
                if (haystack[candidate + offset] != needle[offset]) {
                    same = UINT8_C(0);
                }
            }
            if (same == UINT8_C(1)) {
                if (*first == HPS_BYTE_NPOS) { *first = candidate; }
                *last = candidate;
            }
        }
    }
}

static int32_t expected_order(const hps_byte_view left, const hps_byte_view right)
{
    const size_t common = (left.length < right.length) ? left.length : right.length;
    int32_t order = INT32_C(0);
    if (common != 0U) {
        if (memcmp(left.data, right.data, common) < 0) { order = -INT32_C(1); }
        else if (memcmp(left.data, right.data, common) > 0) { order = INT32_C(1); }
        else { /* Matching prefixes are ordered by length below. */ }
    }
    if (order == INT32_C(0)) {
        if (left.length < right.length) { order = -INT32_C(1); }
        else if (left.length > right.length) { order = INT32_C(1); }
        else { /* Equal representations and lengths remain equal. */ }
    }
    return order;
}

static void test_small_binary_oracle(void)
{
    /* Five/two-valued haystack positions and three needle positions give 945
     * independent length/pattern pairs, including every empty combination. */
    for (size_t length = 0U; length <= 5U; length += 1U) {
        for (uint32_t bits = UINT32_C(0); bits < (UINT32_C(1) << length); bits += UINT32_C(1)) {
            uint8_t haystack[5] = {0U};
            for (size_t index = 0U; index < length; index += 1U) {
                haystack[index] = ((bits & (UINT32_C(1) << index)) == UINT32_C(0)) ? 0U : 255U;
            }
            const hps_byte_view view = {(length == 0U) ? NULL : haystack, length};
            for (size_t needle_length = 0U; needle_length <= 3U; needle_length += 1U) {
                for (uint32_t pattern = UINT32_C(0);
                     pattern < (UINT32_C(1) << needle_length); pattern += UINT32_C(1)) {
                    uint8_t bytes[3] = {0U};
                    for (size_t index = 0U; index < needle_length; index += 1U) {
                        bytes[index] = ((pattern & (UINT32_C(1) << index)) == UINT32_C(0)) ? 0U : 255U;
                    }
                    const hps_byte_view needle = {(needle_length == 0U) ? NULL : bytes, needle_length};
                    size_t first = 0U;
                    size_t last = 0U;
                    size_t actual = 0U;
                    int32_t order = INT32_C(71);
                    uint8_t matches = UINT8_C(71);
                    expected_matches(haystack, length, bytes, needle_length, &first, &last);
                    assert(hps_byte_view_find(view, needle, &actual) == HPS_BYTE_OK && actual == first);
                    assert(hps_byte_view_rfind(view, needle, &actual) == HPS_BYTE_OK && actual == last);
                    assert(hps_byte_view_compare(view, needle, &order) == HPS_BYTE_OK);
                    assert(order == expected_order(view, needle));
                    assert(hps_byte_view_starts_with(view, needle, &matches) == HPS_BYTE_OK);
                    assert(matches == (uint8_t)((needle_length == 0U) ||
                        ((needle_length <= length) && (memcmp(haystack, bytes, needle_length) == 0))));
                    assert(hps_byte_view_ends_with(view, needle, &matches) == HPS_BYTE_OK);
                    assert(matches == (uint8_t)((needle_length == 0U) ||
                        ((needle_length <= length) &&
                         (memcmp(&haystack[length - needle_length], bytes, needle_length) == 0))));
                }
            }
        }
    }
}

static void test_invalid_views_and_metadata_only_bounds(void)
{
    const uint8_t storage[1] = {UINT8_C(71)};
    const hps_byte_view view = {storage, 1U};
    const hps_byte_view empty = {NULL, 0U};
    const hps_byte_view malformed = {NULL, SIZE_MAX};
    size_t index = 71U;
    int32_t order = INT32_C(71);
    uint8_t matches = UINT8_C(71);

    assert(hps_byte_view_compare(malformed, view, &order) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_compare(view, malformed, &order) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_compare(view, view, NULL) == HPS_BYTE_INVALID_ARGUMENT);
    assert(order == INT32_C(71));
    assert(hps_byte_view_starts_with(malformed, view, &matches) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_starts_with(view, malformed, &matches) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_ends_with(malformed, view, &matches) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_ends_with(view, malformed, &matches) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_starts_with(view, view, NULL) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_ends_with(view, view, NULL) == HPS_BYTE_INVALID_ARGUMENT);
    assert(matches == UINT8_C(71));
    assert(hps_byte_view_find(malformed, view, &index) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_find(view, malformed, &index) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_rfind(malformed, view, &index) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_rfind(view, malformed, &index) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_find(view, view, NULL) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_view_rfind(view, view, NULL) == HPS_BYTE_INVALID_ARGUMENT);
    assert(index == 71U);

    /* Synthetic metadata only: none of these calls form/access a payload
     * pointer. This does not claim a real SIZE_MAX-element backing array. */
    const hps_byte_view huge = {storage, SIZE_MAX};
    assert(hps_byte_view_compare(huge, empty, &order) == HPS_BYTE_OK);
    assert(order == INT32_C(1));
    assert(hps_byte_view_compare(empty, huge, &order) == HPS_BYTE_OK);
    assert(order == -INT32_C(1));
    assert(hps_byte_view_starts_with(huge, empty, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(1));
    assert(hps_byte_view_ends_with(huge, empty, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(1));
    assert(hps_byte_view_find(huge, empty, &index) == HPS_BYTE_OK);
    assert(index == HPS_BYTE_NPOS);
    assert(hps_byte_view_rfind(huge, empty, &index) == HPS_BYTE_OK);
    assert(index == HPS_BYTE_NPOS);
    assert(hps_byte_view_starts_with(view, huge, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(0));
    assert(hps_byte_view_ends_with(view, huge, &matches) == HPS_BYTE_OK);
    assert(matches == UINT8_C(0));
    assert(hps_byte_view_find(view, huge, &index) == HPS_BYTE_OK);
    assert(index == HPS_BYTE_NPOS);
    assert(hps_byte_view_rfind(view, huge, &index) == HPS_BYTE_OK);
    assert(index == HPS_BYTE_NPOS);
    assert(storage[0] == UINT8_C(71));
}

static void test_pop_and_export_composition(void)
{
    uint8_t storage[5] = {UINT8_C(0), UINT8_C(128), UINT8_C(255),
                           UINT8_C(71), UINT8_C(72)};
    const uint8_t before[5] = {UINT8_C(0), UINT8_C(128), UINT8_C(255),
                               UINT8_C(71), UINT8_C(72)};
    hps_byte_buffer buffer = {storage, 3U, 5U};
    uint8_t value = UINT8_C(73);

    assert(hps_byte_buffer_pop_u8(NULL, &value) == HPS_BYTE_INVALID_ARGUMENT);
    assert(hps_byte_buffer_pop_u8(&buffer, NULL) == HPS_BYTE_INVALID_ARGUMENT);
    assert(buffer.data == storage && buffer.length == 3U && buffer.capacity == 5U);
    assert(value == UINT8_C(73));
    assert(hps_byte_buffer_pop_u8(&buffer, &value) == HPS_BYTE_OK);
    assert(value == UINT8_C(255) && buffer.length == 2U);
    assert(hps_byte_buffer_pop_u8(&buffer, &value) == HPS_BYTE_OK);
    assert(value == UINT8_C(128) && buffer.length == 1U);
    assert(hps_byte_buffer_pop_u8(&buffer, &value) == HPS_BYTE_OK);
    assert(value == UINT8_C(0) && buffer.length == 0U);
    assert(hps_byte_buffer_pop_u8(&buffer, &value) == HPS_BYTE_OUT_OF_RANGE);
    assert(value == UINT8_C(0) && buffer.length == 0U);
    assert(memcmp(storage, before, sizeof(storage)) == 0);
    assert(buffer.data == storage && buffer.capacity == 5U);

    buffer.length = SIZE_MAX;
    value = UINT8_C(73);
    assert(hps_byte_buffer_pop_u8(&buffer, &value) == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == storage && buffer.length == SIZE_MAX && buffer.capacity == 5U);
    assert(value == UINT8_C(73) && memcmp(storage, before, sizeof(storage)) == 0);
    assert(hps_byte_buffer_pop_u8(&buffer, NULL) == HPS_BYTE_INVALID_ARGUMENT);
    buffer.data = NULL;
    buffer.length = 0U;
    buffer.capacity = 1U;
    assert(hps_byte_buffer_pop_u8(&buffer, &value) == HPS_BYTE_INVALID_STATE);
    assert(buffer.data == NULL && buffer.length == 0U && buffer.capacity == 1U);
    assert(value == UINT8_C(73));
    assert(hps_byte_buffer_init(&buffer, NULL, 0U) == HPS_BYTE_OK);
    assert(hps_byte_buffer_pop_u8(&buffer, &value) == HPS_BYTE_OUT_OF_RANGE);
    assert(value == UINT8_C(73));

    /* The existing assign engine already exports a view into bounded output,
     * including overlapping storage, without another byte-export API. */
    assert(hps_byte_buffer_init(&buffer, &storage[1], 4U) == HPS_BYTE_OK);
    const hps_byte_view source = {storage, 3U};
    assert(hps_byte_buffer_assign_bytes(&buffer, source) == HPS_BYTE_OK);
    assert(buffer.length == 3U && buffer.capacity == 4U);
    const uint8_t copied[5] = {0U, 0U, 128U, 255U, 72U};
    assert(memcmp(storage, copied, sizeof(storage)) == 0);
}

int32_t test_byte_algorithms(void)
{
    test_binary_examples_and_overlapping_inputs();
    test_small_binary_oracle();
    test_invalid_views_and_metadata_only_bounds();
    test_pop_and_export_composition();
    return INT32_C(0);
}
