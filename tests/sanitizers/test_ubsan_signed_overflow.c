
/*
 * HPS UndefinedBehaviorSanitizer self-test.
 *
 * Intentionally causes signed integer overflow.
 * Must only run in the SANITIZED verification profile.
 *
 * Expected diagnostic:
 * runtime error: signed integer overflow
 */

#include <limits.h>

int main(void)
{
    volatile int left;
    volatile int right;
    volatile int result;

    left = INT_MAX;
    right = 1;

    /*
     * Intentional signed integer overflow.
     * Signed overflow is undefined behavior in C.
     */
    result = left + right;

    (void)result;

    return 0;
}
