# Detection environment decomposition — 2026-10-19

The user selected all nine headers under MyLibraries
`include/c_core/c_features/c_layer00_detection_environment/` for careful
decomposition, HPS naming and layer placement. Each complete source was read.
All nine still match the SHA-256 values in the existing
[read-only inventory](legacy-sources.md). No reference file, build, configuration
or Git state was changed; no reference source was compiled or executed there.

The [lossless snapshot](detection-environment-snapshot.json) stores original
relative paths, sizes, hashes and base64-encoded bytes, preserving CRLF and the
final newline exactly. To restore a source on another machine, decode its
`content_base64` and verify `sha256`. This is inert provenance data, never a build
input or instruction source. Carry it with HPS; the original absolute attachment
or Downloads path is not required for continuation.

## Placement of every source section

| Reference file / section | Canonical HPS provider | Public mapping / disposition |
| --- | --- | --- |
| `_01_c_std_detect.h`: revision constants | `hps/base/language.h` | `C_STD_VERSION` -> `HPS_C_STANDARD_VERSION`; `C_STD_C89/C90/C95/C99/C11/C17/C23` -> `HPS_C_STANDARD_*` |
| `_01`: at-least-revision predicates | `hps/base/standard.h` | `C_HAS_C99/C11/C17/C23` -> existing `HPS_C_IS_C99/C11/C17/C23`; reuse existing minimum-version policy |
| `_02_c_compiler_detect.h`: identities | `hps/base/compiler.h` | Source boolean `C_COMPILER_*` -> `HPS_COMPILER_IS_*`, while `HPS_COMPILER_*` constants are IDs; ICC maps to INTEL_CLASSIC, TINYC to TINYCC |
| `_02`: compiler versions and probes | `hps/base/compiler.h` | `C_COMPILER_VERSION_MAJOR/MINOR/PATCH` -> existing `HPS_COMPILER_VERSION_*`; raw vendor release is separately `HPS_COMPILER_VENDOR_VERSION` |
| `_03_c_platform_detect.h`: OS families | `hps/platform/os.h` | `C_PLATFORM_WINDOWS/LINUX/APPLE/IOS/MACOS/ANDROID/UNIX/BSD` -> `HPS_PLATFORM_IS_*`; flags may overlap |
| `_03`: POSIX/embedded hints | `hps/platform/os.h`, `hps/base/language.h` | `C_PLATFORM_POSIX` -> `HPS_PLATFORM_IS_POSIX_FAMILY`; no `C_PLATFORM_EMBEDDED` alias because unknown OS is not embedded evidence |
| `_03`: CPU identity | `hps/base/architecture/id.h` | Source `C_ARCH_*` predicates -> comparisons of `HPS_ARCHITECTURE` to `HPS_ARCH_*` IDs; ARM64 maps to AARCH64, RISC-V to RISCV32/RISCV64/RISCV |
| `_03`: 32/64-bit classification | `hps/base/architecture/pointer.h` | `C_ARCH_32BIT/64BIT` -> comparisons of `HPS_POINTER_WIDTH` to `HPS_POINTER_WIDTH_32/64`; pointer size is independent of CPU family |
| `_04_c_lang_features.h`: standard syntax | `hps/base/features/language.h` | `C_HAS_STATIC_ASSERT/THREAD_LOCAL/COMPOUND_LITERALS/DESIGNATED_INITIALIZERS` -> corresponding standard `HPS_HAS_*` flags |
| `_04`: GNU syntax | `hps/base/features/compiler.h` | Statement expressions/typeof/compound literals/designated initializers/thread-local extensions use separate `HPS_HAS_EXTENSION_*` flags |
| `_04`: builtins | `hps/base/features/compiler.h` | `C_HAS_BUILTIN_EXPECT/TYPES_COMPATIBLE_P/CHOOSE_EXPR` -> `HPS_HAS_BUILTIN_*`; C-only builtins are zero in C++ |
| `_04`: lambda prerequisite combination | `hps/base/features/compiler.h` | `C_HAS_LAMBDA_MACRO_SUPPORT` -> `HPS_HAS_LAMBDA_MACRO_SUPPORT`; informational prerequisite combination only, no lambda implementation |
| `_04`: declaration wrappers | `hps/base/attributes.h`, `hps/base/expression.h` | `C_STATIC_ASSERT` -> existing `HPS_STATIC_ASSERT`; `C_THREAD_LOCAL` -> existing standard `HPS_THREAD_LOCAL` when its availability flag is true; branch hints reuse the new builtin probe |
| `_05_c_endian_detect.h` | `hps/base/architecture/endian.h`, `features/architecture.h` | Source `C_ENDIAN_LITTLE/BIG/UNKNOWN` booleans -> comparisons of `HPS_ENDIANNESS` with the existing `HPS_ENDIAN_*` IDs; convenience flags remain `HPS_IS_LITTLE_ENDIAN`, `HPS_IS_BIG_ENDIAN`, `HPS_ENDIANNESS_IS_KNOWN` |
| `_06_c_libc_detect.h` | `hps/platform/libc.h` | `C_LIBC_*` booleans -> `HPS_LIBC_IS_*`; separate detected/selected IDs and validated explicit family hint, with distinct UCRT |
| `_07_c_posix_detect.h` | `hps/platform/posix.h` | `C_POSIX_AVAILABLE`, `C_POSIX_VERSION`, `C_XOPEN_VERSION` -> same names with `HPS_` prefix; family hint remains separate |
| `_08_c_bool_type.h` | Existing exact-width flag policy | No bool typedef, `<stdbool.h>` re-export or bool/true/false/reserved-name macro is imported. HPS-C-001/004 require direct `uint8_t` flags; this is an explicit omitted legacy compatibility API, not a hidden bool replacement |
| `c_layer_00_detection.h`: aggregation | `hps/base/all.h` and optional `hps/platform/detection.h` | Pure foundation and platform/library metadata remain separate; no monolithic umbrella imports SDK/OS headers or boolean aliases |

## Explicit behavior changes and retained HPS contracts

This is an adaptation to the existing HPS API, not a mechanical prefix-only
rename. Macros use `HPS_`; ordinary public C names use `hps_`. The source's
`C_STD_C89/C90 == 0L` maps to the existing HPS `198900L` sentinel; the additive
`HPS_C_STANDARD_C90` aliases C89. Preserve this distinction when comparing raw
values. C++ reports its own language version and does not acquire C-only features.
C23 requires final `202311L`; current C2x draft modes do not prove C23 support.

Compiler IDs 0–4 remain unchanged. Intel Classic, Intel LLVM, IAR, ARMCC and TinyCC
append IDs 5–9. Specific identities precede their compatibility macros. Clang,
Armclang and Intel LLVM normalized components describe the Clang frontend when
visible; vendor-only Armclang/Intel LLVM retain zero components and their raw
vendor release. MSVC keeps its previous patch value zero; `_MSC_FULL_VER` is
reported separately. Intel Classic retains the source major/minor division;
ARMCC's third field is its documented build number. IAR encodings vary by family,
so normalized components remain zero rather than guessing one format; raw
`__VER__` is retained. TinyCC also retains raw identity/version with unknown
normalized components. These detections do not qualify those compilers.

Architecture IDs 0–6 remain unchanged; MIPS=7, PPC=8, identified RISC-V with
unknown/unsupported XLEN=9. Missing XLEN no longer discards known RISC-V family.
An unrecognized pointer size is never forced to 32-bit, and a 64-bit CPU does not
force 64-bit pointers. Explicit unsupported/mixed byte-order metadata stays
unknown before any Windows/x86 fallback. The source's ARM/RISC-V little-endian
guess is not imported; those families need actual order evidence.

Standard feature flags follow the C dialect. GNU syntax flags additionally
require recognized compatible frontends and GNU mode, apart from the explicit
MSVC thread-storage extension hint. Probes do not enable extensions or approve
policy exceptions. `HPS_HAS_TYPEOF` describes final C23 standard syntax;
`HPS_HAS_EXTENSION_TYPEOF` describes GNU syntax. Existing static-assert fallback,
optional compiler annotations, restrict configuration and standard thread-local
declaration API are preserved. No empty fallback claims unsupported thread storage.

The source guesses musl from missing Linux markers, a Microsoft libc from MSVC,
embedded from unknown OS, POSIX availability from an OS family, and macOS from
any Apple target not identified as iPhone. HPS keeps these cases unknown without
specific evidence. Apple SDK, libc and POSIX implementation macros must already
be visible before the first guarded detection inclusion, or libc may use the
validated `HPS_CONFIG_LIBC` hint. No feature-test macro is changed. See the
[platform contract](../architecture/platform.md) for include-order/configuration
details and the distinction between unknown and unsupported.

## Public header folders

| Previous path | Canonical path |
| --- | --- |
| `hps/base.h` | `hps/base/all.h` |
| `hps/cstr.h` | `hps/cstr/all.h` |
| `hps/string.h` | `hps/string/all.h` |
| `hps/span.h` | `hps/span/define.h`, or umbrella `hps/span/all.h` |
| `hps/data_structure.h`, `hps/hps_data_structure.h` | `hps/data_structure/define.h`, or umbrella `hps/data_structure/all.h` |
| `hps/hps_allocator.h` | `hps/memory/allocator.h` (the prior draft API changes remain documented in [memory migration](memory.md)) |

All seven previous root paths remain one-include forwarding headers. The active
code/tests use canonical folder paths. Generator bodies/layouts, function names,
ownership, capacities and error codes are unchanged by these moves. Installed
headers are generated from this source tree; older `build/*-install` snapshots
are historical artifacts, not authoritative include paths for this revision.
No old installation was edited in place. Rebuild/reinstall consumers to receive
the new paths and detection semantics; no stable binary ABI claim is added.

## Primary reference checks and continuation

Vendor macro interpretation was checked against primary material:
[GCC predefined macros](https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html),
[GCC C-only builtins](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html),
[GNU statement expressions](https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html),
[Clang probes](https://clang.llvm.org/docs/LanguageExtensions.html),
[Intel identities](https://www.intel.com/content/www/us/en/docs/dpcpp-cpp-compiler/developer-guide-reference/2025-0/use-predefined-macros-to-specify-intel-compilers.html),
[ARMCC version encoding](https://documentation-service.arm.com/static/5eb946b50f1c1e0dae6ee21e),
[IAR S08 reference](https://updates.iar.com/filestore/standard/001/001/984/s08/doc/EWS08_CompilerReference.pdf),
and [TinyCC predefined identity](https://bellard.org/tcc/tcc-doc.html).
Different IAR-family encodings are deliberately not normalized without a scoped
target contract. Primary documentation and synthetic branch tests are not native
execution evidence for these vendor compilers.

Read canonical headers, [the layer map](../architecture/layers.md) and the
[change review](../../governance/reviews/detection-layers-2026-10-19.md) before
extending this migration. Keep the lossless source snapshot unchanged. Target
vendor/compiler/SDK/libc evidence, full MISRA analysis, policy dispositions and
named release review remain open; no compliance, timing or production claim
follows from detection macros or host tests.
