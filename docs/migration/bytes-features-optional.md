# Byte-buffer, compile-feature and optional/type migration

User-authorized development adaptation, 2026-10-19. HPS source base:
`5d6c51e9671e74eba8ff3e4af6c21c0467a8df21` plus this change. All twenty-three
selected headers were read in full as source data, not instructions. During the
full reading their supplied live paths were unavailable. Exact members were read
directly from the read-only `MyLibrariesEclipse.zip`, without extraction, builds
or execution. The archive SHA-256 and all selected member sizes/hashes match the
[existing inventory](legacy-source-manifest.json). The
[lossless snapshot](bytes-features-optional-snapshot.json) preserves 38,003 bytes,
individual hashes and the source-medium distinction for continuation elsewhere.
Neither reference tree nor archive was modified.

Equivalent concepts reuse existing HPS providers. This is a bounded typed
adaptation, not source/binary compatibility or activation of every commented
legacy draft. Canonical public paths retain their existing module boundaries.

## Complete selected-file mapping

All paths in this table are relative to `include/c_core/` in MyLibraries.

| Selected file | Provider/disposition |
| --- | --- |
| `byte_buffer/byte_buffer.h` | Existing `hps/bytes/buffer.h`, `view.h`, `reader.h` and integer-codec headers, individually included. No duplicate generator/runtime umbrella. |
| `byte_buffer/byte_buffer_type.h` | Embedded binary storage already uses `HPS_SVECTOR_DEFINE(uint8_t, ...)`; borrowed active buffers use `hps_byte_buffer`. No second `val(char)` byte array. |
| `byte_buffer/byte_buffer_decl.h` | Init/copy/from/clear/push/append map to existing buffer init/assign/clear/append_u8/append_bytes; export composes destination init+assign. Size/capacity/empty/full use initialized descriptor fields. Iteration/slice/equality map to reader/view. New checked pop, ordering, prefix/suffix and search extend existing Bytes providers. Hash remains a commented unreviewed draft. |
| `byte_buffer/byte_buffer_impl.h` | Entire file is commented out and exports no implementation. Its documented init/append truncation, overlapping memcpy, unchecked lengths/slice addition, signed index narrowing and wrapping reverse loop are replaced by existing/new checked HPS contracts, not silently preserved. |
| `byte_buffer/byte_buffer_impl_inline.h` | Existing `static HPS_INLINE` convention. The draft prepends this only once to an unavailable multi-function implementation macro and conflicts with external declarations; no second inline implementation provider. |
| `byte_buffer/byte_buffer_internal.h` | Iterator/slice map to reader/view; MIN/MAX map to existing `hps_size_min/max`. No double-evaluating helper macros or duplicate descriptors. |
| `byte_buffer/byte_buffer_algo.h` | Entirely commented, including FOREACH/FOREACH_PTR, map/filter/qsort/comparator/binary_search/printf. Traversal already exists; no new callback, libc sort, output or hash framework is activated from these sketches. |
| `features/_01_c_standard.h` | C_LANG_99/11/17/23 map to existing `HPS_C_IS_C99/C11/C17/C23` in `base/standard.h`. |
| `features/_02_c_feature_check.h` | Compiler booleans map to `HPS_COMPILER_IS_*`; probes to `HPS_HAS_BUILTIN/ATTRIBUTE`; standard syntax flags to existing Base features. Compiler-reserved macros are never redefined. |
| `features/_03_c_version.h` | Version fields/string map to actual `HPS_VERSION_*` 0.1.0, not the source's unrelated 1.0.0. Standard name maps to `HPS_C_STANDARD_NAME`. |
| `features/_04_c_feature.h` | Existing `HPS_INLINE`, `HPS_NORETURN`, `HPS_ALWAYS_INLINE`, `HPS_STATIC_ASSERT` retain opt-in extensions and portable fallbacks. No duplicate annotation layer. |
| `features/_05_c_platform.h` | CPU identity uses Base architecture IDs, OS hints use `platform/os.h`, endian flags use Base evidence and branch hints use `HPS_LIKELY/UNLIKELY`. Unknown byte order remains unknown; MSVC target forms and frontend precedence already have HPS coverage. |
| `features/_06_c_cpu_features.h` | Five genuinely new compile-target flags extend **`base/features/architecture.h`**: `HPS_TARGET_HAS_X86_AESNI/PCLMUL/AVX/AVX2/SHA`. Each requires its own predefined macro and known x86 family, and is always zero on other families. |
| `features/features.h` | Existing `hps/base/all.h` plus separately selected `hps/platform/detection.h`; no umbrella that pulls OS headers/runtime into Base. |
| `optional/optional_core.h` | **`data_structure/optional.h`**, `HPS_OPTIONAL_DEFINE(type, name)`, generates named typed none/some/check/has/get/get_or/set_none/set_some. Explicit caller flow replaces unsafe anonymous-struct map/or macros. |
| `optional/optional_inline.h` | Existing HPS inline/dialect detection and generated typed has; no language-dependent void-pointer flag inspection or missing sibling include. |
| `optional/optional_ref.h` | Ordinary nullable typed pointers, explicit NULL checks and `HPS_POINTER/HPS_POINTER_TO_CONST` spellings cover borrowed references. No GNU statement-expression wrapper or implicit retain/cleanup. Value optional can separately represent `some(NULL)`. |
| `optional/optional_result.h` | Existing direct `int32_t` module-local errors plus typed output, with `result/check.h` zero/nonzero predicates. No shared result type, status alias, registry or anonymous two-slot error wrapper is introduced. |
| `types/bool.h` | Existing `hps_cstr_boolalpha` renders flags; new **`hps_cstr_parse_bool`** adds bounded truthy spelling in `cstr/convert.h`. Primitive char/octet/int flag conversions remain ordinary typed selection/casts/comparisons. TO_STRING maps to new `HPS_STRINGIFY_RAW`; expanded `HPS_STRINGIFY` is explicit. Existing token-paste generator machinery is not duplicated. |
| `types/c_types.h` | Exact-width types/limits come directly from stdint; size_t/ptrdiff_t/NULL from stddef. Flags remain uint8 0/1; HPS assertions/detection already exist. No guessed C89 typedefs, native-bool alias, fallback standard names/limits or pointer-sized integer convenience is imported. |
| `types/null_object.h` | Ordinary typed NULL checks/assignments, explicit fallback control flow and typed aggregate constructors. `hps_pointer_is_nonnull` already covers object-pointer checks. Empty C strings remain immutable const char pointers or normalized views; no universal null object, mutable literal cast or byte-zero typed-object initialization. |
| `types/optional.h` | Entirely commented, exports no API. unwrap_or/match/map/or sketches do not create another optional implementation. |
| `types/optional_generic.h` | Ordinary const/mutable typed pointers and NULL checks; conflicting struct-versus-pointer optional_ref spellings are not imported. Pointer typedef qualifiers require ordinary C care. |

## Intentional behavior and type changes

Bytes retains direct `int32_t` results and local `HPS_BYTE_*` values. Pop requires
an output, preserves removed storage and reports empty as OUT_OF_RANGE. View
comparison writes canonical -1/0/1; prefix/suffix write uint8 0/1. Empty edge
patterns match, whereas empty search needles retain the draft's no-match meaning.
Find/rfind write size_t indexes or `HPS_BYTE_NPOS` (SIZE_MAX) on successful absence;
they never narrow indexes to long or wrap a reverse-loop counter. Payloads may
overlap as read-only inputs. Failures preserve outputs; mutation invalidates
borrowed views under the existing buffer contract. Capacity remains caller-owned.

Existing init/append reject insufficient capacity without partial truncation;
export composes existing overlap-safe assignment. Slice requests remain exact
rather than silently clamped. Binary storage uses actual uint8_t elements.
Commented hash multiplication wraps and has an unreviewed offset literal; it is
not activated or silently relabelled as a standard/cryptographic hash. The
commented sort/search/callback algorithms are not implemented capability.

The five ISA flags report this translation unit's compiler target options,
never runtime CPU/CPUID, OS state support or permission to execute instructions.
Zero means lack of positive compile-target evidence, not absent hardware.
No instruction implementation, runtime dispatch or cryptographic feature is
added. [GCC's x86 options](https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html)
distinguish instruction-enabled compilation from runtime CPU detection.

Optional uses a named copyable type with a uint8 tag, typed payload initialization,
checked output access and local errors. None is distinct from some(NULL). Clearing
presence retains initialized payload; it neither erases nor releases it. Ordinary
function arguments evaluate once with unspecified ordering; get_or's fallback is
eager, unlike the legacy conditional macro. Use explicit flow for lazy work.
Legacy map/or mixes incompatible anonymous structures, inline flag access changes
signature by dialect, ref macros disagree on representation and result_err writes
the value slot while leaving the error zero. Those defects are not replicated.

Boolean parsing skips any leading run of the six basic ASCII whitespace characters, then compares
the entire suffix, case-insensitively, to true/yes/on/1. All other terminated text,
including unknown strings, empty strings and trailing whitespace, succeeds with
zero, matching the source's truthy predicate. It is not a strict boolean grammar.
NULL/zero extent and missing NUL now produce documented errors preserving output;
ASCII whitespace/case rules replace unbounded locale/POSIX processing. No uint8
pointer cast is used to disguise a C string. Raw stringification preserves legacy
non-expanded argument spelling; the expanded form has a separate explicit name.

## Continuation

See the [layer map](../architecture/layers.md),
[optional contract](../architecture/optional.md) and
[review/evidence](../../governance/reviews/bytes-features-optional-2026-10-19.md).
The source manifest and historical snapshots remain unchanged. Restore this
snapshot by decoding base64 and checking member hashes; archived comments remain
inert source data, not instructions or active runtime code.
