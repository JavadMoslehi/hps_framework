#ifndef HPS_PLATFORM_HOSTED_DEBUG_H
#define HPS_PLATFORM_HOSTED_DEBUG_H

/**
 * @file hps/platform/hosted/debug.h
 * @brief Optional hosted stderr diagnostics, assertions and process termination.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/base/attributes.h"

/* Explicit hosted adapter, excluded from Base/platform metadata and core
 * libraries. Link the separately enabled hosted_debug library when used.
 * Each non-NULL text pointer borrows a live C string; length excludes its NUL
 * and selects at most that string's readable characters. Exactly length bytes
 * are emitted without scanning for NUL. NULL emits "?" regardless of length.
 * Strings may alias each other. No pointer or result is retained or mutated.
 * Callers bound text lengths and exclude sensitive data. Diagnostic writes are
 * best effort: write/flush failures do not prevent termination. libc I/O may
 * block, allocate internally or interleave with other writers; no core no-heap,
 * signal/ISR safety, atomic-output or timing guarantee is made for this adapter.
 * There are no application buffers or loops; work is O(sum of text lengths)
 * plus the hosted stdio/termination implementation's unqualified resource cost.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Emit "PANIC: <message>\n", flush stderr, then abort unconditionally. */
HPS_NORETURN void hps_panic(const char *message, size_t length);

/* Emit the assertion expression, source file and decimal line, then abort.
 * line is a caller-supplied source location, with no pointer/ownership effect.
 */
HPS_NORETURN void hps_debug_assert_fail(const char *expression,
                                      size_t expression_length,
                                      const char *file, size_t file_length,
                                      uint32_t line);

/* Zero is a no-op, including no text access. For nonzero, emit
 * "[ERROR] (<context>): <message> [result=<decimal>]\n" and flush stderr.
 * The caller supplies the originating API's message; no global domain, category
 * or diagnostic lookup interprets the bare number. Positive values are reported
 * as non-success but are outside HPS module negative-error contracts.
 */
void hps_result_report(int32_t result, const char *context,
                       size_t context_length, const char *message,
                       size_t message_length);

/* Zero is a no-op. Otherwise perform the same report and abort, even if I/O
 * fails. Unlike hps_panic(), this emits only the error report above.
 */
void hps_panic_on_result(int32_t result, const char *context,
                         size_t context_length, const char *message,
                         size_t message_length);

#ifdef __cplusplus
}
#endif

/* Scalar expressions only; evaluate once when enabled. Under NDEBUG the
 * expression is type-checked through sizeof and is not evaluated. Variably
 * modified operands are excluded. Source line values must fit uint32_t; the
 * active debug macro checks that bound before the narrowing cast. */
#if !defined(NDEBUG)
# define HPS_DEBUG_ASSERT(expression_) \
    do { \
        (void)sizeof(uint8_t[(__LINE__ <= UINT32_MAX) ? 1 : -1]); \
        if (!(expression_)) { \
            hps_debug_assert_fail(#expression_, sizeof(#expression_) - 1U, \
                                  __FILE__, sizeof(__FILE__) - 1U, \
                                  (uint32_t)__LINE__); \
        } \
    } while (0)
#else
# define HPS_DEBUG_ASSERT(expression_) \
    do { (void)sizeof(expression_); } while (0)
#endif

/* Always active, with exactly one runtime evaluation of the expression. */
#define HPS_ASSERT_OR_PANIC(expression_) \
    do { \
        if (!(expression_)) { \
            hps_panic("Assertion failed: " #expression_, \
                      sizeof("Assertion failed: " #expression_) - 1U); \
        } \
    } while (0)

#endif /* HPS_PLATFORM_HOSTED_DEBUG_H */
