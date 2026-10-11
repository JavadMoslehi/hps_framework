#ifndef HPS_TESTS_TEST_SUITES_H
#define HPS_TESTS_TEST_SUITES_H

/**
 * @file tests/test_suites.h
 * @brief Internal module test entry points for the shared hosted runner.
 */

/* Each suite runs once per process; zero means success, nonzero means failure.
 * Assertion failures terminate the process. The pool suite requires the initial
 * uninitialized pool state. The requested int signatures have a proposed policy
 * exception in governance/reviews/test-entrypoints-2026-10-11.md.
 */
int test_base(void);
int test_pool(void);
int test_bytes(void);

#endif /* HPS_TESTS_TEST_SUITES_H */
