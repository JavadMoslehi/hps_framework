
#ifndef HPS_BASE_STATIC_ASSERT_H
#define HPS_BASE_STATIC_ASSERT_H

/*
 * HPS Compile-Time Assertion
 *
 * Supports ISO C90 and later, and ISO C++98 and later.
 *
 * This macro can be used at file scope or block scope.
 *
 * Requirements:
 *   - condition must be an integer constant expression.
 *   - identifier must be unique in the current scope.
 *
 * No runtime code is generated.
 */

#define HPS_STATIC_ASSERT(condition, identifier) \
enum { hps_static_assert_##identifier = \
1 / (!!(condition)) \
}

#endif /* HPS_BASE_STATIC_ASSERT_H */
