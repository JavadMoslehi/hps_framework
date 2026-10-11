# Base layer contract

The base layer provides compile-time information and portable C declarations.
Industry policy, protocols, memory management, threading implementations, device
access, logging and domain data models belong in higher layers. All base headers
can be included independently and repeatedly; none defines executable runtime
state. The API starts at development version 0.1.0 and is not a stable ABI.

## Language and standards

`language.h` exposes `HPS_LANGUAGE_IS_C`, `HPS_LANGUAGE_IS_CPP`, the raw
`HPS_C_STANDARD_VERSION`, C89/C95/C99/C11/C17/C23 constants, and the raw C++
version for detection only. C++ is not advertised as a C dialect. The complete
base API, including `types.h`, requires C99 or newer; C++ consumption is not
supported. Detection, version and declaration headers can be used in C89.

`standard.h` preserves the C99 minimum policy. `HPS_REQUIRE_C99` enables an
explicit compile-time rejection in this header. `HPS_C_IS_C99` through
`HPS_C_IS_C23` mean **at least** that revision. `HPS_C_LEGACY` is true only for
pre-C99 C. `HPS_C_STANDARD_NAME` is a diagnostic string. C23 is recognized only
at `__STDC_VERSION__ >= 202311L`; older C2x draft modes are not treated as full
C23 implementations.

`HPS_HOSTED_IS_KNOWN` distinguishes a known execution environment from absent
information. `HPS_IS_HOSTED` and `HPS_IS_FREESTANDING` are both false if the
compiler does not supply `__STDC_HOSTED__`. Freestanding use assumes the target
provides the standard C99 headers `<limits.h>`, `<stddef.h>` and `<stdint.h>`.

## Compiler and capabilities

`compiler.h` checks Armclang, Clang, MSVC, then GCC so compatibility macros
do not hide more specific identities. Version fields
describe the selected compiler frontend. Unknown compilers have identifier and
version fields equal to zero. Known GCC derivatives excluded from GCC detection
fall back to unknown unless recognized as another frontend. Compiler detection
is not a claim of certification or exhaustive vendor coverage.

`HPS_HAS_ATTRIBUTE(name)`, `HPS_HAS_BUILTIN(name)` and `HPS_HAS_FEATURE(name)`
delegate to compiler probes when present and return zero otherwise. These are
preprocessor probes. `HPS_HAS_GNU_EXTENSIONS` describes GNU compatibility mode;
use individual probes when a particular extension is required.

`features.h` aggregates language, compiler and architecture capabilities.
Standard feature flags follow the selected C dialect; VLA, complex and atomics
also honor the corresponding `__STDC_NO_*__` macro. MSVC compatibility mode is
conservative about VLA, complex, atomics and standard thread-local syntax.
Flags describe compiler language facilities, not OS services or library linkage.
No flag promises availability of `<threads.h>` or a threading runtime.

## Architecture

Existing identifiers cover x86, x86-64, ARM32, AArch64 and RISC-V 32/64.
Unrecognized targets remain `HPS_ARCH_UNKNOWN`; architecture identity does not
imply SIMD, atomic lock freedom or permission for unaligned accesses.

`HPS_POINTER_BITS` uses the compiler's pointer size multiplied by `CHAR_BIT`,
with Windows 32/64-bit fallbacks and zero for unknown. `HPS_POINTER_WIDTH`
classifies only 32/64-bit pointers; a known 128-bit width remains an unknown
**classification** while its bit count is still available. Use
`HPS_POINTER_WIDTH_IS_KNOWN` to test bit-count availability.

Byte order uses compiler byte-order macros, then Windows' little-endian
convention. Other targets return `HPS_ENDIAN_UNKNOWN`. Check
`HPS_ENDIANNESS_IS_KNOWN`, `HPS_IS_LITTLE_ENDIAN` and `HPS_IS_BIG_ENDIAN` before
selecting order-dependent code. Unknown is never silently assumed little-endian.

## Types

| Type | Contract |
| --- | --- |
| `hps_byte` | `unsigned char`; a C byte contains `HPS_BITS_PER_BYTE` bits |
| `hps_bool` | `_Bool`, with `HPS_TRUE` and `HPS_FALSE` |
| `hps_size` | `size_t` |
| `hps_ssize` | `ptrdiff_t`; pointer differences, not POSIX `ssize_t` |
| `hps_float`, `hps_double` | Native `float`/`double`; no fixed width or IEEE-754 promise |
| `hps_i8`/`hps_u8` through `hps_i64`/`hps_u64` | Exact widths, available only with `HPS_HAS_INT8`/`16`/`32`/`64` |
| `hps_i_least8`/`hps_u_least8` through `hps_i_least64`/`hps_u_least64` | Standard minimum-width integer types |
| `hps_iptr`, `hps_uptr` | Optional `intptr_t`/`uintptr_t`, guarded by `HPS_HAS_INTPTR` |
| `hps_imax`, `hps_umax` | `intmax_t`/`uintmax_t` |

`HPS_NULL` is the standard C `NULL`. There is no assumption that a byte has eight
bits, that a pointer fits in a 64-bit integer, or that `long` has a fixed width.
Pointer-sized integers guarantee only the standard `void *` round-trip contract;
do not use them as a promise about function pointers or arbitrary address math.

## Declaration macros

`HPS_STATIC_ASSERT(condition, message)` accepts an integer constant expression.
It uses a standard assertion in C11+ and a negative-size typedef rejection in
C89/C99. For the fallback, place assertions on distinct source lines; the
message is not part of the diagnostic. It never becomes a runtime check.

`HPS_INLINE` and `HPS_RESTRICT` use C99 keywords, with empty C89 fallbacks.
Use `static HPS_INLINE` for header-defined helpers to avoid external inline
linkage differences.

`HPS_NORETURN`, `HPS_NODISCARD`, and `HPS_DEPRECATED(message)` use supported
compiler attributes or standard syntax. Check their corresponding
`HPS_HAS_NORETURN`, `HPS_HAS_NODISCARD`, `HPS_HAS_DEPRECATED` flags if the
annotation is required. An empty unsupported annotation is not a guarantee;
`HPS_NODISCARD` diagnostics also depend on compiler warning settings.

`HPS_ALIGNAS(alignment)`/`HPS_ALIGNOF(type)` are defined only when the standard
facilities exist; guard their use with `HPS_HAS_ALIGNAS`/`HPS_HAS_ALIGNOF`.
`HPS_THREAD_LOCAL` is defined only with `HPS_HAS_THREAD_LOCAL`. Alignment and
thread-local storage have no silent semantic fallback. `HPS_UNUSED(value)`
casts to void and evaluates its operand once; avoid side effects in its operand.

## Version and validation

`version.h` supplies the major/minor/patch components, string and
`HPS_VERSION_AT_LEAST(major, minor, patch)`. This predicate is intended for
constant version values, not expressions with side effects.

Native tests check type sizes, pointer round trips, byte order, version
comparisons and declaration usage. The Python suite compiles each header twice
in isolation for C99/C11/C17/C2x, compiles the base in freestanding mode, checks
expected rejection cases and tests synthetic preprocessor branches for supported
compiler/architecture identities, unknown targets and optional features.

Synthetic target tests validate branch selection, not target machine code,
foreign standard libraries, ABI correctness or actual hardware. GCC and Clang
on the development host are the tested toolchains. MSVC, Armclang, cross targets
and actual C23 toolchains still need native validation before production use.
Safety/security certification, MISRA compliance and industry-specific assurance
are separate activities; this base implementation does not claim them.

Detection and probe rules follow the primary documentation:
[GCC predefined macros](https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html),
[GCC builtin probes](https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005fbuiltin.html),
[Clang extensions](https://clang.llvm.org/docs/LanguageExtensions.html), and
[Microsoft predefined macros](https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros).
