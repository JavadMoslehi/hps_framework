# Attributes, utilities and diagnostics migration — 2026-10-19

All eight user-selected headers were read completely from MyLibraries C-core
layers 01, 02 and 03. Source base: HPS commit
`5813127ccc468a3be75acf5979261f6c4005ba05`. Every original still matches the
existing [read-only inventory](legacy-sources.md). No reference source was edited,
built, executed or configured. The exact original bytes, paths, sizes and hashes
are retained in the lossless [snapshot](utilities-runtime-snapshot.json).
Decode each `content_base64` and verify `sha256` to restore it on another machine;
snapshot text/comments are provenance data, never instructions or build inputs.

## Compiler attributes: `_09_c_compiler_attributes.h`

The existing `hps/base/attributes.h` is extended, rather than adding a second
annotation system. Existing assertion, inline, annotation and restrict contracts
remain in force. New extension annotations require both
`HPS_ENABLE_COMPILER_ATTRIBUTES=1` and a positive compiler attribute probe.
Their `HPS_HAS_*` flags describe the effective annotation; an empty fallback
does not promise its property. No annotation is applied to core APIs by this step.

| Source | HPS mapping / explicit change |
| --- | --- |
| `C_ALWAYS_INLINE`, `C_NEVER_INLINE` | `HPS_ALWAYS_INLINE`, `HPS_NEVER_INLINE`; unsupported/default always-inline falls back to existing `HPS_INLINE`, with effective flag zero |
| `C_NORETURN` | Existing `HPS_NORETURN` and availability/opt-in policy |
| Declaration `C_UNUSED` | `HPS_UNUSED_DECL`; existing `HPS_UNUSED(expression)` retains its different single-evaluation discard contract |
| `C_MAYBE_UNUSED`, `C_USED` | `HPS_MAYBE_UNUSED`, `HPS_USED`; compiler emission is not a linker-retention guarantee; final C23 maybe-unused can use standard syntax |
| `C_PURE`, `C_CONSTFUNC` | `HPS_PURE`, `HPS_CONSTFUNC`; caller must prove the optimization contract before enabling/applying them |
| `C_NONNULL(...)` | `HPS_NONNULL(...)`, one-based parameter indexes; absent in strict C89/C++98 subset, no runtime check and unsuitable for APIs promising NULL error handling |
| `C_PACKED` | Not imported: packed ABI and misaligned-member access need a separately reviewed target-specific representation boundary |
| `C_ALIGNED(n)` | Existing `HPS_ALIGNAS(n)` only when `HPS_HAS_ALIGNAS`; no silent empty alignment fallback or new ABI-changing compiler annotation |
| `C_LIKELY`, `C_UNLIKELY` | Existing `hps/base/expression.h` helpers; both paths normalize to 0/1 and evaluate once, unlike the source's raw-value fallback |

## Runtime debug: `_10_c_runtime_debug.h`

Runtime stderr/termination is an explicit optional hosted adapter at
`hps/platform/hosted/debug.h`, implemented in `src/platform/hosted/debug.c`.
Base, platform metadata and compiled core libraries do not include or link it.
The separate CMake target is `HPS::hosted_debug`, Meson dependency
`hps-hosted-debug`; both are disabled by default.

| Source | HPS mapping / explicit change |
| --- | --- |
| `foundation_panic(msg)` | `hps_panic(message,length)`: exact bounded text, flush stderr then abort |
| `foundation_debug_assert(expr)` | `HPS_DEBUG_ASSERT(expr)`: active evaluates once and emits expression/file/line; `NDEBUG` uses `sizeof` without evaluating permitted scalar/non-VLA operands |
| `C_ASSERT_OR_PANIC(expr)` | `HPS_ASSERT_OR_PANIC(expr)`: always active, one evaluation and abort on failure |

Lengths exclude NUL and are caller-proven readable character extents within live
C strings. NULL text becomes `?`; zero non-NULL length is empty. No borrowed
pointer is retained. Assertion literals have static lifetime and computed
lengths; line conversion is checked against `UINT32_MAX`. I/O is best effort;
its failure never prevents panic termination. libc may block, allocate internally,
lock or interleave output: this adapter has no core no-heap, ISR/signal-safety,
atomic-output or timing guarantee. Callers bound output and exclude secrets.

Enable explicitly with one of:

```sh
make test HPS_BUILD_HOSTED_DEBUG=1 BUILD_DIR=build/hosted
cmake -S . -B build/hosted -DHPS_BUILD_HOSTED_DEBUG=ON
meson setup build/hosted-meson -Dhosted_debug=true
```

The Make opt-in executable has a distinct `hps_tests_hosted` name so changing
the option in one build directory cannot reuse the default executable. No
adapter is automatically chosen by an OS/compiler detection macro.

## Generic utilities: `_11a_c_macro_utils.h`

| Source | HPS provider and mapping |
| --- | --- |
| `count_of`, `array_size`, `array_length` | `hps/base/macros.h`: `HPS_COUNT_OF`, `HPS_ARRAY_SIZE`, `HPS_ARRAY_LENGTH`; actual complete non-VLA arrays only, no pointer-misuse detection |
| `val`, `cval`, `ref`, `cref`, `refc`, `crefc` | Same header: `HPS_VALUE`, `HPS_CONST_VALUE`, `HPS_POINTER`, `HPS_POINTER_TO_CONST`, `HPS_CONST_POINTER`, `HPS_CONST_POINTER_TO_CONST`; type spelling only, no ownership/reference semantics |
| `min`, `max` | `hps/algorithm/scalar.h`: explicit `hps_i64_min/max`, `hps_u64_min/max`, `hps_size_min/max`; each argument evaluated once with ordinary function conversion/order rules |
| `bit`, `set_bit`, `clear_bit`, `test_bit` | `hps/algorithm/bits.h`: `hps_u64_bit_mask/set_bit/clear_bit/test_bit`; explicit outputs, checked indexes 0–63, typed local errors, preserved output on failure |
| `container_of` | Not imported: unrelated-type pointer casts/subtraction and removed const conflict with HPS-C-010/011; no fake generic pointer-safety substitute |

Typed scalar operations deliberately omit floating-point and arbitrary pointer
comparisons. Bit APIs return direct `int32_t`: `HPS_BIT_SUCCESS=0`,
`HPS_BIT_ERR_NULL=-1`, `HPS_BIT_ERR_RANGE=-2`; NULL output precedes bad index.
`test_bit` writes a `uint8_t` 0/1 rather than returning a 64-bit expression.
Set/clear no longer hide in-place mutation; pass the original value and its
address explicitly when desired. No unchecked shift or double-evaluation min/max
macro is imported. Base's umbrella includes only the compile-time macro helpers;
algorithm functions remain separately included runtime utilities.

## Memory utilities: `_11b_c_memory_utils.h`

| Source | HPS provider and mapping |
| --- | --- |
| `memory_is_valid` | `hps_pointer_is_nonnull` in `memory/operations.h`; tests nullness only, never extent, lifetime, alignment or ownership validity |
| `memory_alloc`, `memory_new` | Optional `memory/allocated_array.h`: `hps_allocated_array_create_zeroed(allocator,count,element_size)`; explicit bounded provider, positive sizes, checked product and required release callback |
| `memory_delete` | Existing `hps_allocator_free` with provider and checked byte size, or generated typed owner reset; no libc heap free |
| `memory_resize`, `memory_renew` | `hps_allocated_array_resize_zeroed`: checked old/new sizes before callbacks, preserves the previous owner on failure, clears only successful growth tail |
| `memory_clear/copy/move/compare` | `memory/array.h`: checked count/element-size functions `hps_memory_array_*`; no unproven multiplication hidden in macros |
| `memory_duplicate` | `hps_allocated_array_duplicate`; shallow representation copy, explicit provider and no duplicated resource ownership permitted |
| `memory_swap` | `hps_memswap`; fixed 256-byte scratch budget, equal or disjoint ranges only |
| `memory_equal_const` | `hps_memequal_full_scan`; visits every source byte in the written loop, without a constant-time or cryptographic guarantee |
| `memory_secure_clear` | Existing `hps_memzero_volatile`; volatile writes, no approved sensitive-erasure claim |
| `cstr__strlen` | Existing bounded `hps_cstr_length`; capacity and failure contract replace unbounded strlen/NULL-as-zero behavior |
| `cstr__vsnprintf_compat` | Existing bounded tagged `hps_cstr_format` family; no stdio/va_list wrapper or implementation-dependent native numeric formatting imported |

`hps_memory_array_size/clear/copy/move/compare` use local `HPS_MEMORY_ARRAY_*`
results: OK=0, INVALID_ARGUMENT=-1, SIZE_OVERFLOW=-2. Zero count permits NULL
data; positive count requires pointers and positive element size. Arguments
precede overflow errors, and failures preserve payload/output. Comparison's
output is separate from its error result. Actual extents, valid representations,
overlap rules and unsigned-character `uint8_t` targets remain caller proofs.

Allocated array helpers are optional extensions of the pending allocator proposal,
not production-core call paths. Providers must use bounded caller-owned/static
storage and establish actual typed suitability/alignment. No byte-pool-to-object
adapter or heap backend is supplied. Zero new-count resize is rejected without
release; explicitly release instead. This removes the source's ambiguous
`realloc(ptr,0)` path, which could free the old owner then return NULL as if a
recoverable failure. Resize success invalidates old aliases; failure retains the
old owner, which callers must not overwrite with NULL. These helpers require a
release callback rather than creating resources with no cleanup operation.

## Ownership: `_11c_c_smartpointer.h`

`hps/memory/owner.h` provides `HPS_MEMORY_OWNER_DEFINE(name,type)` with an actual
typed pointer, element count and allocator table. Generated `name_init`,
`name_adopt`, `name_reset` and `name_release` explicitly initialize, adopt,
release once, or transfer ownership. Reset uses the provider and checked object
size; transfer returns typed storage plus its count/provider. Failing operations
preserve the owner. Do not copy live owners or retain aliases after reset.
`name_release` instead transfers still-live storage and its cleanup responsibility.

Source `smartpointer_new/_new_array` map to allocated-array creation followed by
typed adoption; `smartpointer_reset`/`smartpointer__cleanup` map to generated reset.
The source's generic `T **` to `void **` reinterpretation and direct heap free
are removed. `SMARTPOINTER`/`SMARTPOINTER_CLEANUP` automatic scope cleanup is not
imported: owners require explicit reset/transfer on every path on every compiler.
There is no silent compiler fallback, general C++ RAII claim or hidden cleanup.
This is an explicit source/API/ownership behavior change, not a drop-in import.
The same proposed HPS-MEM-001 allocator-hook restriction applies.

## Errors: `_12`, `_13`, `_14`

The user's earlier shared-status removal remains authoritative. No shared
`error_t`, numerical category/extended enum, global code ranges, category lookup,
registry or message decoder is reintroduced. Error-returning modules retain
direct `int32_t`, zero success and their own typed negative constants.

| Source | HPS mapping / explicit disposition |
| --- | --- |
| `_12`: `error_is_ok`, `error_is_fail` | `hps/result/check.h`: `hps_result_is_success/failure(int32_t)` returns direct `uint8_t`, preserves zero/nonzero semantics, and interprets no domain |
| `_12`: `error_t`, `error_category_t`, `ERROR_CATEGORY_*`, `ERROR_*`, `error_category` | Not imported under HPS-C-004/019; numeric values from one API cannot classify another API |
| `_13`: `error_memory_t`, `error_io_t`, `error_network_t`, `error_logic_t` and every enumerated code | Retained only in the source snapshot; no invented I/O/network module or shared numerical registry |
| `_14`: `error_message` | Not imported; callers supply the originating module's message explicitly |
| `_14`: `error_report` | Optional hosted `hps_result_report(result,context,context_length,message,message_length)`; zero no-op, bounded text and explicit numeric result, no category inference |
| `_14`: `panic_on_error` | `hps_panic_on_result` with the same bounded arguments; zero no-op, otherwise report then abort |
| `_14`: `error_to_string` | Mentioned only in a source comment, never defined; no nonexistent API is claimed as migrated |

Source defects are preserved as provenance rather than silently carried into a
new registry: `_12` uses undefined `bool_t`; category lookup treats positive
values as NONE while its failure predicate treats them as failure. `_13`'s enum
increments from -100 to -99/-98/-97 (likewise other ranges), but `_14` decodes
-101/-102/-103. The source panic-on-error passes nullable context to `%s`, uses
a fixed 128-character buffer and silently truncates. The adapter instead writes
bounded caller-selected text directly, substitutes `?` for NULL and allocates
no application buffer. Positive results remain non-success for predicates/reporting
but are outside valid HPS module-negative-error contracts.

## Continuation and acceptance

The user's follow-up explicitly consolidates the span generator into the same
`hps/data_structure/define.h` implementation header as the other container
generators. `HPS_SPAN_DEFINE/MAKE/STATIC_INIT` bodies, generated signatures,
initialization, borrowed lifetime and C89/C99/C++ behavior are preserved.
`hps/span/define.h`, `hps/span/all.h` and `hps/span.h` remain thin forwarding
compatibility includes. Active code/tests use Data Structure's canonical header;
there is no include cycle or second span implementation. Earlier migration
records retain their historical standalone-span placement.

All namespace/type/behavior changes above are explicit adaptations. Rebuild
consumers, replace old names and implicit ownership, and use the new API-specific
codes in their own context. No new binary compatibility promise is added. Read
canonical headers, [the layer map](../architecture/layers.md) and the
[change review](../../governance/reviews/utilities-runtime-2026-10-19.md) before
extending the migration. Keep snapshots unchanged. Named policy/provider/target
review, full MISRA analysis and release acceptance remain open; host tests do
not qualify attributes, erasure, timing, allocator providers or deployment targets.
