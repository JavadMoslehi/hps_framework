# Supplied memory and allocator drafts

Migration date: 2026-10-19. Source base:
`cf5a43715dc6179fac1895131b2a8d8c09699611` plus the user's untracked allocator
draft and pasted memory attachment. Neither read-only reference framework is
used or modified in this step. Exact source snapshots are retained for another
session/machine; these JSON files are reference data, not build inputs. Decode
`content_base64` to restore the original bytes, including whitespace; the hashes
below identify those decoded source bytes.

| Source | Retained snapshot | SHA-256 |
| --- | --- | --- |
| User's `hps_allocator.h` | [allocator-draft.h.json](sources/allocator-draft.h.json) | `367be0e973d6fcb75740e45604af16fc34b9e51e26461d29a83643bb26431335` |
| Pasted `HPS_MEMORY_H` | [memory-draft.h.json](sources/memory-draft.h.json) | `ddaa48a9ebb5c04a9442c91c48afc4b30315ef181bb238a51a9b158a4857e0fe` |

## API and behavior mapping

| Draft | HPS placement / change |
| --- | --- |
| `hps_memset`, `hps_memcpy`, `hps_memmove`, `hps_memcmp`, `hps_memchr` | `hps/memory/operations.h`, external C functions implemented in `src/memory/operations.c`; link `HPS::memory`/`hps-memory` |
| `hps_memzero_secure` | `hps_memzero_volatile`; describes volatile writes without an unverified sensitive-erasure promise |
| Local `HPS_INLINE` detection | Existing `hps/base/attributes.h` for header-only proposals |
| `hps_word_t`, `HPS_WORD_SIZE`, `HPS_WORD_MASK` | Removed; pointer-to-size conversions and unrelated word-type loads/stores do not establish portable alignment/effective type |
| `hps_allocator_t`, `struct hps_allocator_i`, `ctx` | Retained in `hps/memory/allocator.h`; callback order/argument types unchanged |
| Vtable `alloc`, `calloc`, `realloc`, `free` | `allocate`, `allocate_zeroed`, `resize`, `release`; update designated initializers and member references |
| Allocator `hps_malloc`, `hps_calloc`, `hps_realloc`, `hps_free` | `hps_allocator_alloc`, `hps_allocator_calloc`, `hps_allocator_realloc`, `hps_allocator_free`; the pool's existing names/signatures are unchanged |
| `HPS_DEFAULT_ALIGNMENT` | `HPS_ALLOCATOR_DEFAULT_ALIGNMENT`; still `sizeof(void *)`, only a request hint, not proof of alignment for any type |
| `hps_check_mul_overflow` | `hps_allocator_check_mul_overflow`, `uint8_t` flag; NULL output is rejected and output is preserved on overflow |
| `hps_clone_fn`, `hps_destruct_fn`, `hps_array_deep_copy` | Optional `hps/memory/clone.h`; allocator parameter and callback ownership design retained |
| Native `int` byte arguments/results | Byte arguments use `uint8_t`; comparison, cloner and deep-copy results use `int32_t` directly |

Memory movement now delegates to libc's overlap contract without comparing
unrelated pointers. Fill/copy also use existing libc primitives; this module is
not a standalone freestanding libc replacement. Comparison preserves the exact
first unsigned-byte difference, including -255/+255 on qualified 8-bit targets.
Search returns `const void *`, preserving input constness. Zero-length primitive
calls explicitly permit NULL and access no payload. Nonempty pointer extents,
lifetime and memcpy nonoverlap remain caller obligations. There is no new invalid
pointer detection or error-result channel for primitives. Conversion of a caller's
integer fill/search value now occurs at the `uint8_t` API boundary.

The operations and clone implementation require `uint8_t` to be an unsigned
character type when walking arbitrary object representations. This is a recorded
target restriction, not a guarantee supplied by the mere presence of `uint8_t`.
Unusual implementations need separate adaptation and qualification. Typed storage,
alignment, object validity and representation rules still apply; the pool cannot
provide arbitrary typed objects just because its return type is `void *`.

Clone validates both array pointers first, preserving the draft's NULL rejection
even for zero count. Positive count with zero element size is newly rejected.
`count * element_size` is checked before copying, pointer arithmetic or callbacks;
overflow returns `HPS_CLONE_SIZE_OVERFLOW` (-2) without mutation. Invalid arguments
return `HPS_CLONE_INVALID_ARGUMENT` (-1). Success is `HPS_CLONE_OK` (0); any nonzero
cloner result maps to `HPS_CLONE_ELEMENT_FAILED` (-3), rather than the draft's
undifferentiated -1. These codes belong only to this API.

A failed cloner cleans its own partial element. Rollback calls the supplied
destructor on completed elements in the original forward order. Resource-owning
clones require a destructor; omitting it is allowed only when no cleanup is needed.
Destination elements must initially own no live resources, arrays/context metadata
must be appropriately disjoint, and callbacks must preserve source ownership.
Rollback releases completed children, but does not restore destination bytes.
The draft's unconditional transactional/no-leak wording is narrowed accordingly.

Allocator dispatch/fallback behavior is retained: missing optional callbacks
give NULL/no-op, zeroed allocation can fall back to allocation plus byte clearing,
NULL resize allocates, and resizing a live pointer to zero invokes release if
provided. With no release callback that last path cannot release ownership;
callers must retain the original owner and arrange cleanup separately. Zero allocation
sizes continue to be provider-defined. Resize failure preserves the caller's
old owner by provider contract; callers must not overwrite it with NULL.

These are explicit source/API changes to drafts, not a binary-compatible import.
Rebuild consumers; update names, return/callback widths, search constness and
numeric error expectations. No compatibility aliases are provided for the draft's
pool-conflicting functions. Old header paths remain forwarding includes only.
The optional allocator/clone contract is still a proposed project exception under
HPS-MEM-001, with no production-core call site or built-in heap provider.

## Continuation

Read [the layer map](../architecture/layers.md), canonical headers and the
[change review](../../governance/reviews/memory-layers-2026-10-19.md) before further
integration. Keep the retained snapshots unchanged. Next steps for release are
named policy disposition, provider-specific typed storage/alignment/lifetime
proofs, callback time/resource bounds, target libc/erasure evidence and target
ABI/MISRA review. Host tests do not resolve those gaps.
