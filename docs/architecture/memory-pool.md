# Static byte-block memory pool

The pool follows the requested design: 32 file-scope static blocks, each containing
128 bytes and a private next pointer. Initialization links the blocks; allocation
pops the head and release pushes it back. Allocation state is private. No heap,
OS, callbacks, locks, atomics or runtime expansion are used. No .bss placement
is promised by portable C; confirm sections and budget in the target linker map.

## Public API

```c
#include "hps/memory/pool.h"

/* Call hps_pool_init() once during exclusive startup first. */
hps_pool_status buffer_example(void)
{
    uint8_t *buffer = hps_calloc(16U, sizeof(uint8_t));
    hps_pool_status status = hps_pool_last_status();
    if (buffer != NULL) {
        buffer[0] = 42U;
        uint8_t *resized = hps_realloc(buffer, 64U);
        status = hps_pool_last_status();
        if (resized != NULL) {
            buffer = resized;
        }
        hps_free(buffer);
        if (status == HPS_POOL_OK) {
            status = hps_pool_last_status();
        }
    }
    return status;
}
```

| Function | Signature and behavior |
| --- | --- |
| hps_pool_init | hps_pool_status (void); initialize once, reject later initialization |
| hps_malloc | void * (size_t); one block for 1..128 bytes; NULL on failure |
| hps_free | void (void *); NULL no-op; validate address and allocated state before insertion |
| hps_realloc | void * (void *, size_t); NULL input allocates; live input and zero size frees; otherwise keep same block for sizes <=128; failure preserves old allocation |
| hps_calloc | void * (size_t, size_t); nonzero factors, checked product <=128; zero exactly the requested bytes |
| hps_pool_last_status | hps_pool_status (void); latest mutation result; read does not change it |
| hps_pool_get_stats | hps_pool_status (hps_pool_stats *); report block size, capacity, available and allocated |

The four allocation signatures match the C function forms. Symbols remain HPS
namespaced: no standard-library interposition or allocation macro redefinition.
Zero-sized malloc/calloc returns NULL with INVALID_ARGUMENT. realloc(NULL, 0)
follows that policy; realloc(live, 0) releases successfully. These are documented
pool choices, not a promise of all zero-size behaviors across libc implementations.

This is byte-buffer storage, not a drop-in general typed-object allocator. Arrays
are declared uint8_t objects; use the returned addresses as uint8_t buffers. Do not
cast them to structures, numerical scalars or over-aligned objects. Signature
compatibility does not establish standard malloc alignment/effective-type semantics.
The void/byte boundary follows the user's explicit interface request and needs
MISRA pointer-rule analysis; no external-standard deviation is approved here.
For typed objects, provide a separate native typed-storage module after review.

## Ownership, validation and work bounds

Initialize once in one context. Reinitialization is rejected even when empty, so
it cannot discard live ownership. No automatic initialization or shutdown reset.
Task/ISR/signal interleaving is prohibited. Each successful request consumes one
whole block; requests larger than a block are rejected even if other blocks are
available. There is no external fragmentation; unused space inside a block is
internal waste. Allocation failure never consumes a block or mutates its payload.
Free and successful realloc-to-zero retain bytes; neither is secret erasure.

free/realloc compare a determinate input address against up to 32 payload starts.
They never dereference the supplied pointer, subtract/order unrelated pointers or
cast addresses to integers. Foreign and interior addresses are rejected. A second
free before reuse is detected. A stale alias after reuse has the same address as
the new owner and cannot be detected with the requested pointer-only API. Discard
all aliases after release and enforce ownership at the application boundary.
Dangling/indeterminate pointers and corrupted private metadata remain outside the
contract. Pointers are not authorization. No arbitrary RAM-fault containment claim.

| Operation | Work bound |
| --- | --- |
| init | O(number of blocks), fixed 32 iterations |
| malloc | O(1) list pop; no search or payload clear |
| free | O(number of blocks) address validation, at most 32 equality comparisons; O(1) push |
| realloc with non-NULL input | Same bounded address lookup; O(1) resize/free, no copy |
| calloc | O(1) pop plus O(requested bytes), at most 128 stores |
| stats/last status | O(1) |

Do not label the lookup search-free or scalable O(1). Fixed limits bound execution
but are not measured latency/WCET evidence. The block count and size are fixed API
constants, not silently differing per-file configurations. Private storage budget
is 32*sizeof(private block), including pointer/flag/padding overhead, plus a head
pointer, size_t counter, uint8_t flag and status; verify actual ABI sizes, stack and
linker layout on a selected target. Payload budget alone is 4096 bytes.

## Migration and evidence

The earlier caller-owned typed arrays and owner/index/generation APIs are removed.
Replace acquire/resolve/release with the four functions above; callers no longer
supply metadata or payload arrays. Generation retirement and post-reuse stale-handle
rejection no longer exist. This is an intentional API/ABI break at development 0.1.0.

Prepared tests cover signatures, pre-init use, reinit rejection, zero/oversize,
calloc product overflow and clearing, realloc preservation, exhaustion/recovery,
foreign/interior addresses and immediate double free. They have not been compiled
or run under the user's no-build instruction. Earlier allocator test and review
results do not validate this replacement. See [current review](misra-review.md)
and [pointer-pool change record](../../governance/reviews/pointer-pool-2026-10-11.md).

The public header has independent standard dependencies and guarded extern "C"
for declarations. C/C++ mixed compilation/linking and target ABI remain untested.
No MISRA compliance, industrial qualification, thread-safety or timing claim is made.
