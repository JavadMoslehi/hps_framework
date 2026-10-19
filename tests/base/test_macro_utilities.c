#include "../test_suites.h"
#include "hps/base/macros.h"
#include "hps/base/attributes.h"
#include "hps/algorithm/scalar.h"
#include "hps/algorithm/bits.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#ifdef NDEBUG
# error "Macro utility tests require enabled assertions"
#endif

int32_t test_macro_utilities(void)
{
    uint32_t matrix[2][3] = {{1U, 2U, 3U}, {4U, 5U, 6U}};
    size_t calls = 0U;
    HPS_VALUE(uint32_t) value = 7U;
    HPS_CONST_VALUE(uint32_t) const_value = 8U;
    HPS_POINTER(uint32_t) pointer = &value;
    HPS_POINTER_TO_CONST(uint32_t) pointer_to_const = &const_value;
    HPS_CONST_POINTER(uint32_t) const_pointer = &value;
    HPS_CONST_POINTER_TO_CONST(uint32_t) const_pointer_to_const = &const_value;
    int64_t signed_a = INT64_C(-5);
    int64_t signed_b = INT64_C(5);
    uint64_t unsigned_a = UINT64_C(1);
    uint64_t unsigned_b = UINT64_C(8);
    size_t size_a = 1U;
    size_t size_b = 8U;
    uint64_t word = UINT64_C(0);
    uint8_t flag = 9U;
    uint64_t outputs[2] = {UINT64_C(0), UINT64_C(0)};
    size_t output_index = 0U;

    assert(HPS_COUNT_OF(matrix) == 2U);
    assert(HPS_ARRAY_SIZE(matrix[0]) == 3U);
    assert(HPS_ARRAY_LENGTH(matrix) == 2U);
    assert(HPS_COUNT_OF(matrix[calls++]) == 3U);
    assert(calls == 0U);
    assert(*pointer == 7U && *const_pointer == 7U);
    assert(*pointer_to_const == 8U && *const_pointer_to_const == 8U);
    HPS_UNUSED(++calls);
    assert(calls == 1U);

    assert(hps_i64_min(INT64_MIN, INT64_MAX) == INT64_MIN);
    assert(hps_i64_max(INT64_MIN, INT64_MAX) == INT64_MAX);
    assert(hps_i64_min(INT64_C(-1), INT64_C(-1)) == INT64_C(-1));
    assert(hps_i64_max(INT64_C(-1), INT64_C(-1)) == INT64_C(-1));
    assert(hps_u64_min(UINT64_MAX, UINT64_C(0)) == UINT64_C(0));
    assert(hps_u64_max(UINT64_MAX, UINT64_C(0)) == UINT64_MAX);
    assert(hps_size_min(SIZE_MAX, (size_t)0) == 0U);
    assert(hps_size_max(SIZE_MAX, (size_t)0) == SIZE_MAX);
    assert(hps_i64_min(++signed_a, ++signed_b) == INT64_C(-4));
    assert(signed_a == INT64_C(-4) && signed_b == INT64_C(6));
    assert(hps_i64_max(++signed_a, ++signed_b) == INT64_C(7));
    assert(signed_a == INT64_C(-3) && signed_b == INT64_C(7));
    assert(hps_u64_min(++unsigned_a, ++unsigned_b) == UINT64_C(2));
    assert(unsigned_a == UINT64_C(2) && unsigned_b == UINT64_C(9));
    assert(hps_u64_max(++unsigned_a, ++unsigned_b) == UINT64_C(10));
    assert(unsigned_a == UINT64_C(3) && unsigned_b == UINT64_C(10));
    assert(hps_size_min(++size_a, ++size_b) == 2U);
    assert(size_a == 2U && size_b == 9U);
    assert(hps_size_max(++size_a, ++size_b) == 10U);
    assert(size_a == 3U && size_b == 10U);

    assert(hps_u64_bit_mask(0U, &word) == HPS_BIT_SUCCESS);
    assert(word == UINT64_C(1));
    assert(hps_u64_bit_mask(63U, &word) == HPS_BIT_SUCCESS);
    assert(word == UINT64_C(0x8000000000000000));
    assert(hps_u64_set_bit(UINT64_C(5), 1U, &word) == HPS_BIT_SUCCESS);
    assert(word == UINT64_C(7));
    assert(hps_u64_set_bit(UINT64_C(5), 2U, &word) == HPS_BIT_SUCCESS);
    assert(word == UINT64_C(5));
    assert(hps_u64_clear_bit(UINT64_MAX, 63U, &word) == HPS_BIT_SUCCESS);
    assert(word == UINT64_C(0x7fffffffffffffff));
    assert(hps_u64_clear_bit(UINT64_C(5), 1U, &word) == HPS_BIT_SUCCESS);
    assert(word == UINT64_C(5));
    assert(hps_u64_test_bit(UINT64_C(0x8000000000000000), 63U, &flag) == HPS_BIT_SUCCESS);
    assert(flag == 1U);
    assert(hps_u64_test_bit(UINT64_C(5), 1U, &flag) == HPS_BIT_SUCCESS);
    assert(flag == 0U);
    assert(hps_u64_test_bit(UINT64_C(5), 0U, &flag) == HPS_BIT_SUCCESS);
    assert(flag == 1U);
    word = UINT64_C(0);
    assert(hps_u64_set_bit(word, 63U, &word) == HPS_BIT_SUCCESS);
    assert(word == UINT64_C(0x8000000000000000));
    assert(hps_u64_clear_bit(word, 63U, &word) == HPS_BIT_SUCCESS);
    assert(word == UINT64_C(0));

    calls = 0U;
    assert(hps_u64_bit_mask(calls++, &outputs[output_index++]) == HPS_BIT_SUCCESS);
    assert(calls == 1U && output_index == 1U && outputs[0] == UINT64_C(1));
    unsigned_a = UINT64_C(4);
    assert(hps_u64_set_bit(++unsigned_a, calls++, &outputs[output_index++]) == HPS_BIT_SUCCESS);
    assert(unsigned_a == UINT64_C(5) && calls == 2U && output_index == 2U);
    assert(outputs[1] == UINT64_C(7));

    word = UINT64_C(42);
    flag = 9U;
    assert(hps_u64_bit_mask(64U, &word) == HPS_BIT_ERR_RANGE);
    assert(word == UINT64_C(42));
    assert(hps_u64_bit_mask(SIZE_MAX, &word) == HPS_BIT_ERR_RANGE);
    assert(word == UINT64_C(42));
    assert(hps_u64_set_bit(UINT64_C(0), 64U, &word) == HPS_BIT_ERR_RANGE);
    assert(word == UINT64_C(42));
    assert(hps_u64_clear_bit(UINT64_MAX, SIZE_MAX, &word) == HPS_BIT_ERR_RANGE);
    assert(word == UINT64_C(42));
    assert(hps_u64_test_bit(UINT64_MAX, 64U, &flag) == HPS_BIT_ERR_RANGE);
    assert(flag == 9U);
    assert(hps_u64_bit_mask(0U, NULL) == HPS_BIT_ERR_NULL);
    assert(hps_u64_set_bit(UINT64_C(0), 0U, NULL) == HPS_BIT_ERR_NULL);
    assert(hps_u64_clear_bit(UINT64_MAX, 0U, NULL) == HPS_BIT_ERR_NULL);
    assert(hps_u64_test_bit(UINT64_MAX, 0U, NULL) == HPS_BIT_ERR_NULL);
    assert(hps_u64_bit_mask(SIZE_MAX, NULL) == HPS_BIT_ERR_NULL);
    assert(hps_u64_set_bit(UINT64_C(0), SIZE_MAX, NULL) == HPS_BIT_ERR_NULL);
    assert(hps_u64_clear_bit(UINT64_MAX, SIZE_MAX, NULL) == HPS_BIT_ERR_NULL);
    assert(hps_u64_test_bit(UINT64_MAX, SIZE_MAX, NULL) == HPS_BIT_ERR_NULL);
    return INT32_C(0);
}
