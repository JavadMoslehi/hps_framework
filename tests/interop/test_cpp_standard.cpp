
/*
 * HPS C++ Standard Verification
 *
 * Checks the C++ language mode selected by CMake.
 *
 * Written using C++98-compatible syntax.
 */

#include <hps/base/language.h>


#include <hps/base/static_assert.h>

/*
 * Positive compile-time checks at file scope.
 */

HPS_STATIC_ASSERT(sizeof(char) == 1,
                  cpp_char_size);

HPS_STATIC_ASSERT(sizeof(short) <= sizeof(int),
                  cpp_short_int_order);



#ifndef HPS_EXPECTED_CXX_STANDARD
#error "HPS: Expected C++ standard is not configured"
#endif

#if !defined(__cplusplus)
#error "HPS: This file must be compiled as C++"
#endif

#if HPS_LANG_CPP != 1
#error "HPS: Incorrect C++ language detection"
#endif

#if HPS_LANG_C != 0
#error "HPS: Incorrect C language detection"
#endif

#if HPS_EXPECTED_CXX_STANDARD == 98

# if __cplusplus != 199711L
#  error "HPS: Expected C++98"
# endif

#elif HPS_EXPECTED_CXX_STANDARD == 11

# if __cplusplus != 201103L
#  error "HPS: Expected C++11"
# endif

#elif HPS_EXPECTED_CXX_STANDARD == 14

# if __cplusplus != 201402L
#  error "HPS: Expected C++14"
# endif

#elif HPS_EXPECTED_CXX_STANDARD == 17

# if __cplusplus != 201703L
#  error "HPS: Expected C++17"
# endif

#elif HPS_EXPECTED_CXX_STANDARD == 20

# if __cplusplus != 202002L
#  error "HPS: Expected C++20"
# endif

#elif HPS_EXPECTED_CXX_STANDARD == 23

/*
 * Permit final C++23 and known draft modes.
 * This does not establish complete C++23 conformance.
 */
# if __cplusplus < 202100L
#  error "HPS: Expected C++23 or C++2b"
# endif

#else

#error "HPS: Unsupported C++ standard"

#endif

int main()
{
    HPS_STATIC_ASSERT(sizeof(int) <= sizeof(long),
                  cpp_local_integer_order);
    
    return 0;
}

