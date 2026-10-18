#include "../test_suites.h"
#include "hps/memory/pool.h"
#include <assert.h>

/**
 * @file tests/memory/test_pool.c
 * @brief Prepared fixed-block ownership, size and malloc-family contract tests.
 */

/* Keep side effects outside assert so disabling assertions does not change
 * allocator operations. Test comparisons still require enabled assertions. */
static void test_startup(void)
{
    hps_pool_stats stats = {0U, 0U, 0U, 0U};
    int32_t status = hps_pool_get_stats(&stats);
    assert(status == HPS_POOL_NOT_INITIALIZED);
    assert(stats.capacity == 0U);
    void *ptr = hps_malloc(1U);
    assert(ptr == NULL);
    assert(hps_pool_last_status() == HPS_POOL_NOT_INITIALIZED);
    hps_free(NULL);
    assert(hps_pool_last_status() == HPS_POOL_OK);
    status = hps_pool_init();
    assert(status == HPS_POOL_OK);
    status = hps_pool_get_stats(NULL);
    assert(status == HPS_POOL_INVALID_ARGUMENT);
}

static void test_sizes_and_resize(void)
{
    void *ptr = hps_malloc(0U);
    assert(ptr == NULL);
    ptr = hps_malloc(HPS_POOL_BUFFER_SIZE + 1U);
    assert(ptr == NULL);
    ptr = hps_calloc(SIZE_MAX, 2U);
    assert(ptr == NULL);
    ptr = hps_calloc(1U, SIZE_MAX);
    assert(ptr == NULL);
    ptr = hps_calloc(0U, 1U);
    assert(ptr == NULL);
    ptr = hps_calloc(1U, 0U);
    assert(ptr == NULL);
    uint8_t *bytes = hps_calloc(HPS_POOL_BUFFER_SIZE, sizeof(uint8_t));
    assert(bytes != NULL);
    for (size_t index = 0U; index < HPS_POOL_BUFFER_SIZE; index += 1U) {
        assert(bytes[index] == 0U);
        bytes[index] = 91U;
    }
    ptr = hps_realloc(bytes, HPS_POOL_BUFFER_SIZE + 1U);
    assert(ptr == NULL);
    assert(hps_pool_last_status() == HPS_POOL_INVALID_ARGUMENT);
    assert(bytes[0] == 91U);
    ptr = hps_realloc(bytes, 1U);
    assert(ptr == bytes);
    ptr = hps_realloc(bytes, HPS_POOL_BUFFER_SIZE);
    assert(ptr == bytes);
    assert(bytes[0] == 91U);
    int32_t status = hps_pool_init();
    assert(status == HPS_POOL_ALREADY_INITIALIZED);
    assert(bytes[0] == 91U);
    ptr = hps_realloc(bytes, 0U);
    assert(ptr == NULL);
    assert(hps_pool_last_status() == HPS_POOL_OK);
    hps_free(bytes);
    assert(hps_pool_last_status() == HPS_POOL_DOUBLE_FREE);
    bytes = hps_malloc(HPS_POOL_BUFFER_SIZE);
    assert(bytes != NULL && bytes[0] == 91U);
    hps_free(bytes);
    bytes = hps_calloc(HPS_POOL_BUFFER_SIZE, 1U);
    assert(bytes != NULL);
    for (size_t index = 0U; index < HPS_POOL_BUFFER_SIZE; index += 1U) {
        assert(bytes[index] == 0U);
    }
    hps_free(bytes);
    ptr = hps_realloc(NULL, 1U);
    assert(ptr != NULL);
    hps_free(ptr);
    ptr = hps_realloc(NULL, 0U);
    assert(ptr == NULL);
}

static void test_invalid_addresses(void)
{
    uint8_t foreign[2] = {0U, 0U};
    hps_free(&foreign[0]);
    assert(hps_pool_last_status() == HPS_POOL_INVALID_POINTER);
    void *ptr = hps_realloc(&foreign[0], 0U);
    assert(ptr == NULL);
    assert(hps_pool_last_status() == HPS_POOL_INVALID_POINTER);
    uint8_t *bytes = hps_malloc(2U);
    assert(bytes != NULL);
    hps_free(&bytes[1]);
    assert(hps_pool_last_status() == HPS_POOL_INVALID_POINTER);
    hps_free(bytes);
    assert(hps_pool_last_status() == HPS_POOL_OK);
    hps_free(bytes);
    assert(hps_pool_last_status() == HPS_POOL_DOUBLE_FREE);
}

static void test_exhaustion(void)
{
    void *blocks[HPS_POOL_MAX_BUFFERS] = {NULL};
    hps_pool_stats stats = {0U, 0U, 0U, 0U};
    for (size_t round = 0U; round < 100U; round += 1U) {
        for (size_t index = 0U; index < HPS_POOL_MAX_BUFFERS; index += 1U) {
            blocks[index] = hps_malloc(HPS_POOL_BUFFER_SIZE);
            assert(blocks[index] != NULL);
            for (size_t other = 0U; other < index; other += 1U) {
                assert(blocks[index] != blocks[other]);
            }
        }
        int32_t status = hps_pool_get_stats(&stats);
        assert(status == HPS_POOL_OK);
        assert(stats.available == 0U && stats.allocated == HPS_POOL_MAX_BUFFERS);
        void *extra = hps_malloc(1U);
        assert(extra == NULL);
        assert(hps_pool_last_status() == HPS_POOL_EXHAUSTED);
        extra = hps_calloc(1U, 1U);
        assert(extra == NULL);
        extra = hps_realloc(blocks[0], HPS_POOL_BUFFER_SIZE);
        assert(extra == blocks[0]);
        for (size_t index = 0U; index < HPS_POOL_MAX_BUFFERS; index += 1U) {
            hps_free(blocks[index]);
            assert(hps_pool_last_status() == HPS_POOL_OK);
        }
        status = hps_pool_get_stats(&stats);
        assert(status == HPS_POOL_OK);
        assert(stats.available == HPS_POOL_MAX_BUFFERS && stats.allocated == 0U);
    }
}

/* These function-pointer assignments verify the requested signatures when built. */
static void test_signatures(void)
{
    int32_t (*initialize)(void) = hps_pool_init;
    int32_t (*last_result)(void) = hps_pool_last_status;
    int32_t (*get_stats)(hps_pool_stats *result) = hps_pool_get_stats;
    hps_pool_stats stats = {0U, 0U, 0U, 0U};
    int32_t result = initialize();
    assert(sizeof(hps_pool_init()) == sizeof(int32_t));
    assert(sizeof(hps_pool_last_status()) == sizeof(int32_t));
    assert(sizeof(hps_pool_get_stats(&stats)) == sizeof(int32_t));
    assert(sizeof(HPS_POOL_CORRUPT) == sizeof(int32_t));
    assert(HPS_POOL_OK == INT32_C(0));
    assert(HPS_POOL_INVALID_ARGUMENT == -INT32_C(1));
    assert(HPS_POOL_EXHAUSTED == -INT32_C(2));
    assert(HPS_POOL_INVALID_POINTER == -INT32_C(3));
    assert(HPS_POOL_DOUBLE_FREE == -INT32_C(4));
    assert(HPS_POOL_NOT_INITIALIZED == -INT32_C(5));
    assert(HPS_POOL_ALREADY_INITIALIZED == -INT32_C(6));
    assert(HPS_POOL_CORRUPT == -INT32_C(7));
    assert(result == -INT32_C(6));
    result = last_result();
    assert(result == -INT32_C(6));
    result = get_stats(&stats);
    assert(result == INT32_C(0));
    assert(stats.available == HPS_POOL_MAX_BUFFERS && stats.allocated == 0U);
    result = last_result();
    assert(result == -INT32_C(6));
    void *(*allocate)(size_t) = hps_malloc;
    void (*release)(void *) = hps_free;
    void *(*resize)(void *, size_t) = hps_realloc;
    void *(*zero_allocate)(size_t, size_t) = hps_calloc;
    void *ptr = allocate(1U);
    assert(ptr != NULL);
    void *resized = resize(ptr, 2U);
    assert(resized == ptr);
    release(ptr);
    ptr = zero_allocate(2U, 1U);
    assert(ptr != NULL);
    release(ptr);
}

int test_pool(void)
{
    test_startup();
    test_sizes_and_resize();
    test_invalid_addresses();
    test_exhaustion();
    test_signatures();
    return 0;
}
