
#include <hps/base/static_assert.h>

/*
 * Intentionally false assertion.
 * This file must fail to compile.
 */

HPS_STATIC_ASSERT(sizeof(char) == 2,
                  intentional_failure);

int main(void)
{
    return 0;
}
