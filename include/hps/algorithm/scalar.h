#ifndef HPS_ALGORITHM_SCALAR_H
#define HPS_ALGORITHM_SCALAR_H

/**
 * @file hps/algorithm/scalar.h
 * @brief Typed scalar minimum and maximum operations with single evaluation.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Each argument is evaluated once by ordinary function-call rules; their
 * relative evaluation order is unspecified. Callers supply initialized values
 * representable in the selected parameter type, without conflicting effects.
 * These functions only compare; they do not add, subtract or narrow internally.
 * They return a for equal inputs and own no storage. Each has O(1) work, scalar
 * automatic state, no heap, blocking, shared state or execution-context dependency.
 * Actual stack/code generation requires qualification of the selected target.
 * The three specializations retain explicit signed/unsigned/size contracts;
 * generic selection macros would obscure conversions and the C99 baseline. */
static HPS_INLINE int64_t hps_i64_min(int64_t a, int64_t b)
{
    return (a <= b) ? a : b;
}

static HPS_INLINE int64_t hps_i64_max(int64_t a, int64_t b)
{
    return (a >= b) ? a : b;
}

static HPS_INLINE uint64_t hps_u64_min(uint64_t a, uint64_t b)
{
    return (a <= b) ? a : b;
}

static HPS_INLINE uint64_t hps_u64_max(uint64_t a, uint64_t b)
{
    return (a >= b) ? a : b;
}

static HPS_INLINE size_t hps_size_min(size_t a, size_t b)
{
    return (a <= b) ? a : b;
}

static HPS_INLINE size_t hps_size_max(size_t a, size_t b)
{
    return (a >= b) ? a : b;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_ALGORITHM_SCALAR_H */
