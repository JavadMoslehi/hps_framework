
/*
 * HPS AddressSanitizer self-test.
 *
 * Intentionally accesses freed heap memory.
 * Must only run in the SANITIZED verification profile.
 *
 * Expected diagnostic:
 * AddressSanitizer: heap-use-after-free
 */

#include <stdlib.h>

static volatile unsigned char *saved_pointer;

static void save_pointer(volatile unsigned char *pointer)
{
    saved_pointer = pointer;
}

static void release_pointer(void *pointer)
{
    free(pointer);
}

int main(void)
{
    unsigned char *pointer;
    volatile unsigned char value;

    pointer = (unsigned char *)malloc(16U);

    if (pointer == NULL) {
        return 2;
    }

    pointer[0] = 42U;

    save_pointer(pointer);
    release_pointer(pointer);

    /*
     * Intentional use-after-free.
     * The volatile access prevents simple dead-code removal.
     */
    value = saved_pointer[0];
    (void)value;

    return 0;
}

