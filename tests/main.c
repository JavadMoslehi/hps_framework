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
    if (test_enum() != 0) {
        return 5;
    }
    if (test_string() != 0) {
        return 6;
    }
    if (test_string_extended() != 0) {
        return 7;
    }
    if (test_string_builder() != 0) {
        return 8;
    }
    if (test_span() != 0) {
        return 9;
    }
    if (test_cstr() != 0) {
        return 10;
    }
    if (test_cstr_convert() != 0) {
        return 11;
    }
    if (test_cstr_format() != 0) {
        return 12;
    }
    if (test_national_code() != 0) {
        return 13;
    }
    if (test_cstr_enum() != 0) {
        return 14;
    }
    if (test_data_structure() != 0) {
        return 15;
    }
    if (test_memory_operations() != 0) {
        return 16;
    }
    if (test_memory_clone() != 0) {
        return 17;
    }
    return 0;
}
