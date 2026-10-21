# Base layer contract

The base layer provides compile-time information and portable C declarations.
It also provides the standard type compatibility headers and fixed-work Boolean
conversion helpers described below.
Industry policy, protocols, memory management, threading implementations, device
access, logging and domain data models belong in higher layers. All base headers
can be included independently and repeatedly; none defines executable runtime
state. The API starts at development version 0.1.0 and is not a stable ABI.
Use `hps/base.h` for the module umbrella. It directly includes the Base providers
and is the only public header at the `hps/` root. Leaf declarations live beneath
`hps/base/`; nested architecture/features umbrellas are
`hps/base/architecture/architecture.h` and `hps/base/features/features.h`.

## Language and standards

`hps_language.h` exposes `HPS_LANGUAGE_IS_C`, `HPS_LANGUAGE_IS_CPP`, the raw
`HPS_C_STANDARD_VERSION`, C89/C95/C99/C11/C17/C23 constants, and the raw C++
version for detection only. C++ is not advertised as a C dialect. The complete
base API targets C99 or newer. C++ identification is supported; public external
C APIs carry guarded C linkage. Full C++/ABI consumption has not been qualified. Detection, version and declaration headers can be used in C89.

`hps_standard.h` preserves the C99 minimum policy. `HPS_REQUIRE_C99` enables an
explicit compile-time rejection in this header. `HPS_C_IS_C99` through
`HPS_C_IS_C23` mean **at least** that revision. `HPS_C_LEGACY` is true only for
pre-C99 C. `HPS_C_STANDARD_NAME` is a diagnostic string. C23 is recognized only
at `__STDC_VERSION__ >= 202311L`; older C2x draft modes are not treated as full
C23 implementations.

`HPS_HOSTED_IS_KNOWN` distinguishes a known execution environment from absent
information. `HPS_IS_HOSTED` and `HPS_IS_FREESTANDING` are both false if the
compiler does not supply `__STDC_HOSTED__`. Freestanding use assumes the target
provides `<limits.h>` and either native `<stddef.h>` or the compiler metadata
required by the explicit size-provider replacement below.

## Standard type providers

Include `hps/base/hps_bool.h`, `hps/base/hps_stdint.h` or
`hps/base/hps_stddef.h` directly when their declarations are needed. The Base
umbrella includes all three. They expose standard `bool`/`true`/`false`, integer
types/constants/limits and `size_t`/`ptrdiff_t`/`wchar_t`/`NULL`/`offsetof`;
there is no HPS type alias. Core and public headers use these providers instead
of directly selecting the native standard headers. Detection-only headers do not
include the Boolean provider or inject Boolean macros.

The integer provider prefers native `<stdint.h>` when `__has_include` confirms
it, including legacy C/C++ modes. Without that probe, C99+, C++11+ and
MSVC 2010+ select the native header. `HPS_STDINT_USE_FALLBACK=0` explicitly
selects native; `=1` requires compiler type/limit/constant metadata for replacement.
HPS requires eight-bit bytes and exact signed/unsigned 8/16/32/64-bit integers;
missing or inconsistent widths are rejected. Optional pointer integers are
published only when both compiler types/limits exist. Least/fast/max families
are standard compatibility exports, not new permitted core storage types.

The size provider defaults to native `<stddef.h>`, available since C89.
`HPS_STDDEF_USE_FALLBACK=1` selects a replacement using compiler size/difference/
wide-character metadata and builtin `offsetof`; C++ retains its keyword
`wchar_t`. Unsupported metadata/builtin support rejects the configuration.
Both switches accept only 0 or 1 and must be consistent across the target.
Do not mix either replacement with another provider of its types, including
indirect native includes. Compiler fast-width choices can differ from libc's
ABI. Replacement stddef covers the C89/C++98 surface, not `max_align_t` or later
additions. Native providers inherit the library surface for the chosen mode.

The Boolean provider leaves C++ and final C23 keywords intact, includes stdbool
in C99 through C17/draft C2x, and supplies a `uint8_t` macro fallback before C99.
The byte fallback does not normalize arbitrary assignments and does not promise
a native Boolean ABI. `hps_bool_from_u8`/`hps_bool_from_i32` normalize zero/nonzero;
`to_u8`/`to_i32` return exactly zero/one. `to_cstr` borrows immutable static
`"true"`/`"false"` literals. Numeric character helpers accept/return `uint8_t`
execution-character codes, recognizing only `'0'` and `'1'` for strict parsing.
`hps_bool_from_char_checked` returns false for NULL without access, clears a
valid output to false on invalid input, and returns true after valid conversion.
All helpers perform fixed work without allocation, retained pointers or mutable
global state. The full framework implementation minimum remains C99.

See the [source/API mapping](../migration/standard-types.md) and
[verification and policy scope](../../governance/reviews/standard-types-2026-10-20.md).

## Compiler and capabilities

`hps_compiler.h` checks Armclang, Intel LLVM, Intel Classic, IAR, ARMCC and TinyCC
before generic Clang, MSVC and GCC so compatibility macros do not hide more
specific identities. Existing IDs 0–4 remain stable; added identities use 5–9.
Version fields describe the selected frontend or documented vendor encoding.
`HPS_COMPILER_VENDOR_VERSION` separately retains a visible raw vendor release.
IAR/TinyCC normalized fields remain unknown rather than guessing one format.
Unknown compilers have identifier and
version fields equal to zero. Known GCC derivatives excluded from GCC detection
fall back to unknown unless recognized as another frontend. Compiler detection
is not a claim of certification or exhaustive vendor coverage.

`HPS_HAS_ATTRIBUTE(name)`, `HPS_HAS_BUILTIN(name)` and `HPS_HAS_FEATURE(name)`
delegate to compiler probes when present and return zero otherwise. These are
preprocessor probes. `HPS_HAS_GNU_EXTENSIONS` describes GNU compatibility mode;
use individual probes when a particular extension is required.

`HPS_HAS_COMPOUND_LITERALS` and `HPS_HAS_DESIGNATED_INITIALIZERS` describe C99
syntax; `HPS_HAS_TYPEOF` describes final C23 syntax. Separate
`HPS_HAS_EXTENSION_*` flags describe known GNU-mode syntax (and the explicit
MSVC thread-storage extension). `HPS_HAS_BUILTIN_*` flags distinguish individual
builtins; type compatibility/choose-expression probes are C-only. These flags
enable no extension, declaration fallback or policy exception.

`features/features.h` aggregates language, compiler and architecture capabilities.
Standard feature flags follow the selected C dialect; VLA, complex and atomics
also honor the corresponding `__STDC_NO_*__` macro. MSVC compatibility mode is
conservative about VLA, complex, atomics and standard thread-local syntax.
Flags describe compiler language facilities, not OS services or library linkage.
No flag promises availability of `<threads.h>` or a threading runtime.

`features/hps_architecture.h` also reports `HPS_TARGET_HAS_X86_AESNI`, `PCLMUL`,
`AVX`, `AVX2` and `SHA` (each with the full `HPS_TARGET_HAS_X86_` prefix).
Each flag requires x86 identity and its own compiler predefined instruction
macro; all are zero on other families. These are translation-unit target-option
facts, not runtime CPU/CPUID or OS-state checks, execution permission, dispatch
or cryptographic implementations. Zero is lack of positive compile evidence,
not a statement that hardware lacks the feature.

## Architecture

Existing identifiers cover x86, x86-64, ARM32, AArch64 and RISC-V 32/64; additive
IDs cover MIPS, PowerPC and a known RISC-V family with unavailable/unsupported XLEN.
Unrecognized targets remain `HPS_ARCH_UNKNOWN`; architecture identity does not
imply SIMD, atomic lock freedom or permission for unaligned accesses.

`HPS_POINTER_BITS` uses the compiler's pointer size multiplied by `CHAR_BIT`,
with Windows 32/64-bit fallbacks and zero for unknown. `HPS_POINTER_WIDTH`
classifies only 32/64-bit pointers; a known 128-bit width remains an unknown
**classification** while its bit count is still available. Use
`HPS_POINTER_WIDTH_IS_KNOWN` to test bit-count availability.

Byte order uses compiler byte-order macros, then Windows/x86 little-endian
conventions only when direct evidence is absent. Explicit unsupported byte order
stays unknown even with those family hints; ARM/RISC-V identity alone never
selects little endian. Other targets return `HPS_ENDIAN_UNKNOWN`. Check
`HPS_ENDIANNESS_IS_KNOWN`, `HPS_IS_LITTLE_ENDIAN` and `HPS_IS_BIG_ENDIAN` before
selecting order-dependent code. Unknown is never silently assumed little-endian.

## Declaration macros

`HPS_STATIC_ASSERT(condition, message)` accepts an integer constant expression.
It uses a standard assertion in C11+ and a negative-size typedef rejection in
C89/C99, using a `size_t` array typedef from `<stddef.h>`. For the fallback, place assertions on distinct source lines; the
message is not part of the diagnostic. It never becomes a runtime check.

`HPS_INLINE` uses the C99/C++ keyword, compiler inline spellings for MSVC/GCC
in legacy C modes, and an empty fallback otherwise.
`HPS_RESTRICT` is empty by default; `HPS_ENABLE_RESTRICT=1` enables it in C99+
only after target review of the aliasing contract.
Use `static HPS_INLINE` for header-defined helpers to avoid external inline
linkage differences.

`HPS_NORETURN`, `HPS_NODISCARD`, and `HPS_DEPRECATED(message)` use supported
standard syntax by default. Compiler-specific annotations require
`HPS_ENABLE_COMPILER_ATTRIBUTES=1` and a recorded target review. Both configuration
switches default to zero and reject values other than zero or one; detection probes
remain informational. Apply configuration consistently across a target. Check their corresponding
`HPS_HAS_NORETURN`, `HPS_HAS_NODISCARD`, `HPS_HAS_DEPRECATED` flags if the
annotation is required. An empty unsupported annotation is not a guarantee;
`HPS_NODISCARD` diagnostics also depend on compiler warning settings.

`HPS_ALIGNAS(alignment)`/`HPS_ALIGNOF(type)` are defined only when the standard
facilities exist; guard their use with `HPS_HAS_ALIGNAS`/`HPS_HAS_ALIGNOF`.
`HPS_THREAD_LOCAL` is defined only with `HPS_HAS_THREAD_LOCAL`. Alignment and
thread-local storage have no silent semantic fallback. `HPS_UNUSED(value)`
casts to void and evaluates its operand once; avoid side effects in its operand.

## Version and validation

`hps_macros.h` supplies `HPS_STRINGIFY_RAW(tokens)` for unexpanded spelling and
`HPS_STRINGIFY(tokens)` for expansion before stringification. Both produce
immutable literals and evaluate no runtime expression. The raw form preserves
the legacy TO_STRING contract. See the
[23-header migration](../migration/bytes-features-optional.md).

`hps_version.h` supplies the major/minor/patch components, string and
`HPS_VERSION_AT_LEAST(major, minor, patch)`. This predicate is intended for
constant version values, not expressions with side effects.

Native tests check pointer width, byte order, version
comparisons and declaration usage. The Python suite compiles each header twice
in isolation for C99/C11/C17/C2x, compiles the base in freestanding mode, checks
expected rejection cases and tests synthetic preprocessor branches for supported
compiler/architecture identities, unknown targets and optional features.

Synthetic target tests validate branch selection, not target machine code,
foreign standard libraries, ABI correctness or actual hardware. Historical GCC and Clang
host results predate the current governance changes and do not validate this revision. MSVC, Armclang, cross targets
and actual C23 toolchains still need native validation before production use.
Safety/security certification, MISRA compliance and industry-specific assurance
are separate activities; this base implementation does not claim them.

Detection and probe rules follow the primary documentation:
[GCC predefined macros](https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html),
[GCC builtin probes](https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005fbuiltin.html),
[Clang extensions](https://clang.llvm.org/docs/LanguageExtensions.html), and
[Microsoft predefined macros](https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros).
