
/*
 * HPS Standard Type Compatibility Test
 *
 * Written using C90-compatible syntax.
 */

#include <hps/base/types.h>
#include <hps/base/static_assert.h>

/* Verify required integer type sizes. */

HPS_STATIC_ASSERT(sizeof(uint8_t) * CHAR_BIT == 8,
                  uint8_width);

HPS_STATIC_ASSERT(sizeof(int8_t) * CHAR_BIT == 8,
                  int8_width);

HPS_STATIC_ASSERT(sizeof(uint16_t) * CHAR_BIT == 16,
                  uint16_width);

HPS_STATIC_ASSERT(sizeof(int16_t) * CHAR_BIT == 16,
                  int16_width);

HPS_STATIC_ASSERT(sizeof(uint32_t) * CHAR_BIT == 32,
                  uint32_width);

HPS_STATIC_ASSERT(sizeof(int32_t) * CHAR_BIT == 32,
                  int32_width);


static int test_integer_values(void)
{
    uint8_t a;
    uint16_t b;
    uint32_t c;

    a = 255U;
    b = 65535U;
    c = 4294967295UL;

    if (a != 255U) {
        return 1;
    }

    if (b != 65535U) {
        return 2;
    }

    if (c != 4294967295UL) {
        return 3;
    }

    return 0;
}


static int test_boolean_values(void)
{
    bool enabled;
    bool disabled;

    enabled = true;
    disabled = false;

    if (!enabled || disabled) {
        return 1;
    }

    return 0;
}


int main(void)
{
    if (test_integer_values() != 0) {
        return 1;
    }

    if (test_boolean_values() != 0) {
        return 2;
    }

    return 0;
}
