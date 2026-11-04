
/*
 * HPS Language Abstraction Unit Test
 *
 * Verify language macros in a C translation unit.
 */

#include <hps/base/language.h>

/* ----------------------------------------------------------
 * 1. This test must be compiled as C.
 * ---------------------------------------------------------- */

#if HPS_LANG_C != 1
#error "HPS: Expected C language"
#endif

#if HPS_LANG_CPP != 0
#error "HPS: Unexpected C++ language"
#endif


/* ----------------------------------------------------------
 * 2. Verify the selected C standard.
 *
 * HPS_EXPECTED_C_STANDARD is supplied by CMake.
 * ---------------------------------------------------------- */

#ifndef HPS_EXPECTED_C_STANDARD
#error "HPS: Expected C standard is not configured"
#endif

#if HPS_C_STANDARD != HPS_EXPECTED_C_STANDARD
#error "HPS: Incorrect language standard classification"
#endif


/* ----------------------------------------------------------
 * 3. Verify draft C2x classification.
 * ---------------------------------------------------------- */

#if HPS_C23_DRAFT != 0 && HPS_C23_DRAFT != 1
#error "HPS: Invalid C23 draft classification"
#endif

#if HPS_C_STANDARD != 23 && HPS_C23_DRAFT != 0
#error "HPS: Draft marker requires C23 mode"
#endif


/* ----------------------------------------------------------
 * 4. Verify C linkage helpers.
 *
 * In C, these macros must expand to valid empty tokens.
 * ---------------------------------------------------------- */

HPS_BEGIN_DECLS

static int hps_test_language(void)
{
    return 0;
}

HPS_END_DECLS


int main(void)
{
    return hps_test_language();
}
