#include "../test_suites.h"
#include "hps/memory/hps_allocated_array.h"
#include "hps/memory/hps_array.h"
#include "hps/memory/hps_operations.h"
#include "hps/memory/hps_owner.h"
#include <assert.h>

/**
 * @file tests/memory/test_memory_utilities.c
 * @brief Checked counts, zero-tail resize and explicit typed ownership.
 */

#ifdef NDEBUG
#error "Memory utility tests require enabled assertions"
#endif

#define TEST_UTIL_SLOT_COUNT 2U
#define TEST_UTIL_SLOT_BYTES 16U

/* Provider budget: two 16-byte payloads plus fixed metadata. Returned payloads
 * support bytes only; the independent owner fixture below uses actual uint32_t
 * storage instead of asserting that byte storage can hold arbitrary objects. */
typedef struct {
    uint8_t slots[TEST_UTIL_SLOT_COUNT][TEST_UTIL_SLOT_BYTES];
    size_t sizes[TEST_UTIL_SLOT_COUNT];
    uint8_t used[TEST_UTIL_SLOT_COUNT];
    uint8_t fail_resize;
    size_t allocations;
    size_t zeroed_allocations;
    size_t resizes;
    size_t releases;
} test_util_provider;

static size_t test_util_find(const test_util_provider *provider,
                             const void *pointer)
{
    size_t slot = TEST_UTIL_SLOT_COUNT;
    for (size_t index = 0U; index < TEST_UTIL_SLOT_COUNT; index += 1U) {
        if (pointer == provider->slots[index]) {
            slot = index;
        }
    }
    return slot;
}

static uint8_t *test_util_reserve(test_util_provider *provider, size_t size)
{
    uint8_t *pointer = NULL;
    if ((size > 0U) && (size <= TEST_UTIL_SLOT_BYTES)) {
        for (size_t slot = 0U; slot < TEST_UTIL_SLOT_COUNT; slot += 1U) {
            if ((provider->used[slot] == 0U) && (pointer == NULL)) {
                provider->used[slot] = 1U;
                provider->sizes[slot] = size;
                for (size_t byte = 0U; byte < TEST_UTIL_SLOT_BYTES; byte += 1U) {
                    provider->slots[slot][byte] = 173U;
                }
                pointer = provider->slots[slot];
            }
        }
    }
    return pointer;
}

static void *test_util_allocate(void *context, size_t size, size_t alignment)
{
    test_util_provider *provider = context;
    assert(alignment == HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
    provider->allocations += 1U;
    return test_util_reserve(provider, size);
}

static void *test_util_zeroed(void *context, size_t count, size_t size,
                              size_t alignment)
{
    test_util_provider *provider = context;
    assert(alignment == HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
    assert((size != 0U) && (count <= (SIZE_MAX / size)));
    provider->zeroed_allocations += 1U;
    uint8_t *pointer = test_util_reserve(provider, count * size);
    if (pointer != NULL) {
        for (size_t index = 0U; index < (count * size); index += 1U) {
            pointer[index] = 0U;
        }
    }
    return pointer;
}

static void test_util_release(void *context, void *pointer, size_t size)
{
    test_util_provider *provider = context;
    const size_t slot = test_util_find(provider, pointer);
    assert(slot < TEST_UTIL_SLOT_COUNT);
    assert(provider->used[slot] == 1U);
    assert(provider->sizes[slot] == size);
    provider->used[slot] = 0U;
    provider->releases += 1U;
}

static void *test_util_resize(void *context, void *pointer, size_t old_size,
                              size_t new_size, size_t alignment)
{
    test_util_provider *provider = context;
    const size_t old_slot = test_util_find(provider, pointer);
    assert(alignment == HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
    assert(old_slot < TEST_UTIL_SLOT_COUNT);
    assert(provider->used[old_slot] == 1U);
    assert(provider->sizes[old_slot] == old_size);
    provider->resizes += 1U;
    uint8_t *replacement = NULL;
    if (provider->fail_resize == 0U) {
        replacement = test_util_reserve(provider, new_size);
        if (replacement != NULL) {
            size_t copied = old_size;
            if (new_size < copied) {
                copied = new_size;
            }
            for (size_t index = 0U; index < copied; index += 1U) {
                replacement[index] = provider->slots[old_slot][index];
            }
            provider->used[old_slot] = 0U;
        }
    }
    return replacement;
}

static hps_allocator_t test_util_allocator(test_util_provider *provider)
{
    const hps_allocator_t allocator = {
        test_util_allocate, NULL, test_util_resize, test_util_release, provider
    };
    return allocator;
}

static void test_swap_and_full_scan(void)
{
    uint8_t left[HPS_MEMSWAP_SCRATCH_BYTES + 4U];
    uint8_t right[HPS_MEMSWAP_SCRATCH_BYTES + 4U];
    for (size_t index = 0U; index < sizeof(left); index += 1U) {
        left[index] = 17U;
        right[index] = 231U;
    }
    hps_memswap(&left[1], &right[1], sizeof(left) - 2U);
    assert(left[0] == 17U);
    assert(right[0] == 231U);
    assert(left[sizeof(left) - 1U] == 17U);
    assert(right[sizeof(right) - 1U] == 231U);
    for (size_t index = 1U; index < (sizeof(left) - 1U); index += 1U) {
        assert(left[index] == 231U);
        assert(right[index] == 17U);
    }
    hps_memswap(left, left, sizeof(left));
    hps_memswap(NULL, NULL, 0U);
    assert(left[1] == 231U);
    assert(hps_pointer_is_nonnull(NULL) == 0U);
    assert(hps_pointer_is_nonnull(left) == 1U);
    assert(hps_memequal_full_scan(NULL, NULL, 0U) == 1U);
    assert(hps_memequal_full_scan(left, left, sizeof(left)) == 1U);
    const uint8_t source[] = {1U, 2U, 3U, 4U};
    uint8_t comparison[] = {1U, 2U, 3U, 4U};
    assert(hps_memequal_full_scan(source, comparison, sizeof(source)) == 1U);
    comparison[0] = 255U;
    assert(hps_memequal_full_scan(source, comparison, sizeof(source)) == 0U);
    comparison[0] = 1U;
    comparison[2] = 255U;
    assert(hps_memequal_full_scan(source, comparison, sizeof(source)) == 0U);
    comparison[2] = 3U;
    comparison[3] = 255U;
    assert(hps_memequal_full_scan(source, comparison, sizeof(source)) == 0U);
}

static void test_checked_arrays(void)
{
    size_t size = 39U;
    assert(hps_memory_array_size(SIZE_MAX, 2U, &size) ==
           HPS_MEMORY_ARRAY_SIZE_OVERFLOW);
    assert(size == 39U);
    assert(hps_memory_array_size(1U, 0U, &size) ==
           HPS_MEMORY_ARRAY_INVALID_ARGUMENT);
    assert(size == 39U);
    assert(hps_memory_array_size(0U, 0U, &size) == HPS_MEMORY_ARRAY_OK);
    assert(size == 0U);
    assert(hps_memory_array_size(SIZE_MAX, 1U, &size) == HPS_MEMORY_ARRAY_OK);
    assert(size == SIZE_MAX);
    assert(hps_memory_array_size(0U, 1U, NULL) ==
           HPS_MEMORY_ARRAY_INVALID_ARGUMENT);
    const uint8_t source[] = {10U, 20U, 30U, 40U};
    uint8_t destination[] = {91U, 1U, 2U, 3U, 4U, 92U};
    assert(hps_memory_array_copy(&destination[1], source, 2U, 2U) ==
           HPS_MEMORY_ARRAY_OK);
    for (size_t index = 0U; index < sizeof(source); index += 1U) {
        assert(destination[index + 1U] == source[index]);
    }
    assert(destination[0] == 91U);
    assert(destination[5] == 92U);
    int32_t comparison = INT32_C(73);
    assert(hps_memory_array_compare(source, &destination[1], 2U, 2U,
                                     &comparison) == HPS_MEMORY_ARRAY_OK);
    assert(comparison == INT32_C(0));
    destination[2] = 255U;
    assert(hps_memory_array_compare(source, &destination[1], 2U, 2U,
                                     &comparison) == HPS_MEMORY_ARRAY_OK);
    assert(comparison == -INT32_C(235));
    comparison = INT32_C(73);
    assert(hps_memory_array_compare(source, destination, SIZE_MAX, 2U,
                                     &comparison) ==
           HPS_MEMORY_ARRAY_SIZE_OVERFLOW);
    assert(comparison == INT32_C(73));
    assert(hps_memory_array_compare(NULL, source, 1U, 1U, &comparison) ==
           HPS_MEMORY_ARRAY_INVALID_ARGUMENT);
    assert(comparison == INT32_C(73));
    assert(hps_memory_array_copy(destination, source, SIZE_MAX, 2U) ==
           HPS_MEMORY_ARRAY_SIZE_OVERFLOW);
    assert(destination[0] == 91U);
    assert(destination[2] == 255U);
    assert(hps_memory_array_clear(destination, SIZE_MAX, 2U) ==
           HPS_MEMORY_ARRAY_SIZE_OVERFLOW);
    assert(destination[0] == 91U);
    assert(hps_memory_array_move(destination, source, SIZE_MAX, 2U) ==
           HPS_MEMORY_ARRAY_SIZE_OVERFLOW);
    assert(destination[0] == 91U);
    assert(hps_memory_array_copy(NULL, source, 1U, 1U) ==
           HPS_MEMORY_ARRAY_INVALID_ARGUMENT);
    assert(hps_memory_array_clear(NULL, 1U, 1U) ==
           HPS_MEMORY_ARRAY_INVALID_ARGUMENT);
    assert(hps_memory_array_move(destination, NULL, 1U, 1U) ==
           HPS_MEMORY_ARRAY_INVALID_ARGUMENT);
    assert(hps_memory_array_copy(NULL, NULL, 0U, 0U) == HPS_MEMORY_ARRAY_OK);
    assert(hps_memory_array_move(NULL, NULL, 0U, 0U) == HPS_MEMORY_ARRAY_OK);
    assert(hps_memory_array_clear(NULL, 0U, 0U) == HPS_MEMORY_ARRAY_OK);
    assert(hps_memory_array_compare(NULL, NULL, 0U, 0U, &comparison) ==
           HPS_MEMORY_ARRAY_OK);
    assert(comparison == INT32_C(0));
    assert(hps_memory_array_compare(NULL, NULL, 0U, 0U, NULL) ==
           HPS_MEMORY_ARRAY_INVALID_ARGUMENT);
    uint8_t overlapping[] = {1U, 2U, 3U, 4U, 5U, 6U};
    assert(hps_memory_array_move(&overlapping[2], overlapping, 2U, 2U) ==
           HPS_MEMORY_ARRAY_OK);
    const uint8_t expected[] = {1U, 2U, 1U, 2U, 3U, 4U};
    for (size_t index = 0U; index < sizeof(expected); index += 1U) {
        assert(overlapping[index] == expected[index]);
    }
    assert(hps_memory_array_clear(&destination[1], 2U, 2U) ==
           HPS_MEMORY_ARRAY_OK);
    for (size_t index = 1U; index < 5U; index += 1U) {
        assert(destination[index] == 0U);
    }
    assert(destination[0] == 91U);
    assert(destination[5] == 92U);
}

static void test_allocated_array_resize(void)
{
    test_util_provider provider = {0U};
    hps_allocator_t allocator = test_util_allocator(&provider);
    uint8_t *pointer = hps_allocated_array_create_zeroed(&allocator, 2U, 2U);
    assert(pointer == provider.slots[0]);
    for (size_t index = 0U; index < 4U; index += 1U) {
        assert(pointer[index] == 0U);
        pointer[index] = (uint8_t)(index + 10U);
    }
    assert(pointer[4] == 173U);
    void *replacement = hps_allocated_array_resize_zeroed(
        &allocator, pointer, 2U, 4U, 2U);
    assert(replacement == provider.slots[1]);
    assert(provider.used[0] == 0U);
    pointer = replacement;
    for (size_t index = 0U; index < 4U; index += 1U) {
        assert(pointer[index] == (index + 10U));
        assert(pointer[index + 4U] == 0U);
    }
    assert(pointer[8] == 173U);
    provider.fail_resize = 1U;
    replacement = hps_allocated_array_resize_zeroed(&allocator, pointer,
                                                    4U, 5U, 2U);
    assert(replacement == NULL);
    assert(provider.used[1] == 1U);
    assert(provider.sizes[1] == 8U);
    assert(pointer[0] == 10U);
    const size_t resize_calls = provider.resizes;
    replacement = hps_allocated_array_resize_zeroed(&allocator, pointer,
                                                    4U, 0U, 2U);
    assert(replacement == NULL);
    replacement = hps_allocated_array_resize_zeroed(&allocator, pointer,
                                                    SIZE_MAX, 1U, 2U);
    assert(replacement == NULL);
    replacement = hps_allocated_array_resize_zeroed(&allocator, pointer,
                                                    4U, SIZE_MAX, 2U);
    assert(replacement == NULL);
    replacement = hps_allocated_array_resize_zeroed(&allocator, pointer,
                                                    0U, 1U, 2U);
    assert(replacement == NULL);
    replacement = hps_allocated_array_resize_zeroed(&allocator, NULL,
                                                    1U, 1U, 2U);
    assert(replacement == NULL);
    allocator.resize = NULL;
    replacement = hps_allocated_array_resize_zeroed(&allocator, pointer,
                                                    4U, 1U, 2U);
    assert(replacement == NULL);
    assert(provider.resizes == resize_calls);
    assert(provider.releases == 0U);
    assert(provider.used[1] == 1U);
    allocator.resize = test_util_resize;
    provider.fail_resize = 0U;
    replacement = hps_allocated_array_resize_zeroed(&allocator, pointer,
                                                    4U, 1U, 2U);
    assert(replacement == provider.slots[0]);
    pointer = replacement;
    assert(pointer[0] == 10U);
    assert(pointer[1] == 11U);
    assert(pointer[2] == 173U);
    hps_allocator_free(&allocator, pointer, 2U);
    assert(provider.releases == 1U);
    pointer = hps_allocated_array_resize_zeroed(&allocator, NULL, 0U, 3U, 1U);
    assert(pointer != NULL);
    for (size_t index = 0U; index < 3U; index += 1U) {
        assert(pointer[index] == 0U);
    }
    hps_allocator_free(&allocator, pointer, 3U);
}

static void test_allocated_array_duplicate_and_failures(void)
{
    test_util_provider provider = {0U};
    hps_allocator_t allocator = test_util_allocator(&provider);
    const uint8_t source[] = {11U, 22U, 33U, 44U};
    uint8_t *pointer = hps_allocated_array_duplicate(&allocator, source, 2U, 2U);
    assert(pointer != NULL);
    for (size_t index = 0U; index < sizeof(source); index += 1U) {
        assert(pointer[index] == source[index]);
    }
    pointer[0] = 77U;
    assert(source[0] == 11U);
    hps_allocator_free(&allocator, pointer, sizeof(source));
    const size_t allocation_calls = provider.allocations;
    assert(hps_allocated_array_create_zeroed(&allocator, SIZE_MAX, 2U) == NULL);
    assert(hps_allocated_array_create_zeroed(&allocator, 0U, 1U) == NULL);
    assert(hps_allocated_array_create_zeroed(&allocator, 1U, 0U) == NULL);
    assert(hps_allocated_array_create_zeroed(NULL, 1U, 1U) == NULL);
    assert(hps_allocated_array_duplicate(&allocator, NULL, 1U, 1U) == NULL);
    assert(hps_allocated_array_duplicate(&allocator, source, SIZE_MAX, 2U) ==
           NULL);
    allocator.release = NULL;
    assert(hps_allocated_array_create_zeroed(&allocator, 1U, 1U) == NULL);
    assert(hps_allocated_array_duplicate(&allocator, source, 1U, 1U) == NULL);
    assert(hps_allocated_array_resize_zeroed(&allocator, NULL, 0U, 1U, 1U) ==
           NULL);
    assert(provider.allocations == allocation_calls);
    allocator.release = test_util_release;
    allocator.allocate = NULL;
    allocator.allocate_zeroed = test_util_zeroed;
    pointer = hps_allocated_array_duplicate(&allocator, source, 2U, 2U);
    assert(pointer != NULL);
    assert(provider.zeroed_allocations == 1U);
    for (size_t index = 0U; index < sizeof(source); index += 1U) {
        assert(pointer[index] == source[index]);
    }
    hps_allocator_free(&allocator, pointer, sizeof(source));
    allocator.allocate_zeroed = NULL;
    assert(hps_allocated_array_create_zeroed(&allocator, 1U, 1U) == NULL);
}

typedef struct {
    uint32_t values[4];
    uint8_t used;
    size_t releases;
} test_util_typed_provider;

HPS_MEMORY_OWNER_DEFINE(hps_test_word_owner, uint32_t)

static void test_util_release_words(void *context, void *pointer, size_t size)
{
    test_util_typed_provider *provider = context;
    assert(pointer == provider->values);
    assert(size == sizeof(provider->values));
    assert(provider->used == 1U);
    provider->used = 0U;
    provider->releases += 1U;
}

static void test_typed_owner(void)
{
    test_util_typed_provider provider = {{1U, 2U, 3U, 4U}, 1U, 0U};
    hps_allocator_t allocator = {NULL, NULL, NULL,
                                test_util_release_words, &provider};
    hps_test_word_owner owner;
    hps_test_word_owner_init(&owner);
    hps_test_word_owner_init(NULL);
    assert(owner.pointer == NULL);
    assert(hps_test_word_owner_reset(&owner) == HPS_MEMORY_OWNER_OK);
    assert(hps_test_word_owner_reset(NULL) ==
           HPS_MEMORY_OWNER_INVALID_ARGUMENT);
    assert(hps_test_word_owner_adopt(&owner, NULL, 4U, &allocator) ==
           HPS_MEMORY_OWNER_INVALID_ARGUMENT);
    assert(hps_test_word_owner_adopt(&owner, provider.values, 0U, &allocator) ==
           HPS_MEMORY_OWNER_INVALID_ARGUMENT);
    assert(hps_test_word_owner_adopt(&owner, provider.values, 4U, NULL) ==
           HPS_MEMORY_OWNER_INVALID_ARGUMENT);
    assert(hps_test_word_owner_adopt(NULL, provider.values, 4U, &allocator) ==
           HPS_MEMORY_OWNER_INVALID_ARGUMENT);
    assert(hps_test_word_owner_adopt(&owner, provider.values, SIZE_MAX,
                                     &allocator) ==
           HPS_MEMORY_OWNER_SIZE_OVERFLOW);
    assert(owner.pointer == NULL);
    allocator.release = NULL;
    assert(hps_test_word_owner_adopt(&owner, provider.values, 4U, &allocator) ==
           HPS_MEMORY_OWNER_RELEASE_UNAVAILABLE);
    allocator.release = test_util_release_words;
    assert(hps_test_word_owner_adopt(&owner, provider.values, 4U, &allocator) ==
           HPS_MEMORY_OWNER_OK);
    assert(owner.pointer == provider.values);
    owner.pointer[1] = UINT32_C(71);
    assert(provider.values[1] == UINT32_C(71));
    assert(hps_test_word_owner_adopt(&owner, provider.values, 4U, &allocator) ==
           HPS_MEMORY_OWNER_NOT_EMPTY);
    size_t count = 37U;
    const hps_allocator_t *transferred_allocator = NULL;
    uint32_t *transferred = hps_test_word_owner_release(&owner, NULL,
                                                       &transferred_allocator);
    assert(transferred == NULL);
    assert(owner.pointer == provider.values);
    assert(count == 37U);
    transferred = hps_test_word_owner_release(&owner, &count,
                                               &transferred_allocator);
    assert(transferred == provider.values);
    assert(count == 4U);
    assert(transferred_allocator == &allocator);
    assert(owner.pointer == NULL);
    assert(provider.releases == 0U);
    assert(hps_test_word_owner_adopt(&owner, transferred, count,
                                     transferred_allocator) ==
           HPS_MEMORY_OWNER_OK);
    /* A provider table that becomes incomplete cannot silently lose its owner. */
    allocator.release = NULL;
    assert(hps_test_word_owner_reset(&owner) ==
           HPS_MEMORY_OWNER_RELEASE_UNAVAILABLE);
    assert(owner.pointer == provider.values);
    assert(provider.used == 1U);
    allocator.release = test_util_release_words;
    assert(hps_test_word_owner_reset(&owner) == HPS_MEMORY_OWNER_OK);
    assert(owner.pointer == NULL);
    assert(owner.count == 0U);
    assert(owner.allocator == NULL);
    assert(provider.used == 0U);
    assert(provider.releases == 1U);
    assert(hps_test_word_owner_reset(&owner) == HPS_MEMORY_OWNER_OK);
    assert(provider.releases == 1U);
    transferred = hps_test_word_owner_release(&owner, &count,
                                               &transferred_allocator);
    assert(transferred == NULL);
    assert(count == 0U);
    assert(transferred_allocator == NULL);
}

int32_t test_memory_utilities(void)
{
    test_swap_and_full_scan();
    test_checked_arrays();
    test_allocated_array_resize();
    test_allocated_array_duplicate_and_failures();
    test_typed_owner();
    return INT32_C(0);
}
