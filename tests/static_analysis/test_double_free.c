
/*
 * HPS Static Analyzer Self-Test
 *
 * Intentional defect:
 * Double Free
 *
 * Expected:
 * Both GCC and Clang static analyzers must report it.
 *
 * This file must never become part of the HPS library.
 */

#include <stdlib.h>

int main(void)
{
    void *pointer;

    pointer = malloc(32U);

    if (pointer == NULL) {
        return 1;
    }

    free(pointer);

    /*
     * Intentional error:
     * The same allocation is released twice.
     */
    free(pointer);

    return 0;
}
