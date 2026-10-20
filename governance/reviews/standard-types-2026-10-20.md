# Standard type providers and dependency review

Date: 2026-10-20. Source base: `81fe9d7` plus this change. User authorization
explicitly requests three standard-name compatibility headers, repository-wide
dependency migration, improvements, commit and push. This is development work;
independent maintainer/reviewer acceptance and target qualification remain open.

## Behavior and scope

The [migration record](../../docs/migration/standard-types.md) maps all supplied
interfaces and adaptations, including scalar `char` to `uint8_t` character codes,
preserved checked-parse failure mutation and replacement/native coexistence.
Providers live only in Base. No module ABI/type namespace, shared status system,
heap/OS/concurrency dependency or static-pool ownership change is introduced.
Native types continue to come from the compiler/library headers. Other modules
keep their existing signatures and stored `uint8_t` flags. The full framework
implementation minimum remains C99; a working C89 provider is not a promise that
all framework modules compile as C89.

Default stdint detection prefers an available native header; default stddef uses
its native header. Explicit replacement modes validate configuration and require
compiler metadata instead of assuming widths from architecture names. stddef
uses builtin offsetof. Integer exact-width requirements and signed ranges are
checked. Unsupported metadata produces a diagnostic; old MSVC, IAR, ARMCC and
other proprietary fallback targets are not claimed to work without evidence.

HPS-C-020 and policy version 0.4.0 record the user-requested provider exception.
The lexical checker recognizes only standard typedef declarations in the stdint
provider, `bool` in the new Boolean provider/dedicated tests, and standard
integer family uses in those dedicated fixtures. It still rejects
basic numerical locals, private aliases, nonexact module storage, allocation and
other violations in those files. It also rejects direct native stdint/stddef/
stdbool includes outside the matching provider in public/core files. This is a
project-policy scope change, not a MISRA deviation approval or a claim that
earlier Boolean enum declarations are now accepted.

Primary portability references:
[GCC type/limit/constant metadata](https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html),
[GCC offsetof builtin](https://gcc.gnu.org/onlinedocs/gcc/Offsetof.html),
[Clang language extensions](https://clang.llvm.org/docs/LanguageExtensions.html),
and [WG14 C23 draft N3096](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf).
These explain language/compiler facilities; they do not qualify a deployment.

## Resource, error and review contracts

Helpers have constant bounded work, scalar arguments/outputs and immutable
borrowed string literals of static lifetime. They allocate nothing, perform no
I/O, retain no caller pointers and create no mutable global state. Checked parse
requires a writable live Boolean output or NULL. It accepts only execution
character codes for `'0'` and `'1'`; callers own string extent/encoding checks.
Invalid non-null pointers cannot be diagnosed. C89 byte bool and native Boolean
storage have different conversion/ABI assumptions, explicitly documented rather
than hidden behind a framework typedef.

HPS-C-017/018 rationale: conversions remain small typed operations with distinct
results; shared providers remove repeated type/header selection across modules.
Repeated compiler-metadata checks and signed/unsigned constant declarations stay
explicit so each standard family, representation and diagnostic is reviewable.
No generic untyped runtime conversion engine is added.

Applicable IDs: HPS-HDR-001 through 006; HPS-C-001/002/003/004/006/007/008/009/
011/012/013/015/016/017/018/019/020; HPS-MEM-001/002/005; HPS-EXEC-001;
HPS-SEC-001/002/007/009; HPS-PRO-001/002; HPS-GOV-004/005/006/008/009;
HPS-VER-001 through 006; HPS-REL-001/003/004. Applicable profiles are hosted
development and future freestanding integrations; none is qualified here.

Relevant MISRA rows: Directives 1.1, 4.1, 4.6, 4.7, 4.9, 4.10, 4.11;
Rules 1.1, 1.2, 1.3, 5.4, 5.6, 8.2, 8.3, 8.10, 8.13, 9.1, 10.1,
10.3, 10.4, 11.9, 17.7, 20.1, 20.4, 20.5, 20.7, 20.10, 20.11,
20.12, 20.14 and 21.1. Matrix assessment/deviation statuses remain unchanged;
previous evidence is retained below. Licensed semantic/essential-type analysis,
reserved standard names/fallback macros, compiler extensions, target ABI and
independent review remain unresolved. No compliance or performance claim.

## Current verification

Host: x86_64 Linux; GCC/G++ 11.4.0, Clang/Clang++ 14.0.0, Python 3.10.12,
CMake 3.22.1 and Meson 0.61.2. Evidence covers the source base above plus this
change, with assertions enabled by `-UNDEBUG`. The new callable suite uses an
`int32_t` entry point registered in the shared runner and all build manifests.
Historical results are not reused as evidence for this change.

| Executed check | Configuration and actual result |
| --- | --- |
| Strict Make/default | `make test BUILD_DIR=build/standard-types-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`: shared runner (28 suites), 373 portability, 303 provider, 157 detection, 15 annotation and 942 JSON checks PASS. Following the final header refinement, `make all` and the rebuilt shared runner PASS. |
| CMake Release/hosted | `cmake -S . -B build/standard-types-cmake -DCMAKE_BUILD_TYPE=Release -DHPS_BUILD_HOSTED_DEBUG=ON -DCMAKE_C_FLAGS='-Wall -Wextra -Wpedantic -Werror'`, `cmake --build build/standard-types-cmake --parallel 4`, `ctest --test-dir build/standard-types-cmake --output-on-failure`: 7/7 PASS, including all 29 hosted/shared suites. Rebuilt after the final header refinement before CTest. |
| Clang/Meson UBSan | `CC=clang meson setup build/standard-types-meson -Dwerror=true -Dhosted_debug=true -Db_sanitize=undefined`, `meson compile -C build/standard-types-meson -j 4`, `meson test -C build/standard-types-meson --print-errorlogs`: 7/7 PASS. Shared suites/libraries use UBSan; generated Python compiler checks use their own explicit ordinary flags. |
| Provider matrix/final fixtures | `python3 tests/base/check_standard_types.py --compiler cc` and `--compiler clang`: 303 and 304 PASS, respectively. C89/C99/C11/C17/C2x, C++98/11/17/20; native and isolated replacement providers, repeated inclusion, runtime conversions, preprocessor constants, promoted expression types and metadata rejection. Final test-only metadata-introspection cleanup was rerun with both compilers. |
| All public headers/C++ | 75 headers, each independently included twice with G++/Clang++ C++11/C++17 and `-Wall -Wextra -Wpedantic -Werror -fsyntax-only`: 300 PASS. |
| Installed C++ consumer/C linkage | `cmake --install build/standard-types-cmake --prefix "$PWD/build/standard-types-install"`; independent C++17 CMake consumer under `build/standard-types-consumer` uses `find_package(HPS CONFIG REQUIRED)`, `HPS::base`, `HPS::bytes`, all three installed providers, Boolean helpers and Bytes functions from fresh C99 archives. Configure/build/run PASS. |
| Policy scope | `python3 tools/check_policy.py --self-test`: 14 implementation-scope, 18 standard-provider, 16 lexical, eight header and six invalid-governance fixture checks PASS. Repository command remains exit 1 with the same 153 existing findings. Path/rule/token multisets match the before-change baseline after normalizing line offsets; no new finding or removal of prior findings. |
| Traceability/integrity | All 221 MISRA matrix rows retain their non-evidence fields; selected evidence links point here with prior links retained below. Attachment snapshot decodes to its recorded byte count/SHA-256. Direct native standard-type includes remain only in matching providers in public/core code; native test include orders are deliberate. `git diff --check` PASS. |

The conversion suite checks every possible octet input, Boolean normalization,
signed extrema, both literal outputs, checked NULL/invalid/valid mutation,
size/count and pointer differences within an actual array, and offset bounds.
Compiler checks cover missing exact/least/fast/max metadata, missing constant
builders, partial optional pointer metadata, invalid configuration, rejected
non-eight-bit replacement and accidental replacement/native mixing. Promoted
integer constant/minimum types are checked with C11 generic selection and C++11
type comparisons, including unsigned wide-character values.

The Clang 32-bit freestanding frontend additionally compiles a metadata fixture
where equal-width signed `int32_t`/`int_least32_t` use the compiler's long type;
constant type and preprocessor value checks PASS. This is compile-only synthetic
metadata evidence, not a foreign target/library qualification or executed 32-bit
test. GCC's optional 32-bit fixture explicitly skips because its freestanding
limits include requires unavailable 32-bit libc headers. Both host compilers
reject the `-std=c23` option, explicitly skipped; draft `-std=c2x` passes but
is not final C23 validation.

Review corrected Clang C89 empty-suffix macro handling, signed constant rank
when native types have equal widths, unsigned zero-minimum promoted types and
missing signedness for optional atomic limits. In replacement mode, optional
atomic bounds remain absent if metadata cannot identify the minimum; no guessed
value is supplied. Scope is the documented type/limit/constant subset rather
than every later stdint/stddef addition. A replacement cannot be combined with
native providers directly or through another library header.

Remaining work: independent project-role acceptance; licensed MISRA/essential-
type and standard-identifier/macro analysis; actual final C23 and proprietary
compiler validation; qualified target representation/ABI/library/stack/timing
and application integration evidence. Existing enum, native-int suite,
character-storage and allocator deviations remain unresolved. No MISRA
compliance, industrial qualification, measured performance or WCET is claimed.

## Retained earlier matrix evidence

| Earlier evidence | Rows |
| --- | --- |
| No prior evidence recorded | Rule 21.1 |
| [bytes-features-optional-2026-10-19.md](bytes-features-optional-2026-10-19.md) | Dir 4.1, Dir 4.6, Dir 4.7, Dir 4.9, Dir 4.10, Dir 4.11, Rule 1.1, Rule 1.3, Rule 5.4, Rule 5.6, Rule 8.2, Rule 8.3, Rule 8.10, Rule 8.13, Rule 9.1, Rule 10.1, Rule 10.3, Rule 10.4, Rule 17.7, Rule 20.1, Rule 20.7, Rule 20.10, Rule 20.11, Rule 20.12, Rule 20.14 |
| [data-structure-2026-10-18.md](data-structure-2026-10-18.md) | Rule 11.9 |
| [detection-layers-2026-10-19.md](detection-layers-2026-10-19.md) | Dir 1.1 |
| [utilities-runtime-2026-10-19.md](utilities-runtime-2026-10-19.md) | Rule 1.2, Rule 20.4, Rule 20.5 |
