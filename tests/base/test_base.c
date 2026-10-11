#include "../test_suites.h"
#include "hps/base.h"
#include "hps/base.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

HPS_STATIC_ASSERT(HPS_C_IS_C99, "base requires C99");
HPS_STATIC_ASSERT(HPS_VERSION_AT_LEAST(0, 1, 0), "version floor");
HPS_STATIC_ASSERT(!HPS_VERSION_AT_LEAST(0, 1, 1), "version patch");
HPS_STATIC_ASSERT(!HPS_VERSION_AT_LEAST(0, 2, 0), "version minor");
HPS_STATIC_ASSERT(!HPS_VERSION_AT_LEAST(1, 0, 0), "version major");
#if HPS_POINTER_WIDTH_IS_KNOWN
HPS_STATIC_ASSERT(sizeof(void *) * CHAR_BIT == HPS_POINTER_BITS, "pointer bits");
#endif

static HPS_INLINE int32_t increment(int32_t value)
{
    return (value < INT32_MAX) ? (value + INT32_C(1)) : INT32_MAX;
}

HPS_NODISCARD static int32_t answer(void)
{
    return 42;
}

#if HPS_HAS_ALIGNAS && HPS_HAS_ALIGNOF
HPS_ALIGNAS(16) static uint8_t aligned_storage[16];
HPS_STATIC_ASSERT(HPS_ALIGNOF(int32_t) > 0, "alignment");
#endif
#if HPS_HAS_THREAD_LOCAL
static HPS_THREAD_LOCAL int32_t thread_value;
#endif

int test_base(void)
{
    if (increment(41) != answer()) {
        return 1;
    }
    if (strcmp(HPS_VERSION_STRING, "0.1.0") != 0) {
        return 2;
    }
#if defined(UINT32_MAX) && CHAR_BIT == 8
    {
        uint32_t word = UINT32_C(0x01020304);
        uint8_t bytes[sizeof(word)];
        memcpy(bytes, &word, sizeof(word));
        if ((HPS_IS_LITTLE_ENDIAN && bytes[0] != 4) ||
            (HPS_IS_BIG_ENDIAN && bytes[0] != 1)) {
            return 3;
        }
    }
#endif
#if HPS_HAS_ALIGNAS && HPS_HAS_ALIGNOF
    HPS_UNUSED(aligned_storage);
#endif
#if HPS_HAS_THREAD_LOCAL
    thread_value = 9;
    if (thread_value != 9) {
        return 5;
    }
#endif
    return 0;
}
