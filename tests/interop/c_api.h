
#ifndef HPS_TEST_INTEROP_C_API_H
#define HPS_TEST_INTEROP_C_API_H

/*
 * HPS C/C++ interoperability test API.
 *
 * This is a test-only interface.
 * It is not part of the public HPS library.
 */

#include <hps/base/language.h>

HPS_BEGIN_DECLS

int hps_test_c_add(int left, int right);

HPS_END_DECLS

#endif /* HPS_TEST_INTEROP_C_API_H */

