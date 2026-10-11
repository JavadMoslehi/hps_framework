#ifndef HPS_MEMORY_POOL_H
#define HPS_MEMORY_POOL_H

/**
 * @file hps/memory/pool.h
 * @brief Single-context static byte-block allocator with malloc-family signatures.
 */

#include <stddef.h>
#include <stdint.h>

/* Fixed ABI/resource budget; these are not per-translation-unit overrides. */
#define HPS_POOL_MAX_BUFFERS 32U
#define HPS_POOL_BUFFER_SIZE 128U

typedef uint8_t hps_pool_status;
#define HPS_POOL_OK ((hps_pool_status)0U)
#define HPS_POOL_INVALID_ARGUMENT ((hps_pool_status)1U)
#define HPS_POOL_EXHAUSTED ((hps_pool_status)2U)
#define HPS_POOL_INVALID_POINTER ((hps_pool_status)3U)
#define HPS_POOL_DOUBLE_FREE ((hps_pool_status)4U)
#define HPS_POOL_NOT_INITIALIZED ((hps_pool_status)5U)
#define HPS_POOL_ALREADY_INITIALIZED ((hps_pool_status)6U)
#define HPS_POOL_CORRUPT ((hps_pool_status)7U)

typedef struct {
    size_t capacity;
    size_t block_size;
    size_t available;
    size_t allocated;
} hps_pool_stats;

#ifdef __cplusplus
extern "C" {
#endif

/* One process-wide pool, exclusive startup/use; no task/ISR/signal interleaving.
 * Private file-scope storage never grows, blocks or calls the standard heap.
 * Storage is declared uint8_t byte arrays: void* signatures do not grant arbitrary
 * typed-object alignment/effective-type guarantees of the standard C allocator.
 * Only byte-buffer use is supported. Do not cast payloads to structures/scalars.
 * Discard all aliases after free; a stale address reused by a new allocation
 * cannot be distinguished from its current owner. No pointer is a capability.
 * Failures preserve live allocations and payloads; last-status is updated.
 * CORRUPT requires system fault handling, not reinitialization. No RAM-fault
 * containment, secret erasure or concurrency guarantee is provided.
 */

/* Call once before allocating. OK initializes the free list, not payload bytes.
 * A later call returns ALREADY_INITIALIZED without resetting live ownership.
 * O(MAX_BUFFERS) startup work, bounded automatic storage. */
hps_pool_status hps_pool_init(void);

/* Signatures match the corresponding C allocation functions; symbols remain
 * namespaced. Each successful request owns one complete BUFFER_SIZE-byte block.
 * size in [1, BUFFER_SIZE]; zero/oversize returns NULL (INVALID_ARGUMENT).
 * Exhaustion returns NULL (EXHAUSTED); no implicit initialization or retry.
 * O(1) free-list work, no payload initialization. */
void *hps_malloc(size_t size);

/* NULL is a successful no-op, including before initialization. Otherwise ptr
 * is a determinate pointer value. Equality lookup rejects foreign/interior
 * addresses without dereferencing them or doing pointer range arithmetic.
 * Immediate double free is rejected; freed payload bytes are retained.
 * O(MAX_BUFFERS) lookup (at most 32 comparisons), O(1) list insertion.
 * The void signature reports errors via hps_pool_last_status(). */
void hps_free(void *ptr);

/* NULL behaves as malloc(size). For a live block: zero frees and returns NULL;
 * size <= BUFFER_SIZE returns the same address, preserving existing bytes;
 * oversize returns NULL and preserves the original allocation. Invalid non-NULL
 * pointers are rejected even with size zero. Growth bytes are unspecified.
 * O(MAX_BUFFERS) validation, O(1) resize; no copy is needed. */
void *hps_realloc(void *ptr, size_t size);

/* count and size are nonzero; product <= BUFFER_SIZE, checked before multiplying.
 * Allocates one block and zeros exactly count*size bytes. Overflow/zero/oversize
 * returns NULL (INVALID_ARGUMENT); exhaustion leaves all payloads unchanged.
 * O(1) allocation plus O(count*size) clearing, at most BUFFER_SIZE bytes. */
void *hps_calloc(size_t count, size_t size);

/* Most recent init/allocation/free/reallocation result. Read-only, O(1).
 * Stats reads do not change this value. Shared single-context diagnostic state. */
hps_pool_status hps_pool_last_status(void);

/* result designates one writable, correctly aligned stats object. Success writes
 * every field; failure leaves it unchanged. Returns OK, INVALID_ARGUMENT,
 * NOT_INITIALIZED or CORRUPT. O(1); available + allocated == capacity. */
hps_pool_status hps_pool_get_stats(hps_pool_stats *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MEMORY_POOL_H */
