#include "test_suites.h"

/**
 * @file tests/main.c
 * @brief Run each module suite once and report the first failing suite.
 */

int main(void)
{
    if (test_base() != 0) {
        return 1;
    }
    if (test_pool() != 0) {
        return 2;
    }
    if (test_bytes() != 0) {
        return 3;
    }
    if (test_reader() != 0) {
        return 4;
    }
    return 0;
}
