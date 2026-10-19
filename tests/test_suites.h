#ifndef HPS_TESTS_TEST_SUITES_H
#define HPS_TESTS_TEST_SUITES_H

/**
 * @file tests/test_suites.h
 * @brief Internal module test entry points for the shared hosted runner.
 */

#include <stdint.h>

/* Each suite runs once per process; zero means success, nonzero means failure.
 * Assertion failures terminate the process. The pool suite requires the initial
 * uninitialized pool state. The requested int signatures have a proposed policy
 * exception in governance/reviews/test-entrypoints-2026-10-11.md.
 */
int test_base(void);
int test_pool(void);
int test_bytes(void);
int32_t test_reader(void);
int32_t test_enum(void);
int32_t test_string(void);
int32_t test_string_extended(void);
int32_t test_string_builder(void);
int32_t test_span(void);
int32_t test_data_structure(void);
int32_t test_cstr(void);
int32_t test_cstr_convert(void);
int32_t test_cstr_format(void);
int32_t test_national_code(void);
int32_t test_cstr_enum(void);
int32_t test_memory_operations(void);
int32_t test_memory_clone(void);

#endif /* HPS_TESTS_TEST_SUITES_H */
