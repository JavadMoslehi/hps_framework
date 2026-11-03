
/*
 * HPS Language Standard Verification
 *
 * This test verifies that the compiler's reported C language
 * standard matches the standard requested by the HPS build.
 *
 * The source itself must remain compatible with ISO C90.
 */

#ifndef HPS_EXPECTED_C_STANDARD
# error "HPS: Expected C standard is not configured"
#endif

#if HPS_EXPECTED_C_STANDARD == 90

# if defined(__STDC_VERSION__)
#  error "HPS: Expected ISO C90"
# endif

#elif HPS_EXPECTED_C_STANDARD == 99

# if !defined(__STDC_VERSION__)
#  error "HPS: C99 version macro is missing"
# elif __STDC_VERSION__ != 199901L
#  error "HPS: Expected ISO C99"
# endif

#elif HPS_EXPECTED_C_STANDARD == 11

# if !defined(__STDC_VERSION__)
#  error "HPS: C11 version macro is missing"
# elif __STDC_VERSION__ != 201112L
#  error "HPS: Expected ISO C11"
# endif

#elif HPS_EXPECTED_C_STANDARD == 17

# if !defined(__STDC_VERSION__)
#  error "HPS: C17 version macro is missing"
# elif __STDC_VERSION__ != 201710L
#  error "HPS: Expected ISO C17"
# endif

#elif HPS_EXPECTED_C_STANDARD == 23

# if !defined(__STDC_VERSION__)
#  error "HPS: C23 version macro is missing"
# elif (__STDC_VERSION__ != 202000L) && \
(__STDC_VERSION__ != 202311L)
#  error "HPS: Unexpected C23/C2x language version"
# endif

#else

# error "HPS: Unsupported expected C standard"

#endif

int main(void)
{
    return 0;
}
