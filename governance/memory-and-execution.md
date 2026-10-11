# Memory, ownership and execution

HPS-MEM-001: Core runtime storage must be budgeted before use. No calls to `malloc`,
`calloc`, `realloc`, `free`, `aligned_alloc`, OS memory mapping, runtime heap growth
or dependency allocator hooks in production core paths. No hidden allocation via
containers, logging, initialization, libc/OS wrappers or callback implementations.
A whole-product no-heap claim requires analysis of the entire linked product.

HPS-MEM-002: Storage declarations must record capacity, object size/alignment,
metadata, stack/ROM consumption, owners and lifetime. Use matching capacity constants
for payload and metadata arrays. Stack storage is not a substitute for a verified
stack budget. DMA/cache-aligned data require a target-approved declaration and
cache protocol; plain C alignment is not proof of hardware suitability.

HPS-MEM-003: Runtime reservations from static storage are a distinct design choice
from compile-time object assignment. MISRA Directive 4.12 disposition is required
before a profile claims compliance for acquire/release pools. A strict static-object
profile assigns objects for their entire lifetime and does not acquire/release them.
The current byte-block pool is available for development, not automatically approved
under that strict profile.

HPS-MEM-004: Validate a pool payload-start address and allocation state before
release or resize. No reinitialization, metadata modification or reference retained
after release. Do not overwrite the only live owner with a failed realloc result.
The user-selected pointer API detects immediate double free, but cannot distinguish
a stale alias after an address is reallocated. Integrators must enforce exclusive
ownership and discard aliases; no generation/stale-after-reuse guarantee is made.
Exhaustion requires a defined bounded application response. This replaces the
previous handle/generation requirement under the explicit pointer-pool request;
see [the design-change record](reviews/pointer-pool-2026-10-11.md).

HPS-MEM-005: The current pool has one execution-context owner and no locks. Task/ISR
interleaving is concurrent use and prohibited. A future synchronized layer needs
its own contract, memory-ordering proof and latency budget; it may not silently
change core semantics. No thread-safe, lock-free, ISR-safe or signal-safe claim is
permitted for the current pool.

HPS-MEM-006: Metadata bounds checks are defense in depth, not a RAM-fault containment
mechanism. Identify faults detectable in software and faults needing MPU/MMU/ECC,
watchdogs or redundancy. After a corruption status, stop using affected state and
apply the system fault policy; do not repair it by resetting live allocations.

HPS-EXEC-001: Each API documents algorithmic work, input bounds, blocking behavior,
stack use, startup versus runtime cost and failure paths. The current byte pool pops/pushes its list in O(1); free/realloc first perform an
O(capacity) address lookup bounded by 32 comparisons. Initialization is O(capacity).
Payload clearing is O(bytes), bounded by 128; in-block realloc does not copy. No target
latency guarantee is established by those complexity statements.

HPS-EXEC-002: Any performance claim needs a reproducible target report: CPU/board,
clock, compiler flags, source revision, library configuration, OS/RTOS, workload,
warm/cold cache conditions, instrumentation, repetitions, distribution and failure
paths. Distinguish throughput, observed worst latency, p50/p95/p99 latency and
analytically justified WCET. A benchmark maximum is not a proven WCET.

HPS-EXEC-003: Performance changes must preserve ownership and bounds checks. Record
regressions and the correctness evidence for optimizations. Do not remove checks
because one benchmark improves. SIMD, zero-copy, prefetch, packed data and atomics
require separately reviewed portability/alignment/lifetime contracts.

HPS-EXEC-004: Timers/deadlines need monotonic-clock semantics, units, counter wrap
handling, maximum interval, scheduling effects and recovery. Bounded retries,
backpressure/drop policy and resource ceilings must be explicit for every queue
or external-input processing path. Service loops are not implicitly hard real-time.
