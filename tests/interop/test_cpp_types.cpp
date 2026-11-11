
/*
 * HPS Type Compatibility Test for C++.
 *
 * Compatible with C++98 and later.
 */

#include <hps/base/types.h>
#include <hps/base/static_assert.h>

HPS_STATIC_ASSERT(sizeof(uint8_t) * CHAR_BIT == 8,
                  cpp_uint8_width);

HPS_STATIC_ASSERT(sizeof(uint16_t) * CHAR_BIT == 16,
                  cpp_uint16_width);

HPS_STATIC_ASSERT(sizeof(uint32_t) * CHAR_BIT == 32,
                  cpp_uint32_width);

int main()
{
    uint32_t value = 100U;
    bool enabled = true;

    if (value != 100U || !enabled) {
        return 1;
    }

    return 0;
}
