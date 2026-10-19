#include "../test_suites.h"
#include "hps/memory/operations.h"
#include <assert.h>

/**
 * @file tests/memory/test_memory_operations.c
 * @brief Primitive byte contracts, overlap, exact differences and extent guards.
 */

#ifdef NDEBUG
#error "Memory operation tests require enabled assertions"
#endif

static void test_empty_ranges(void)
{
    uint8_t byte = 79U;
    void *result = hps_memset(NULL, 255U, 0U);
    assert(result == NULL);
    result = hps_memcpy(NULL, NULL, 0U);
    assert(result == NULL);
    result = hps_memcpy(&byte, NULL, 0U);
    assert(result == &byte);
    result = hps_memmove(NULL, NULL, 0U);
    assert(result == NULL);
    result = hps_memmove(&byte, NULL, 0U);
    assert(result == &byte);
    int32_t comparison = hps_memcmp(NULL, NULL, 0U);
    assert(comparison == INT32_C(0));
    const void *found = hps_memchr(NULL, 0U, 0U);
    assert(found == NULL);
    hps_memzero_volatile(NULL, 0U);
    hps_memzero_volatile(&byte, 0U);
    assert(byte == 79U);
}

static void test_fill_and_copy(void)
{
    uint8_t destination[35] = {0U};
    const uint8_t source[17] = {
        0U, 255U, 1U, 254U, 2U, 253U, 3U, 252U, 4U,
        251U, 5U, 250U, 6U, 249U, 7U, 248U, 8U
    };
    destination[0] = 43U;
    destination[34] = 61U;
    void *result = hps_memset(&destination[1], 219U, 33U);
    assert(result == &destination[1]);
    assert(destination[0] == 43U);
    assert(destination[34] == 61U);
    for (size_t index = 1U; index < 34U; index += 1U) {
        assert(destination[index] == 219U);
    }
    result = hps_memcpy(&destination[3], source, sizeof(source));
    assert(result == &destination[3]);
    for (size_t index = 0U; index < sizeof(source); index += 1U) {
        assert(destination[index + 3U] == source[index]);
    }
    assert(destination[2] == 219U);
    assert(destination[20] == 219U);
    assert(destination[0] == 43U);
    assert(destination[34] == 61U);
    result = hps_memset(&destination[17], 255U, 1U);
    assert(result == &destination[17]);
    assert(destination[16] == source[13]);
    assert(destination[17] == 255U);
    assert(destination[18] == source[15]);
}

static void test_compare_and_find(void)
{
    const uint8_t left[] = {23U, 0U, 255U, 7U, 0U};
    const uint8_t right[] = {23U, 255U, 0U, 7U, 0U};
    int32_t comparison = hps_memcmp(left, right, sizeof(left));
    assert(comparison == -INT32_C(255));
    comparison = hps_memcmp(right, left, sizeof(left));
    assert(comparison == INT32_C(255));
    comparison = hps_memcmp(left, right, 1U);
    assert(comparison == INT32_C(0));
    comparison = hps_memcmp(left, left, sizeof(left));
    assert(comparison == INT32_C(0));
    const uint8_t first[] = {1U, 2U, 10U, 255U};
    const uint8_t second[] = {1U, 2U, 13U, 0U};
    comparison = hps_memcmp(first, second, sizeof(first));
    assert(comparison == -INT32_C(3));
    const void *found = hps_memchr(left, 0U, sizeof(left));
    assert(found == &left[1]);
    found = hps_memchr(left, 255U, sizeof(left));
    assert(found == &left[2]);
    found = hps_memchr(left, 0U, 1U);
    assert(found == NULL);
    found = hps_memchr(left, 42U, sizeof(left));
    assert(found == NULL);
}

static void test_overlapping_move(void)
{
    uint8_t rightward[] = {10U, 20U, 30U, 40U, 50U, 60U, 70U};
    const uint8_t expected_right[] = {10U, 20U, 10U, 20U, 30U, 40U, 50U};
    void *result = hps_memmove(&rightward[2], rightward, 5U);
    assert(result == &rightward[2]);
    for (size_t index = 0U; index < sizeof(rightward); index += 1U) {
        assert(rightward[index] == expected_right[index]);
    }
    uint8_t leftward[] = {10U, 20U, 30U, 40U, 50U, 60U, 70U};
    const uint8_t expected_left[] = {30U, 40U, 50U, 60U, 70U, 60U, 70U};
    result = hps_memmove(leftward, &leftward[2], 5U);
    assert(result == leftward);
    result = hps_memmove(leftward, leftward, sizeof(leftward));
    assert(result == leftward);
    for (size_t index = 0U; index < sizeof(leftward); index += 1U) {
        assert(leftward[index] == expected_left[index]);
    }
    uint8_t disjoint[7] = {0U};
    result = hps_memmove(disjoint, expected_left, sizeof(disjoint));
    assert(result == disjoint);
    for (size_t index = 0U; index < sizeof(disjoint); index += 1U) {
        assert(disjoint[index] == expected_left[index]);
    }
}

static void test_volatile_zero_bounds(void)
{
    uint8_t storage[] = {91U, 255U, 1U, 2U, 3U, 4U, 92U};
    hps_memzero_volatile(&storage[1], 5U);
    assert(storage[0] == 91U);
    assert(storage[6] == 92U);
    for (size_t index = 1U; index < 6U; index += 1U) {
        assert(storage[index] == 0U);
    }
}

int32_t test_memory_operations(void)
{
    test_empty_ranges();
    test_fill_and_copy();
    test_compare_and_find();
    test_overlapping_move();
    test_volatile_zero_bounds();
    return INT32_C(0);
}
