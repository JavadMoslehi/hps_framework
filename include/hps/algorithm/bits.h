#ifndef HPS_ALGORITHM_BITS_H
#define HPS_ALGORITHM_BITS_H

/**
 * @file hps/algorithm/bits.h
 * @brief Checked bit operations on exact-width 64-bit unsigned values.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/attributes.h"

#define HPS_BIT_SUCCESS ((int32_t)0)
#define HPS_BIT_ERR_NULL ((int32_t)-1)
#define HPS_BIT_ERR_RANGE ((int32_t)-2)

#ifdef __cplusplus
extern "C" {
#endif

/* index is a zero-based bit index, 0..63 inclusive. out must be NULL or point
 * to one live, writable object of its declared type; a non-NULL arbitrary pointer
 * is not validated. NULL out takes priority over an out-of-range index. Failure
 * preserves *out; success writes the result exactly once. The uint64_t value
 * inputs are copied by value, so the output may be their original source object.
 * Caller owns all objects and serializes any concurrent access to the output.
 * Each call has O(1) work, at most one uint64_t scalar local and two int32_t
 * scalar locals across nested calls plus parameters/call state; no loops, heap,
 * blocking, callbacks or
 * shared state. Actual stack and compiler runtime helpers need target review.
 * Exact uint8_t, uint64_t and int32_t are required. No shift is evaluated until
 * index < 64 has been established. No implicit in-place macro mutation occurs. */
static HPS_INLINE int32_t hps_u64_bit_mask(size_t index, uint64_t *out)
{
    int32_t result = HPS_BIT_SUCCESS;

    if (out == NULL) {
        result = HPS_BIT_ERR_NULL;
    } else if (index >= (size_t)64) {
        result = HPS_BIT_ERR_RANGE;
    } else {
        *out = UINT64_C(1) << index;
    }
    return result;
}

static HPS_INLINE int32_t hps_u64_set_bit(uint64_t value, size_t index,
                                      uint64_t *out)
{
    int32_t result = HPS_BIT_SUCCESS;
    uint64_t mask = UINT64_C(0);

    if (out == NULL) {
        result = HPS_BIT_ERR_NULL;
    } else {
        result = hps_u64_bit_mask(index, &mask);
        if (result == HPS_BIT_SUCCESS) {
            *out = value | mask;
        }
    }
    return result;
}

static HPS_INLINE int32_t hps_u64_clear_bit(uint64_t value, size_t index,
                                        uint64_t *out)
{
    int32_t result = HPS_BIT_SUCCESS;
    uint64_t mask = UINT64_C(0);

    if (out == NULL) {
        result = HPS_BIT_ERR_NULL;
    } else {
        result = hps_u64_bit_mask(index, &mask);
        if (result == HPS_BIT_SUCCESS) {
            *out = value & ~mask;
        }
    }
    return result;
}

/* On success, writes precisely uint8_t 0 or 1 rather than a uint64_t mask. */
static HPS_INLINE int32_t hps_u64_test_bit(uint64_t value, size_t index,
                                       uint8_t *out)
{
    int32_t result = HPS_BIT_SUCCESS;
    uint64_t mask = UINT64_C(0);

    if (out == NULL) {
        result = HPS_BIT_ERR_NULL;
    } else {
        result = hps_u64_bit_mask(index, &mask);
        if (result == HPS_BIT_SUCCESS) {
            *out = (uint8_t)(((value & mask) != UINT64_C(0)) ? 1U : 0U);
        }
    }
    return result;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_ALGORITHM_BITS_H */
