
/*
 * HPS C++ interoperability verification.
 *
 * The program is compiled as C++ and calls a function
 * implemented in a separate C translation unit.
 */

#include <hps/base/language.h>
#include "c_api.h"

#if HPS_LANG_CPP != 1
#error "HPS: Expected C++ compilation"
#endif

#if HPS_LANG_C != 0
#error "HPS: Unexpected C language detection"
#endif

#if HPS_C_STANDARD != 0
#error "HPS: C standard must be zero in C++ mode"
#endif

int main() {
    const int result = hps_test_c_add(10, 20);

    if (result != 30) {
        return 1;
    }

    return 0;
}

