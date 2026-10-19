# Compiler attributes, utilities and hosted diagnostics — 2026-10-19

Status: user-authorized development migration; named maintainer/reviewer,
allocator-policy disposition, target qualification and release acceptance pending.
Author: Codex. Independent named project reviewer: unassigned. Source base:
`5813127ccc468a3be75acf5979261f6c4005ba05` plus this change. Profile: hosted,
single-context development; no industrial/embedded target is qualified.
Independent agent reviews are verification inputs, not assigned role approvals.

## Request, provenance and module boundaries

The user requested the same full reading, HPS naming/layer decomposition,
verification, commit and push for eight selected C-core attribute/debug/utility/
error headers. Every complete file was read as source data, not instructions.
The follow-up additionally requests repairing found issues and placing the span
implementation inside the same `data_structure/define.h` as the other generators.
The [lossless snapshot](../../docs/migration/utilities-runtime-snapshot.json)
retains all original bytes/hashes and matches the existing inventory. Reference
trees were neither modified nor built/executed. The
[migration mapping](../../docs/migration/utilities-runtime.md) records every
source symbol family and all intentional API/type/behavior changes.

Existing Base annotations are extended under their existing opt-in/probe policy;
array/type-spelling macros belong in Base. Typed scalar and checked bit functions
live separately in `hps/algorithm/`. Memory reuses existing raw primitives and
adds checked count operations, bounded swap and explicitly unqualified full-scan
equality. Optional allocated arrays/manual typed owners extend the existing
allocator proposal, not production core paths. The source's unbounded C-string
and variadic formatting wrappers map to existing bounded Cstr contracts.

An explicit `platform/hosted/debug.h` and separate default-disabled library
provide bounded stderr reporting/assertions/panic. Base/platform metadata and
core libraries neither include nor link this adapter. Generic `result/check.h`
provides only direct-int32 zero/nonzero predicates with uint8 results. No shared
status type, code range, category, registry or message lookup returns. Existing
module-local errors, pool ownership/resources and the prior status-removal
decision remain unchanged.

The actual span generator section is moved into `data_structure/define.h` without
changing its body or the existing container section. Old span paths forward to
that canonical provider; active source/tests use it directly. There is no include
cycle or duplicate implementation, and all existing generated names/lifetimes
remain unchanged. Compatibility wrappers are retained for existing consumers.

Intentional omissions are explicit: container_of's pointer reinterpretation,
packed ABI, unsupported alignment fallbacks, generic void** cleanup, implicit
compiler cleanup, libc heap allocation, native variadic formatting and global
error enums/lookup are not imported. Type/ownership changes are adaptations,
not claims of source/binary-compatible prefix renaming. Source defects include
undefined bool_t, mismatched negative enum progression versus message decoding,
nullable %s context, ambiguous realloc-to-zero and truncated panic text. The
snapshot preserves these facts; the new APIs do not silently reproduce them.

## Bounds, ownership, error priority and resources

Count products and bit shifts are checked before evaluation/mutation. Bit
outputs and checked-array payload/output state are preserved on errors; argument
errors precede range/overflow as documented. Empty array operations permit NULL
payload pointers; positive counts require positive element size and real extents.
Output metadata must be correctly typed, writable and disjoint where required.
No API can validate arbitrary pointers or forged extents/lifetimes.

Scalar functions and result predicates have fixed work/automatic scalar state.
Swap has exactly 256 automatic payload bytes plus scalar/call bookkeeping; its
loop handles bounded chunks of the caller-proven count, on equal/disjoint ranges.
Count wrappers use O(1) bookkeeping plus underlying byte/libc work. Full-scan
equality has no early exit in the source loop, but compiler timing/transformations
are unverified. Volatile zeroing retains no approved sensitive-erasure claim.
Arbitrary representation traversal requires an unsigned-character uint8_t target.
Zero bytes are not general typed-object initialization, and the byte pool is not
an arbitrary typed allocator.

Allocation helpers validate both old/new products before callbacks and require
release capability. Resize failure and zero-new-count rejection preserve the old
owner; successful growth clears only the added tail. Providers establish real
typed storage/alignment and bounded execution. Typed owners contain one native
typed pointer, count and provider pointer, with explicit adopt/reset/transfer;
no live-owner copy or automatic scope cleanup is allowed. Reset releases once;
transfer passes cleanup responsibility without casting a T** to void**. Failure
preservation depends on the documented provider contract.

Hosted text lengths bound output; NULL text becomes '?'. Assertions evaluate
once when enabled; NDEBUG suppresses only scalar/non-VLA evaluation. Always-on
assertions remain active. Failure reporting uses the caller's module-specific
message and direct numeric result, without classifying a bare code. Diagnostics
use no application buffers and ignore write failures so abort still occurs.
The hosted libc may block, allocate internally, lock/interleave output or retain
data; callers exclude secrets and this adapter has no core no-heap, signal/ISR,
atomic-output or timing guarantee. Core contains no new heap/OS/concurrency path.

HPS-C-017/018 rationale: existing attributes, Cstr formatting, byte primitives and
volatile zeroing are reused. Host text writes share one bounded/null helper.
Typed min/max specializations retain explicit signed/unsigned/size contracts;
bit routines share checked-mask construction. Allocator's multiplication flag
helper intentionally retains its older zero-size contract; checked array sizes
add positive-element-size validation and distinguish local errors. Owner/array
callback and byte-only contracts remain visibly distinct. No generic macro
language, double-evaluation extrema or unnecessary new registry is introduced.

## Proposed allocator extension disposition

Project exception `HPS-DEV-ALLOCATOR-UTILITIES-2026-10-19`: **proposed**, not
approved. Owner/author: Codex; independent reviewer/approving authority unassigned.
Policy version 0.3.0. Scope: optional `memory/allocated_array.h`, `memory/owner.h`
and explicit development tests/providers at this revision. Duration: development
only, expiring before production-core adoption or qualified release unless named
roles decide otherwise. This is not an external-standard deviation.

HPS-MEM-001 prohibits dependency allocator hooks in production core. The selected
source functionality needs ownership/allocation; retaining a bounded explicit
provider proposal preserves that purpose without adding a heap backend or silently
changing existing core ownership. Static/caller-owned providers do not themselves
resolve the prohibition. No core translation unit or umbrella calls these helpers.

Risks include hidden provider allocation, invalid types/alignment, exhaustion,
copied ownership and missing explicit cleanup. Controls are opt-in proposal
headers, positive checked sizes, required release, explicit ownership transfer,
provider contracts and static typed/byte test fixtures. Whole-product/provider
behavior, stack/timing and native target correctness remain unverified. Any
provider/API/target/compiler/libc/standard/ownership change requires revalidation.
No mandatory MISRA rule is proposed for deviation; project eligibility and
explicit approval/date are pending. Remove the hooks from production adoption or
obtain a scoped named decision; no approval is inferred from passing tests.

## Checker scope and traceability

The existing HPS-PRO-002 platform-adapter boundary is reflected in the checker:
only `src/platform/hosted/debug.c` may include stdio. Basic numerical types, heap,
concurrency and other platform headers remain checked in that source; public
headers and other files keep the old I/O restriction. Core inclusion of hosted
headers is additionally rejected. Quoted include scanning preserves strings while
blanking comments; numeric/call scans retain their existing literal exclusion.
Fourteen scope self-tests include allowed adapter I/O, rejected neighboring/header
I/O, rejected heap/types/concurrency and both quoted/angle core dependencies.
This corrects lexical scope for an already permitted separate adapter; it changes
no normative core requirement or approves any new heap/provider exception.

Applicable policy: HPS-HDR-001 through 006; HPS-C-001 through 019;
HPS-MEM-001/002/004/005; HPS-EXEC-001 through 003; HPS-SEC-001/002/003/005/006/007/009;
HPS-GOV-004/005/006/008/009; HPS-PRO-001/002/003; HPS-VER-001 through 006;
HPS-REL-001/003/004. Existing numerical/string/test/enum findings remain open.

Relevant MISRA rows: Directives 4.1, 4.6, 4.7, 4.8, 4.9, 4.10, 4.11, 4.12, 4.14;
Rules 1.1, 1.2, 1.3, 5.1, 5.4, 5.6, 8.2, 8.3, 8.8, 8.10, 8.13, 9.1,
10.1, 10.3, 10.4, 11.3, 11.5, 11.8, 12.2, 13.1, 13.2, 13.5, 13.6,
17.2, 17.7, 18.1, 18.2, 18.4, 18.6, 20.1, 20.4 through 20.14,
21.3, 21.6, 21.15, 21.16, 22.1. Existing dispositions/deviations are preserved;
updated evidence links retain earlier references below. Licensed rule, essential-
type, attribute/macro, stdio, representation and target analysis remain open.

## Current verification

Host: x86_64 Linux; GCC/G++ 11.4.0, Clang/Clang++ 14.0.0, Python 3.10.12,
CMake 3.22.1 and Meson 0.61.2. Evidence covers the source base above plus the
final working tree, including the span consolidation. Assertions remain active
with `-UNDEBUG`. New suites have direct `int32_t` entry points registered in all
build manifests/shared runner; hosted tests are conditional on the adapter.

| Executed check | Configuration and result |
| --- | --- |
| Strict default Make | `make test BUILD_DIR=build/utilities-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`; all 20 shared suites, 325 portability, 105 detection and 15 annotation checks PASS, including a fresh rerun after span consolidation. |
| Make hosted opt-in | `make all HPS_BUILD_HOSTED_DEBUG=1 BUILD_DIR=build/utilities-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`, then `build/utilities-make/hps_tests_hosted`; 21 suites PASS. Distinct core/hosted executable names avoid option-stale reuse. |
| Hosted subprocess contracts | `python3 tests/platform/check_hosted_debug.py --compiler cc`; 18 isolated process cases PASS, covering debug/release success, always-on abort, debug-assert suppression, exact bounded/null/long messages, numeric reporting and no-op zero results. Core dumps are disabled where supported. |
| CMake Release default | `cmake -S . -B build/utilities-cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS='-Wall -Wextra -Wpedantic -Werror'`, build `--parallel 4`, then `ctest --test-dir build/utilities-cmake --output-on-failure`; 4/4 PASS with adapter disabled. |
| CMake hosted branch/final span | Reconfigure the same directory with `-DHPS_BUILD_HOSTED_DEBUG=ON`, build and `ctest -R '^(hps_tests\|hps_hosted_debug_contracts)$'`; 2/2 PASS. After span consolidation, affected rebuild plus `ctest -R '^(hps_tests\|hps_base_portability)$'` again PASS (2/2). Unaffected earlier checks are not presented as rerun. |
| Meson/Clang ASan+UBSan | `CC=clang meson setup build/utilities-meson -Dwerror=true -Dhosted_debug=true -Db_sanitize=address,undefined -Db_lundef=false`, compile, then `ASAN_OPTIONS=detect_leaks=0 meson test -C build/utilities-meson --print-errorlogs`; 5/5 PASS. After span consolidation the affected rebuild, shared executable and portability checks again PASS. The 21-suite executable is instrumented; Python-generated child compiles use their explicitly recorded ordinary warning flags. LeakSanitizer remains disabled due the previously established ptrace environment limitation; no leak-sanitizer result is claimed. |
| Public C++ headers | 63 public headers independently/repeatedly included in G++/Clang++ C++11/17: 252 strict checks PASS. Annotation opt-in has four compile/link/run checks; eight mixed C99/C++ runs exercise all generated owner functions on actual typed storage, one-time cleanup, array/scalar/bit/result APIs and nonfatal hosted behavior. |
| Final merged span C++ refresh | Seven affected canonical/compatibility paths in both compilers/modes: 28 independent/repeated checks PASS. Eight generator runs with both include orders instantiate Span/CSPAN/Vector together, verify constness/static types and shared reads; PASS, no duplicate implementation/cycle. |
| Installed package | Install with `cmake --install build/utilities-cmake --prefix "$PWD/build/utilities-install"`; separate C++17 `find_package(HPS CONFIG REQUIRED)` consumer links `HPS::memory` and `HPS::hosted_debug`, instantiates typed owner and merged span/CSPAN through old/new paths, transfers/resets ownership and executes PASS. Consumer sources are under `build/utilities-consumer/`. |
| Adapter isolation | Undefined-symbol inspection of all five compiled core archives finds no stderr/abort/hosted diagnostic dependency. This is direct archive evidence, not whole-program no-heap or libc qualification. |
| Policy checker and its tests | `python3 tools/check_policy.py --self-test`: 14 scope, 16 lexical, eight header and six invalid-governance fixture cases PASS; repository result remains exit 1 with exactly the same 153 prior findings. No new finding or existing disposition suppression. |
| Integrity and whitespace | Eight lossless sources still match inventory/originals; moved span and pre-existing container sections are byte-identical. Existing non-evidence fields of all 221 MISRA rows preserved. `git diff --check` PASS; staged check precedes commit. |

C/C++ checks use `-Wall -Wextra -Wpedantic -Werror`, relevant language modes and
fresh C99 objects. Reproducer scripts: `/tmp/hps-final-cxx-check.py` and the
span-refresh checker supplied during this task. Repository tests retain the
important bounds/failure cases; temporary host consumers are not qualified target
evidence bundles. Earlier detection/memory reports do not verify this change.

Tests cover typed extrema limits/single evaluation, bit indexes 0/63/64/SIZE_MAX,
null-first error priority and output preservation; swap across its chunk boundary,
full-scan mismatch positions, checked products and overlapping movement; relocating
resize growth zeroing/shrink/failure and zero-size refusal; typed owner adoption,
transfer and exactly-once reset; module-local result conventions and bounded
hosted success/failure paths. Annotation probes are tested enabled, disabled and
unavailable; misuse of enabled nonnull is rejected by compiler diagnostics.

Independent reviews found and corrected one void*-to-uint8* C++ header conversion
and quoted-include detection in the checker; documentation distinguishes reset
invalidation from transfer. No blocking implementation/test defect remains under
documented caller/provider contracts. Named review/allocator disposition, licensed
MISRA analysis, vendor/target ABI/resources/libc, annotation proof and sensitive-
data/erasure/timing qualification remain open. Host results do not approve them.

## Retained earlier matrix evidence

| Earlier evidence | Rows |
| --- | --- |
| [bytes-endian-2026-10-11.md](bytes-endian-2026-10-11.md) | Rule 12.2 |
| [data-structure-2026-10-18.md](data-structure-2026-10-18.md) | Rule 13.1, Rule 13.2 |
| [detection-layers-2026-10-19.md](detection-layers-2026-10-19.md) | Dir 4.1, Dir 4.6, Dir 4.9, Dir 4.10, Dir 4.11, Rule 1.1, Rule 1.2, Rule 1.3, Rule 5.1, Rule 5.4, Rule 5.6, Rule 8.2, Rule 8.3, Rule 8.8, Rule 8.10, Rule 10.1, Rule 10.3, Rule 10.4, Rule 20.1, Rule 20.4, Rule 20.5, Rule 20.6, Rule 20.7, Rule 20.8, Rule 20.9, Rule 20.10, Rule 20.11, Rule 20.12, Rule 20.13, Rule 20.14 |
| [memory-layers-2026-10-19.md](memory-layers-2026-10-19.md) | Dir 4.8, Dir 4.12, Dir 4.14, Rule 8.13, Rule 9.1, Rule 11.3, Rule 11.5, Rule 11.8, Rule 17.2, Rule 17.7, Rule 18.1, Rule 18.2, Rule 18.4, Rule 18.6, Rule 21.3, Rule 21.15, Rule 21.16, Rule 22.1 |
| [status-removal-2026-10-18.md](status-removal-2026-10-18.md) | Dir 4.7 |
