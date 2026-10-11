# Static pointer-pool review status

The current module implements the user's fixed-block/free-list design with
malloc/free/realloc/calloc-compatible signatures and HPS names. Storage is declared
at file scope; no standard heap, OS or synchronization is used. Metadata is private,
calloc multiplication is bounded before evaluation, and release/resize compare
addresses without pointer subtraction, ordering or pointer/integer casts.

This replaces the earlier handle/index API. Its generation and retirement claims
no longer apply. Current ownership rules reject immediate double free, but cannot
reject stale aliases after address reuse. The void-pointer API supports byte-buffer
use only; general typed malloc alignment/effective-type semantics are not promised.

The [change record](../../governance/reviews/pointer-pool-2026-10-11.md) maps policies,
tradeoffs and source hashes. All MISRA dispositions remain unresolved, including
Directive 4.12 for runtime reservation and pointer/essential-type rules needing a
qualified analyzer and authoritative interpretation. No deviation is approved.
See the [MISRA plan](../../governance/misra/README.md) and
[adoption gaps](../../governance/status.md).

Prepared boundary/ownership tests are unexecuted: the user has retained the
no-build instruction. Prior source-review or runtime evidence does not apply to
this replacement. C/C++ ABI, stack, target timing, fault containment, secret erasure,
product threat modeling and profile qualification remain open. No automotive,
telecom, payment or MISRA compliance claim is made.
