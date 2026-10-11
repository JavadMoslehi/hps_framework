#include "hps/base.h"
#include "hps/base.h"

#include <limits.h>
#include <stddef.h>
#include <string.h>

HPS_STATIC_ASSERT(HPS_C_IS_C99, "base requires C99");
HPS_STATIC_ASSERT(HPS_VERSION_AT_LEAST(0, 1, 0), "version floor");
HPS_STATIC_ASSERT(!HPS_VERSION_AT_LEAST(0, 1, 1), "version patch");
HPS_STATIC_ASSERT(!HPS_VERSION_AT_LEAST(0, 2, 0), "version minor");
HPS_STATIC_ASSERT(!HPS_VERSION_AT_LEAST(1, 0, 0), "version major");
HPS_STATIC_ASSERT(sizeof(hps_size) == sizeof(size_t), "size type");
HPS_STATIC_ASSERT(sizeof(hps_ssize) == sizeof(ptrdiff_t), "difference type");
#if HPS_HAS_INT8
HPS_STATIC_ASSERT(sizeof(hps_u8) * CHAR_BIT == 8, "8-bit integer");
HPS_STATIC_ASSERT(sizeof(hps_i8) * CHAR_BIT == 8, "signed 8-bit integer");
#endif
#if HPS_HAS_INT16
HPS_STATIC_ASSERT(sizeof(hps_u16) * CHAR_BIT == 16, "16-bit integer");
#endif
#if HPS_HAS_INT32
HPS_STATIC_ASSERT(sizeof(hps_u32) * CHAR_BIT == 32, "32-bit integer");
#endif
#if HPS_HAS_INT64
HPS_STATIC_ASSERT(sizeof(hps_u64) * CHAR_BIT == 64, "64-bit integer");
#endif
#if HPS_POINTER_WIDTH_IS_KNOWN
HPS_STATIC_ASSERT(sizeof(void *) * CHAR_BIT == HPS_POINTER_BITS, "pointer bits");
#endif

static HPS_INLINE int increment(int value)
{
    return value + 1;
}

HPS_NODISCARD static int answer(void)
{
    return 42;
}

#if HPS_HAS_ALIGNAS && HPS_HAS_ALIGNOF
HPS_ALIGNAS(16) static unsigned char aligned_storage[16];
HPS_STATIC_ASSERT(HPS_ALIGNOF(int) > 0, "alignment");
#endif
#if HPS_HAS_THREAD_LOCAL
static HPS_THREAD_LOCAL int thread_value;
#endif

int main(void)
{
    hps_bool enabled = HPS_TRUE;
    void *pointer = HPS_NULL;
    if (!enabled || HPS_FALSE || pointer != NULL || increment(41) != answer()) {
        return 1;
    }
    if (strcmp(HPS_VERSION_STRING, "0.1.0") != 0) {
        return 2;
    }
#if HPS_HAS_INT32 && CHAR_BIT == 8
    {
        hps_u32 word = UINT32_C(0x01020304);
        hps_byte bytes[sizeof(word)];
        memcpy(bytes, &word, sizeof(word));
        if ((HPS_IS_LITTLE_ENDIAN && bytes[0] != 4) ||
            (HPS_IS_BIG_ENDIAN && bytes[0] != 1)) {
            return 3;
        }
    }
#endif
#if HPS_HAS_INTPTR
    {
        int value = 7;
        hps_uptr bits = (hps_uptr)(void *)&value;
        if ((void *)bits != (void *)&value) {
            return 4;
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
