# Pointer byte-pool design change — 2026-10-11

Authorization: the user supplied a file-scope byte-block/free-list design, clarified
that this is the intended static pool, and requested implementation with exact
malloc/free/realloc/calloc signatures. This record replaces the prior typed-handle
pool contract. It is an author design/source review, not independent approval or an
external-standard deviation. The no-build and no-thread-safety instructions remain.

## Implementation and policy mapping

- HPS-C-001/002/005: numerical declarations remain uint8_t or size_t; hosted main
  retains its explicit exception. No bool, basic numerical aliases or least types.
- HPS-HDR-001 through 005, HPS-C-013: independent guarded public header and guarded
  C linkage; private block definition, arrays, flags and free-list state in source.
  Four namespaced functions have the requested parameter/return types. This does
  not interpose the standard library. C++/ABI qualification under HDR-006 is open.
- HPS-MEM-001/002/005, HPS-EXEC-001: one statically budgeted 32x128-byte pool; private
  pointer free list, no heap/OS/locks/atomics. malloc list pop is O(1). free/realloc
  safely match an input address by up to 32 equalities, then O(1) push/resize.
  calloc clears up to 128 bytes. No scalable O(1) free or measured WCET claim.
- HPS-C-006/007/009/011: counts stay in [0,32]; allocate decrements only above zero;
  release increments only for a live allocation and available <32. Init next index
  is formed only below last index. calloc checks count <=128/size with nonzero
  size before evaluating the product. Resize failure preserves old bytes/ownership.
  Zero-sized behavior and status diagnostics are explicit. Essential-type/pointer
  semantics still require target analyzer review, not just spelling checks.
- HPS-C-010, HPS-MEM-004: the requested void-pointer signature is a user-directed
  boundary for byte buffers. No pointer/integer conversion, foreign-pointer
  dereference, relational pointer comparison or unrelated-object cast occurs in
  the allocator. byte/void conversions and examples require MISRA analysis. Native
  uint8_t storage is not advertised as general typed malloc storage or arbitrary
  alignment. Previous generation/retirement guarantees are removed intentionally.
- HPS-MEM-006, HPS-SEC-001/003/005/006: basic count/list/allocated-state checks do
  not contain RAM faults. Input pointer equality rejects foreign/interior addresses
  and immediate double free. A stale address after reuse is indistinguishable;
  caller ownership must prevent this. No automatic recovery/reset, secret wipe,
  retry or logging. Error state is shared in the exclusive execution context.
- HPS-SEC-009, HPS-REL-001/003: rewritten tests and hosted example cover the new API.
  Old C tests and source review are historical and provide no replacement evidence.
  Breaking migration, byte-only use, fixed capacity and residual risks are documented.

## Threat boundary and open gates

Assets: payload lifetime, private free-list integrity and finite availability.
Entry points: init, four allocation functions, diagnostics and stats. Trusted
assumptions: intact private metadata, one execution context and determinate pointer
values. Foreign addresses are not dereferenced; clients must discard all aliases
on release. Attackers controlling payload bytes do not legitimately control metadata;
out-of-bounds writes require separate protection. Addresses are not authentication.
Sensitive-byte erasure, overload admission, tenant isolation, MPU/ECC, linker/stack
budgets, DMA/cache alignment and recovery are target/application obligations.

No full MISRA assessment or qualification occurred. Directive 4.12 remains open;
static backing storage does not approve runtime allocation under a strict profile.
Compiler/modes, libc and linked dependencies, pointer/essential-type rules, target
measurements and independent review remain required. No industrial certification,
full malloc replacement, thread safety or performance measurement is claimed.

The function-type reference is the WG14 C99 consolidated draft
[N1256, section 7.20.3](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf).
Matching function types alone does not establish standard allocator semantics.

## Evidence

`python3 tools/check_policy.py --self-test` and `git diff --check` pass on this
snapshot. The release-record gate still rejects 221 unresolved MISRA rows. No C
build, runtime test, analyzer or target qualification was run as instructed.
Prepared tests verify signatures, startup/reinit, bounds, calloc arithmetic/zeroing,
resize preservation, foreign/interior pointers, immediate double free and repeated
exhaustion/recovery. These are test intentions, not passing runtime results.

## Changed source hashes

| File | SHA-256 |
| --- | --- |
| include/hps/memory/pool.h | `3220f354011f27de08db09810d17cffd4b855c3e80688b7ecbe9beaf0271b48c` |
| src/memory/pool.c | `d30a1701250401bef840a15343ebc96eb990d2320ec7a3f4796a3dd4ae95b560` |
| src/main.c | `9084ee4b922faf3363f90f0318e3a98e02f9939c7b9533784859b1985c9ad4c0` |
| tests/memory/test_pool.c | `898b3198beec3454975d0cb7bb647a98425f8799c7552f115dcd14fc75d043c2` |
