#ifndef HPS_BASE_HPS_EXPRESSION_H
#define HPS_BASE_HPS_EXPRESSION_H

/**
 * @file hps/base/hps_expression.h
 * @brief Single-evaluation branch hints and C99 object expressions.
 */

#include "hps/base/hps_compiler.h"
#include "hps/base/features/hps_compiler.h"
#include "hps/base/hps_standard.h"

/* Hints affect optimization only; both paths normalize to 0/1 and evaluate x once.
 * No measured performance or prediction guarantee follows from their use. */
#if HPS_HAS_BUILTIN_EXPECT
# define HPS_LIKELY(expression_) __builtin_expect(!!(expression_), 1)
# define HPS_UNLIKELY(expression_) __builtin_expect(!!(expression_), 0)
#else
# define HPS_LIKELY(expression_) (!!(expression_))
# define HPS_UNLIKELY(expression_) (!!(expression_))
#endif

/* The resulting object's lifetime follows C compound-literal rules. Its address
 * must not escape the enclosing block; this macro is unavailable in C89/C++. */
#if HPS_C_IS_C99
# define HPS_LVALUE(type_, ...) ((type_){ __VA_ARGS__ })
#endif

#endif /* HPS_BASE_HPS_EXPRESSION_H */
