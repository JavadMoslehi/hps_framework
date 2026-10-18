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
int32_t buffer_example(void)
{
    uint8_t *buffer = hps_calloc(16U, sizeof(uint8_t));
    int32_t status = hps_pool_last_status();
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
| hps_pool_init | int32_t (void); initialize once, reject later initialization |
| hps_malloc | void * (size_t); one block for 1..128 bytes; NULL on failure |
| hps_free | void (void *); NULL no-op; validate address and allocated state before insertion |
| hps_realloc | void * (void *, size_t); NULL input allocates; live input and zero size frees; otherwise keep same block for sizes <=128; failure preserves old allocation |
| hps_calloc | void * (size_t, size_t); nonzero factors, checked product <=128; zero exactly the requested bytes |
| hps_pool_last_status | int32_t (void); latest mutation result; read does not change it |
| hps_pool_get_stats | int32_t (hps_pool_stats *); report block size, capacity, available and allocated |

The four allocation signatures match the C function forms. Symbols remain HPS
namespaced: no standard-library interposition or allocation macro redefinition.
Zero-sized malloc/calloc returns NULL with INVALID_ARGUMENT. realloc(NULL, 0)
follows that policy; realloc(live, 0) releases successfully. These are documented
pool choices, not a promise of all zero-size behaviors across libc implementations.

The result-returning pool APIs use `int32_t` directly from `<stdint.h>`. Zero
means success; local negative values distinguish the errors below. Constants in
`hps/memory/pool.h` are typed as `int32_t`. Codes belong to this API and may overlap
codes from other APIs; compare the named pool constants. No shared result typedef,
module registry or diagnostic-string lookup is required.

| Status | Value |
| --- | --- |
| HPS_POOL_OK | 0 |
| HPS_POOL_INVALID_ARGUMENT | -1 |
| HPS_POOL_EXHAUSTED | -2 |
| HPS_POOL_INVALID_POINTER | -3 |
| HPS_POOL_DOUBLE_FREE | -4 |
| HPS_POOL_NOT_INITIALIZED | -5 |
| HPS_POOL_ALREADY_INITIALIZED | -6 |
| HPS_POOL_CORRUPT | -7 |

The memory target (`HPS::memory` in CMake, `hps-memory` in Meson) has no shared
result-library dependency. `hps_pool_last_status` retains its read-only,
single-context behavior; `hps_pool_get_stats` does not alter the stored result.

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
pointer, size_t counter, uint8_t flag and int32_t result; verify actual ABI sizes,
stack and linker layout on a selected target. Payload budget alone is 4096 bytes.
The last-result object uses `int32_t`; no shared diagnostic registry is linked.
Actual total storage, padding and stack still require target evidence.

## Migration and evidence

The earlier caller-owned typed arrays and owner/index/generation APIs are removed.
Replace acquire/resolve/release with the four functions above; callers no longer
supply metadata or payload arrays. Generation retirement and post-reuse stale-handle
rejection no longer exist. This is an intentional API/ABI break at development 0.1.0.

The shared result abstraction has now been removed at the user's request.
Replace the former pool result alias in callers with direct `int32_t`. The
previous `int64_t` return/storage representation and global codes -65537..-65543
change to `int32_t` and local codes -1..-7. Function names, allocation signatures,
pool capacity, ownership, error priority and failure preservation are unchanged.
The `hps_pool_stats` layout is unchanged because it has no result field. Rebuild
all callers, callbacks and bindings with the matching library; update persisted
numeric assumptions and keep error results in `int32_t`. Old binaries cannot be
mixed with the new headers/library. See the
[result-removal review](../../governance/reviews/status-removal-2026-10-18.md)
for policy/MISRA mapping and verification scope.

Tests cover direct result-returning signatures and local codes, pre-init use,
reinit rejection, zero/oversize requests, calloc product overflow and clearing,
realloc preservation, exhaustion/recovery, foreign/interior addresses and
immediate double free. Current migration evidence is recorded in the review
above. The [string review](../../governance/reviews/string-2026-10-13.md),
[source review](misra-review.md) and
[pointer-pool change record](../../governance/reviews/pointer-pool-2026-10-11.md)
retain their historical scope. They do not verify this result migration or
establish qualified target ABI and resource evidence.

The public header has independent standard dependencies and guarded extern "C"
for declarations. Qualified target C/C++ compilation/linking and ABI validation
remain pending.
No MISRA compliance, industrial qualification, thread-safety or timing claim is made.
