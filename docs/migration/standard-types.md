# Standard type provider migration

Date: 2026-10-20. Source base: `81fe9d7` plus this change. The user supplied
CFJ Boolean and stddef snippets in the request and a complete stdint attachment.
The [attachment snapshot](standard-types-snapshot.json) preserves the latter's
original bytes, size and SHA-256 without depending on the session attachment path.
Neither legacy framework, its archive nor its original source manifest changes.

The three providers live in Base, beneath compiler/language detection and beneath
all utility modules. Consumers use `bool`, `true`, `false`, `uint8_t`, `int32_t`,
`size_t`, `ptrdiff_t`, `wchar_t`, `NULL` and `offsetof` as standard names. There
is no `hps_bool` or other replacement type namespace. The Base umbrella exports
the providers; detection-only headers remain independent from Boolean macros.

| Supplied interface | HPS provider and deliberate adaptation |
| --- | --- |
| `cfj_bool.h` and its conversion functions | `hps/base/hps_bool.h`; `hps_bool_*` helpers, standard type names, shared `HPS_INLINE`, native C++/C23 keywords and C99 stdbool, byte fallback before C99 |
| Scalar character arguments/results in Boolean helpers | `uint8_t` execution-character codes under HPS-C-003; callers pass `'0'`/`'1'`. This differs from the supplied scalar `char` ABI and is not an ASCII-only wire-format promise. |
| `cfj_bool_from_char_checked` | A NULL output returns false without access; invalid input clears a valid writable output to false and returns false, preserving the supplied behavior. A valid input writes the parsed result and returns true. |
| `cfj_stdint.h` | `hps/base/hps_stdint.h`; prefer a usable native stdint, including C++ and pre-C99 modes when discoverable. Replacement types/limits/constants use compiler metadata and verified exact-width representations. |
| Architecture guesses for `intptr_t`/`uintptr_t` | Compiler-provided pointer-integer metadata; unavailable optional pointer types remain unavailable. There is no assumed 32-bit default. |
| Least/fast/max families | Kept as standard provider declarations; their availability does not authorize their use in ordinary HPS module storage. Replacement fast types need not match a host libc. |
| `cfj_stddef.h` | `hps/base/hps_stddef.h`; native stddef by default; explicit replacement requires compiler type metadata. C++ keeps its built-in `wchar_t`. |
| Address-based `offsetof` fallback | Compiler builtin with integer constant-expression semantics; unsupported replacement compilers reject the configuration. No null-member-address expression. |
| CFJ includes, guards and macros | Base dependencies, path-derived guards, file metadata, `HPS_STDINT_USE_FALLBACK` / `HPS_STDDEF_USE_FALLBACK` validated as 0 or 1; includes outside linkage blocks. |

All existing direct stdint/stddef/stdbool includes in public headers, sources,
tests and active documentation examples now use the providers. The dedicated
portability test intentionally includes native headers to verify default-mode
coexistence in both orders. Public module signatures, layouts, error numbers and
`uint8_t` flag storage remain unchanged in native mode; rebuild consumers to
verify their selected compiler/library ABI. Existing Cstr Boolean parsing retains
its bounded `uint8_t` output contract.

Replacement mode is for a missing/unusable native header. Do not combine a
replacement stdint provider with native stdint/inttypes, directly or transitively:
compiler fast-width type choices can differ from libc choices. The replacement
integer provider is a scoped type/limits/constant surface, not a complete substitute
for every later revision's stdint additions. Native modes inherit their library's
surface. C89 byte `bool` does not normalize arbitrary assignments to zero/one;
use the conversion helpers when normalization is required. Never share its ABI
with native `_Bool`/C++ bool without target evidence.

Earlier migration decisions to omit standard-type wrappers are historical and
superseded by this explicit request. This step does not rewrite those reports or
silently remove pending enum, character-storage, allocator or test-entry findings.
Read the [Base contract](../architecture/base.md) and the
[current review/evidence](../../governance/reviews/standard-types-2026-10-20.md)
when continuing on another machine.
