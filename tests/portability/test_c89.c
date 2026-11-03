#include <limits.h>
#include <stddef.h>

static int test_c89_environment(void)
{
    if (CHAR_BIT < 8) {
        return 1;
    }

    if (sizeof(char) != 1U) {
        return 2;
    }

    if (sizeof(int) < sizeof(short)) {
        return 3;
    }

    return 0;
}

int main(void)
{
    return test_c89_environment();
}

