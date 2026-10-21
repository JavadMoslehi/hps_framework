#include "../test_suites.h"
#include "hps/memory/hps_allocator.h"
#include "hps/memory/hps_clone.h"
#include <assert.h>

/**
 * @file tests/memory/test_memory_clone.c
 * @brief Bounded allocator dispatch and typed clone ownership/failure contracts.
 */

#ifdef NDEBUG
#error "Memory clone tests require enabled assertions"
#endif

#define TEST_MEMORY_SLOTS 3U
#define TEST_MEMORY_SLOT_SIZE 8U

/* This caller-owned provider supports byte payloads only. Its complete budget
 * is three eight-byte slots plus this fixed metadata; it never uses the pool.
 * The alignment argument is recorded as a request hint, not a storage promise.
 */
typedef struct {
    uint8_t bytes[TEST_MEMORY_SLOTS][TEST_MEMORY_SLOT_SIZE];
    uint8_t used[TEST_MEMORY_SLOTS];
    size_t sizes[TEST_MEMORY_SLOTS];
    size_t capacity;
    size_t allocate_calls;
    size_t zeroed_calls;
    size_t resize_calls;
    size_t release_calls;
    size_t last_count;
    size_t last_size;
    size_t last_old_size;
    size_t last_new_size;
    size_t last_alignment;
    size_t release_order[TEST_MEMORY_SLOTS];
    size_t clone_calls;
    size_t destruct_calls;
    size_t fail_after_allocation;
    uint32_t destruct_order[TEST_MEMORY_SLOTS];
} test_memory_provider;

typedef struct {
    uint8_t *bytes;
    size_t size;
    uint32_t identifier;
} test_clone_element;

static size_t test_provider_find(const test_memory_provider *provider,
                                 const void *ptr)
{
    size_t result = TEST_MEMORY_SLOTS;
    for (size_t index = 0U; index < TEST_MEMORY_SLOTS; index += 1U) {
        if (ptr == provider->bytes[index]) {
            result = index;
        }
    }
    return result;
}

static uint8_t *test_provider_reserve(test_memory_provider *provider,
                                      const size_t size)
{
    uint8_t *result = NULL;
    assert(provider->capacity <= TEST_MEMORY_SLOTS);
    if ((size > 0U) && (size <= TEST_MEMORY_SLOT_SIZE)) {
        for (size_t index = 0U; index < provider->capacity; index += 1U) {
            if ((provider->used[index] == 0U) && (result == NULL)) {
                provider->used[index] = 1U;
                provider->sizes[index] = size;
                for (size_t byte = 0U; byte < TEST_MEMORY_SLOT_SIZE;
                     byte += 1U) {
                    provider->bytes[index][byte] = 173U;
                }
                result = provider->bytes[index];
            }
        }
    }
    return result;
}

static void *test_provider_allocate(void *ctx, size_t size, size_t alignment)
{
    test_memory_provider *provider = ctx;
    provider->allocate_calls += 1U;
    provider->last_size = size;
    provider->last_alignment = alignment;
    return test_provider_reserve(provider, size);
}

static void *test_provider_zeroed(void *ctx, size_t count, size_t size,
                                  size_t alignment)
{
    test_memory_provider *provider = ctx;
    provider->zeroed_calls += 1U;
    provider->last_count = count;
    provider->last_size = size;
    provider->last_alignment = alignment;
    assert((size == 0U) || (count <= (SIZE_MAX / size)));
    const size_t total = count * size;
    uint8_t *bytes = test_provider_reserve(provider, total);
    if (bytes != NULL) {
        for (size_t index = 0U; index < total; index += 1U) {
            bytes[index] = 0U;
        }
    }
    return bytes;
}

static void *test_provider_resize(void *ctx, void *ptr, size_t old_size,
                                  size_t new_size, size_t alignment)
{
    test_memory_provider *provider = ctx;
    const size_t slot = test_provider_find(provider, ptr);
    provider->resize_calls += 1U;
    provider->last_old_size = old_size;
    provider->last_new_size = new_size;
    provider->last_alignment = alignment;
    assert(slot < TEST_MEMORY_SLOTS);
    assert(provider->used[slot] == 1U);
    assert(provider->sizes[slot] == old_size);
    void *result = NULL;
    if ((new_size > 0U) && (new_size <= TEST_MEMORY_SLOT_SIZE)) {
        provider->sizes[slot] = new_size;
        result = ptr;
    }
    return result;
}

static void test_provider_release(void *ctx, void *ptr, size_t size)
{
    test_memory_provider *provider = ctx;
    const size_t slot = test_provider_find(provider, ptr);
    assert(slot < TEST_MEMORY_SLOTS);
    assert(provider->used[slot] == 1U);
    assert(provider->sizes[slot] == size);
    assert(provider->release_calls < TEST_MEMORY_SLOTS);
    provider->release_order[provider->release_calls] = slot;
    provider->release_calls += 1U;
    provider->used[slot] = 0U;
}

static hps_allocator_t test_provider_allocator(test_memory_provider *provider)
{
    const hps_allocator_t allocator = {
        test_provider_allocate, test_provider_zeroed, test_provider_resize,
        test_provider_release, provider
    };
    return allocator;
}

static void test_allocator_product(void)
{
    size_t product = 37U;
    uint8_t overflow = hps_allocator_check_mul_overflow(SIZE_MAX, 2U, &product);
    assert(overflow == 1U);
    assert(product == 37U);
    overflow = hps_allocator_check_mul_overflow(SIZE_MAX, 1U, &product);
    assert(overflow == 0U);
    assert(product == SIZE_MAX);
    overflow = hps_allocator_check_mul_overflow(0U, SIZE_MAX, &product);
    assert(overflow == 0U);
    assert(product == 0U);
    overflow = hps_allocator_check_mul_overflow(3U, 7U, &product);
    assert(overflow == 0U);
    assert(product == 21U);
    overflow = hps_allocator_check_mul_overflow(0U, 0U, NULL);
    assert(overflow == 1U);
}

static void test_allocator_dispatch(void)
{
    test_memory_provider provider = {0U};
    provider.capacity = TEST_MEMORY_SLOTS;
    const hps_allocator_t allocator = test_provider_allocator(&provider);
    uint8_t *owner = hps_allocator_alloc(&allocator, 4U);
    assert(owner == provider.bytes[0]);
    assert(provider.allocate_calls == 1U);
    assert(provider.zeroed_calls == 0U);
    assert(provider.last_size == 4U);
    assert(provider.last_alignment == HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
    owner[0] = 46U;
    void *replacement = hps_allocator_realloc(&allocator, owner, 4U,
                                              TEST_MEMORY_SLOT_SIZE + 1U);
    assert(replacement == NULL);
    assert(provider.resize_calls == 1U);
    assert(provider.last_old_size == 4U);
    assert(provider.last_new_size == TEST_MEMORY_SLOT_SIZE + 1U);
    assert(provider.last_alignment == HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
    assert(provider.used[0] == 1U);
    assert(owner[0] == 46U);
    replacement = hps_allocator_realloc(&allocator, owner, 4U, 6U);
    assert(replacement == owner);
    assert(owner[0] == 46U);
    hps_allocator_free(&allocator, owner, 6U);
    assert(provider.release_calls == 1U);
    assert(provider.used[0] == 0U);
    owner = hps_allocator_realloc(&allocator, NULL, 0U, 2U);
    assert(owner == provider.bytes[0]);
    assert(provider.allocate_calls == 2U);
    replacement = hps_allocator_realloc(&allocator, owner, 2U, 0U);
    assert(replacement == NULL);
    assert(provider.resize_calls == 2U);
    assert(provider.release_calls == 2U);
    assert(provider.used[0] == 0U);
    replacement = hps_allocator_realloc(&allocator, NULL, 0U, 0U);
    assert(replacement == NULL);
    assert(provider.allocate_calls == 3U);
    assert(provider.last_size == 0U);
}

static void test_allocator_zeroed(void)
{
    test_memory_provider provider = {0U};
    provider.capacity = TEST_MEMORY_SLOTS;
    hps_allocator_t allocator = test_provider_allocator(&provider);
    uint8_t *owner = hps_allocator_calloc(&allocator, 2U, 3U);
    assert(owner == provider.bytes[0]);
    assert(provider.allocate_calls == 0U);
    assert(provider.zeroed_calls == 1U);
    assert(provider.last_count == 2U);
    assert(provider.last_size == 3U);
    assert(provider.last_alignment == HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
    for (size_t index = 0U; index < 6U; index += 1U) {
        assert(owner[index] == 0U);
    }
    assert(owner[6] == 173U);
    hps_allocator_free(&allocator, owner, 6U);
    owner = hps_allocator_calloc(&allocator, SIZE_MAX, 2U);
    assert(owner == NULL);
    assert(provider.zeroed_calls == 1U);
    owner = hps_allocator_calloc(&allocator, 0U, SIZE_MAX);
    assert(owner == NULL);
    assert(provider.zeroed_calls == 2U);
    assert(provider.last_count == 0U);
    assert(provider.last_size == SIZE_MAX);
    allocator.allocate_zeroed = NULL;
    owner = hps_allocator_calloc(&allocator, 3U, 2U);
    assert(owner == provider.bytes[0]);
    assert(provider.allocate_calls == 1U);
    assert(provider.zeroed_calls == 2U);
    assert(provider.last_size == 6U);
    for (size_t index = 0U; index < 6U; index += 1U) {
        assert(owner[index] == 0U);
    }
    assert(owner[6] == 173U);
    hps_allocator_free(&allocator, owner, 6U);
    owner = hps_allocator_calloc(&allocator, SIZE_MAX, 2U);
    assert(owner == NULL);
    assert(provider.allocate_calls == 1U);
    provider.capacity = 0U;
    owner = hps_allocator_calloc(&allocator, 1U, 1U);
    assert(owner == NULL);
    assert(provider.allocate_calls == 2U);
}

static void test_allocator_missing_callbacks(void)
{
    test_memory_provider provider = {0U};
    provider.capacity = TEST_MEMORY_SLOTS;
    const hps_allocator_t allocator = test_provider_allocator(&provider);
    const hps_allocator_t absent = {NULL, NULL, NULL, NULL, &provider};
    void *result = hps_allocator_alloc(NULL, 1U);
    assert(result == NULL);
    result = hps_allocator_alloc(&absent, 1U);
    assert(result == NULL);
    result = hps_allocator_calloc(NULL, 1U, 1U);
    assert(result == NULL);
    result = hps_allocator_calloc(&absent, 1U, 1U);
    assert(result == NULL);
    uint8_t *owner = hps_allocator_alloc(&allocator, 2U);
    assert(owner != NULL);
    owner[0] = 81U;
    result = hps_allocator_realloc(NULL, owner, 2U, 1U);
    assert(result == NULL);
    result = hps_allocator_realloc(&absent, owner, 2U, 1U);
    assert(result == NULL);
    result = hps_allocator_realloc(&absent, owner, 2U, 0U);
    assert(result == NULL);
    hps_allocator_free(NULL, owner, 2U);
    hps_allocator_free(&absent, owner, 2U);
    hps_allocator_free(&allocator, NULL, 0U);
    assert(provider.resize_calls == 0U);
    assert(provider.release_calls == 0U);
    assert(provider.used[0] == 1U);
    assert(owner[0] == 81U);
    hps_allocator_free(&allocator, owner, 2U);
    assert(provider.release_calls == 1U);
}

static int32_t test_clone_owned(void *dst, const void *src,
                                 const hps_allocator_t *allocator)
{
    test_memory_provider *provider = allocator->ctx;
    test_clone_element *destination = dst;
    const test_clone_element *source = src;
    provider->clone_calls += 1U;
    uint8_t *bytes = hps_allocator_alloc(allocator, source->size);
    if (bytes == NULL) {
        return -INT32_C(51);
    }
    if (provider->clone_calls == provider->fail_after_allocation) {
        /* A failed cloner owns its partial cleanup, before array rollback. */
        hps_allocator_free(allocator, bytes, source->size);
        return INT32_C(9);
    }
    for (size_t index = 0U; index < source->size; index += 1U) {
        bytes[index] = source->bytes[index];
    }
    destination->bytes = bytes;
    destination->size = source->size;
    destination->identifier = source->identifier;
    return INT32_C(0);
}

static void test_clone_destruct(void *item, const hps_allocator_t *allocator)
{
    test_memory_provider *provider = allocator->ctx;
    test_clone_element *element = item;
    assert(provider->destruct_calls < TEST_MEMORY_SLOTS);
    provider->destruct_order[provider->destruct_calls] = element->identifier;
    provider->destruct_calls += 1U;
    hps_allocator_free(allocator, element->bytes, element->size);
    element->bytes = NULL;
}

static int32_t test_clone_byte(void *dst, const void *src,
                                const hps_allocator_t *allocator)
{
    uint8_t *destination = dst;
    const uint8_t *source = src;
    assert(allocator == NULL);
    if (*source == UINT8_MAX) {
        return -INT32_C(24);
    }
    *destination = *source;
    return INT32_C(0);
}

static void test_clone_validation_and_shallow(void)
{
    test_memory_provider provider = {0U};
    const hps_allocator_t allocator = test_provider_allocator(&provider);
    const uint8_t source[] = {7U, 9U, 13U, 21U};
    uint8_t destination[] = {83U, 83U, 83U, 83U};
    int32_t status = hps_array_deep_copy(NULL, source, 0U, 0U,
                                         test_clone_owned,
                                         test_clone_destruct, &allocator);
    assert(status == HPS_CLONE_INVALID_ARGUMENT);
    status = hps_array_deep_copy(destination, NULL, 0U, 0U,
                                 test_clone_owned, test_clone_destruct,
                                 &allocator);
    assert(status == HPS_CLONE_INVALID_ARGUMENT);
    status = hps_array_deep_copy(destination, source, 1U, 0U,
                                 test_clone_owned, test_clone_destruct,
                                 &allocator);
    assert(status == HPS_CLONE_INVALID_ARGUMENT);
    status = hps_array_deep_copy(destination, source, SIZE_MAX, 2U,
                                 test_clone_owned, test_clone_destruct,
                                 &allocator);
    assert(status == HPS_CLONE_SIZE_OVERFLOW);
    status = hps_array_deep_copy(destination, source, 0U, 0U,
                                 test_clone_owned, test_clone_destruct,
                                 &allocator);
    assert(status == HPS_CLONE_OK);
    assert(provider.clone_calls == 0U);
    assert(provider.allocate_calls == 0U);
    assert(provider.destruct_calls == 0U);
    for (size_t index = 0U; index < sizeof(destination); index += 1U) {
        assert(destination[index] == 83U);
    }
    status = hps_array_deep_copy(destination, source, SIZE_MAX, 2U,
                                 NULL, NULL, NULL);
    assert(status == HPS_CLONE_SIZE_OVERFLOW);
    status = hps_array_deep_copy(destination, source, sizeof(source), 1U,
                                 NULL, NULL, NULL);
    assert(status == HPS_CLONE_OK);
    for (size_t index = 0U; index < sizeof(destination); index += 1U) {
        assert(destination[index] == source[index]);
    }
    status = hps_array_deep_copy(destination, source, sizeof(source), 1U,
                                 test_clone_byte, NULL, NULL);
    assert(status == HPS_CLONE_OK);
    for (size_t index = 0U; index < sizeof(destination); index += 1U) {
        assert(destination[index] == source[index]);
    }
    const uint8_t failing_source[] = {5U, UINT8_MAX};
    status = hps_array_deep_copy(destination, failing_source,
                                 sizeof(failing_source), 1U,
                                 test_clone_byte, NULL, NULL);
    assert(status == HPS_CLONE_ELEMENT_FAILED);
}

static void test_clone_owning_success(void)
{
    test_memory_provider provider = {0U};
    provider.capacity = TEST_MEMORY_SLOTS;
    const hps_allocator_t allocator = test_provider_allocator(&provider);
    uint8_t source_bytes[TEST_MEMORY_SLOTS][2] = {
        {3U, 4U}, {5U, 6U}, {7U, 8U}
    };
    const test_clone_element source[] = {
        {source_bytes[0], 2U, 31U}, {source_bytes[1], 2U, 32U},
        {source_bytes[2], 2U, 33U}
    };
    test_clone_element destination[TEST_MEMORY_SLOTS];
    const int32_t status = hps_array_deep_copy(
        destination, source, TEST_MEMORY_SLOTS, sizeof(source[0]),
        test_clone_owned, test_clone_destruct, &allocator);
    assert(status == HPS_CLONE_OK);
    assert(provider.clone_calls == TEST_MEMORY_SLOTS);
    assert(provider.destruct_calls == 0U);
    for (size_t index = 0U; index < TEST_MEMORY_SLOTS; index += 1U) {
        assert(destination[index].bytes == provider.bytes[index]);
        assert(destination[index].bytes != source[index].bytes);
        assert(destination[index].identifier == source[index].identifier);
        assert(destination[index].size == source[index].size);
        assert(destination[index].bytes[0] == source_bytes[index][0]);
        assert(destination[index].bytes[1] == source_bytes[index][1]);
    }
    destination[0].bytes[0] = 99U;
    assert(source_bytes[0][0] == 3U);
    for (size_t index = 0U; index < TEST_MEMORY_SLOTS; index += 1U) {
        test_clone_destruct(&destination[index], &allocator);
        assert(provider.used[index] == 0U);
    }
    assert(provider.release_calls == TEST_MEMORY_SLOTS);
}

static void test_clone_failure_cleanup(void)
{
    uint8_t source_bytes[TEST_MEMORY_SLOTS][2] = {
        {3U, 4U}, {5U, 6U}, {7U, 8U}
    };
    const test_clone_element source[] = {
        {source_bytes[0], 2U, 31U}, {source_bytes[1], 2U, 32U},
        {source_bytes[2], 2U, 33U}
    };
    /* Exhaustion fails before the third destination slot is initialized. */
    test_memory_provider exhausted = {0U};
    exhausted.capacity = 2U;
    const hps_allocator_t allocator = test_provider_allocator(&exhausted);
    test_clone_element destination[TEST_MEMORY_SLOTS];
    int32_t status = hps_array_deep_copy(
        destination, source, TEST_MEMORY_SLOTS, sizeof(source[0]),
        test_clone_owned, test_clone_destruct, &allocator);
    assert(status == HPS_CLONE_ELEMENT_FAILED);
    assert(exhausted.clone_calls == 3U);
    assert(exhausted.destruct_calls == 2U);
    assert(exhausted.destruct_order[0] == 31U);
    assert(exhausted.destruct_order[1] == 32U);
    assert(exhausted.release_calls == 2U);
    assert(exhausted.release_order[0] == 0U);
    assert(exhausted.release_order[1] == 1U);
    for (size_t index = 0U; index < TEST_MEMORY_SLOTS; index += 1U) {
        assert(exhausted.used[index] == 0U);
        assert(source[index].bytes == source_bytes[index]);
        assert(source[index].size == 2U);
        assert(source[index].identifier == (31U + index));
        assert(source_bytes[index][0] == (3U + (2U * index)));
        assert(source_bytes[index][1] == (4U + (2U * index)));
    }
    test_memory_provider partial = {0U};
    partial.capacity = TEST_MEMORY_SLOTS;
    partial.fail_after_allocation = 2U;
    const hps_allocator_t partial_allocator = test_provider_allocator(&partial);
    status = hps_array_deep_copy(
        destination, source, TEST_MEMORY_SLOTS, sizeof(source[0]),
        test_clone_owned, test_clone_destruct, &partial_allocator);
    assert(status == HPS_CLONE_ELEMENT_FAILED);
    assert(partial.clone_calls == 2U);
    assert(partial.destruct_calls == 1U);
    assert(partial.destruct_order[0] == 31U);
    assert(partial.release_calls == 2U);
    assert(partial.release_order[0] == 1U);
    assert(partial.release_order[1] == 0U);
    for (size_t index = 0U; index < TEST_MEMORY_SLOTS; index += 1U) {
        assert(partial.used[index] == 0U);
    }
    /* Destination values are deliberately discarded after either failure. */
}

int32_t test_memory_clone(void)
{
    test_allocator_product();
    test_allocator_dispatch();
    test_allocator_zeroed();
    test_allocator_missing_callbacks();
    test_clone_validation_and_shallow();
    test_clone_owning_success();
    test_clone_failure_cleanup();
    return INT32_C(0);
}
