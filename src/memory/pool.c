#include "hps/memory/pool.h"

/**
 * @file src/memory/pool.c
 * @brief File-scope static blocks and a pointer free list; no heap or OS calls.
 */

typedef struct hps_pool_block {
    uint8_t data[HPS_POOL_BUFFER_SIZE];
    struct hps_pool_block *next;
    uint8_t allocated;
} hps_pool_block;

/* Actual segment placement is a linker/target matter, not a portable C promise.
 * Budget: sizeof(storage) + head/count/flags/diagnostics. */
static hps_pool_block storage[HPS_POOL_MAX_BUFFERS];
static hps_pool_block *free_head = NULL;
static size_t available = 0U;
static uint8_t initialized = 0U;
static int32_t last_status = HPS_POOL_NOT_INITIALIZED;

/* Compare only: never dereference a caller-supplied address. The bounded lookup
 * deliberately avoids pointer subtraction, ordering and pointer/integer casts. */
static size_t pool_find(const void *ptr)
{
    size_t index = 0U;
    while ((index < HPS_POOL_MAX_BUFFERS) &&
           (ptr != &storage[index].data[0])) {
        index += 1U;
    }
    return index;
}

static int32_t pool_check(void)
{
    int32_t status = HPS_POOL_NOT_INITIALIZED;
    if (initialized == 1U) {
        status = HPS_POOL_CORRUPT;
        if (available <= HPS_POOL_MAX_BUFFERS) {
            if (((available == 0U) && (free_head == NULL)) ||
                ((available != 0U) && (free_head != NULL))) {
                status = HPS_POOL_OK;
            }
        }
    }
    return status;
}

static int32_t pool_validate(const void *ptr, size_t *index)
{
    int32_t status = pool_check();
    if (status == HPS_POOL_OK) {
        const size_t found = pool_find(ptr);
        if (found == HPS_POOL_MAX_BUFFERS) {
            status = HPS_POOL_INVALID_POINTER;
        } else if (storage[found].allocated == 0U) {
            status = HPS_POOL_DOUBLE_FREE;
        } else if ((storage[found].allocated != 1U) ||
                   (storage[found].next != NULL) ||
                   (available == HPS_POOL_MAX_BUFFERS)) {
            status = HPS_POOL_CORRUPT;
        } else {
            *index = found;
        }
    }
    return status;
}

/* Metadata is private and assumed intact. This helper does not claim detection
 * of arbitrary corrupted free-list pointers or a complete list integrity proof. */
static hps_pool_block *pool_take(void)
{
    hps_pool_block *block = NULL;
    last_status = pool_check();
    if (last_status == HPS_POOL_OK) {
        if (available == 0U) {
            last_status = HPS_POOL_EXHAUSTED;
        } else {
            last_status = HPS_POOL_CORRUPT;
            if ((free_head->allocated == 0U) &&
                (free_head->next != free_head)) {
                if (((available == 1U) && (free_head->next == NULL)) ||
                    ((available > 1U) && (free_head->next != NULL))) {
                    block = free_head;
                    free_head = block->next;
                    block->next = NULL;
                    block->allocated = 1U;
                    available -= 1U;
                    last_status = HPS_POOL_OK;
                }
            }
        }
    }
    return block;
}

static void pool_put(const size_t index)
{
    storage[index].next = free_head;
    free_head = &storage[index];
    storage[index].allocated = 0U;
    available += 1U;
}

int32_t hps_pool_init(void)
{
    last_status = HPS_POOL_ALREADY_INITIALIZED;
    if (initialized == 0U) {
        for (size_t index = 0U; index < HPS_POOL_MAX_BUFFERS; index += 1U) {
            if (index < (HPS_POOL_MAX_BUFFERS - 1U)) {
                storage[index].next = &storage[index + 1U];
            } else {
                storage[index].next = NULL;
            }
            storage[index].allocated = 0U;
        }
        free_head = &storage[0];
        available = HPS_POOL_MAX_BUFFERS;
        initialized = 1U;
        last_status = HPS_POOL_OK;
    }
    return last_status;
}

void *hps_malloc(const size_t size)
{
    void *result = NULL;
    last_status = HPS_POOL_INVALID_ARGUMENT;
    if ((size != 0U) && (size <= HPS_POOL_BUFFER_SIZE)) {
        hps_pool_block *block = pool_take();
        if (block != NULL) {
            result = &block->data[0];
        }
    }
    return result;
}

void hps_free(void *ptr)
{
    last_status = HPS_POOL_OK;
    if (ptr != NULL) {
        size_t index = 0U;
        last_status = pool_validate(ptr, &index);
        if (last_status == HPS_POOL_OK) {
            pool_put(index);
        }
    }
}

void *hps_realloc(void *ptr, const size_t size)
{
    void *result = NULL;
    if (ptr == NULL) {
        result = hps_malloc(size);
    } else {
        size_t index = 0U;
        last_status = pool_validate(ptr, &index);
        if (last_status == HPS_POOL_OK) {
            if (size == 0U) {
                pool_put(index);
            } else if (size > HPS_POOL_BUFFER_SIZE) {
                last_status = HPS_POOL_INVALID_ARGUMENT;
            } else {
                result = ptr;
            }
        }
    }
    return result;
}

void *hps_calloc(const size_t count, const size_t size)
{
    void *result = NULL;
    last_status = HPS_POOL_INVALID_ARGUMENT;
    if ((count != 0U) && (size != 0U)) {
        if (count <= (HPS_POOL_BUFFER_SIZE / size)) {
            const size_t bytes = count * size;
            hps_pool_block *block = pool_take();
            if (block != NULL) {
                for (size_t index = 0U; index < bytes; index += 1U) {
                    block->data[index] = 0U;
                }
                result = &block->data[0];
            }
        }
    }
    return result;
}

int32_t hps_pool_last_status(void)
{
    return last_status;
}

int32_t hps_pool_get_stats(hps_pool_stats *result)
{
    int32_t status = HPS_POOL_INVALID_ARGUMENT;
    if (result != NULL) {
        status = pool_check();
        if (status == HPS_POOL_OK) {
            result->capacity = HPS_POOL_MAX_BUFFERS;
            result->block_size = HPS_POOL_BUFFER_SIZE;
            result->available = available;
            result->allocated = HPS_POOL_MAX_BUFFERS - available;
        }
    }
    return status;
}
