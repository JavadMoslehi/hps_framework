
/*
 * HPS Static Assertion Unit Test
 *
 * All checks are evaluated at compile time.
 */

#include <limits.h>
#include <hps/base/static_assert.h>

/* File-scope assertions. */

HPS_STATIC_ASSERT(CHAR_BIT >= 8, char_at_least_8_bits);

HPS_STATIC_ASSERT(sizeof(char) == 1, char_size_is_one);

HPS_STATIC_ASSERT(sizeof(short) <= sizeof(int),
                  short_not_larger_than_int);

HPS_STATIC_ASSERT(sizeof(int) <= sizeof(long),
                  int_not_larger_than_long);


static int hps_test_local_assertion(void)
{
    /* Block-scope assertion. */
    HPS_STATIC_ASSERT(sizeof(long) >= sizeof(int),
                      local_long_size);

    return 0;
}


int main(void)
{
    return hps_test_local_assertion();
}
