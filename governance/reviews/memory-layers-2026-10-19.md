# Memory primitives, ownership proposals and layer paths — 2026-10-19

Status: user-authorized development work; named maintainer/reviewer acceptance,
policy exception and deployment qualification remain pending. Author: Codex.
Independent named project reviewer: unassigned. Source base:
`cf5a43715dc6179fac1895131b2a8d8c09699611` plus supplied drafts and this change.
Profile: hosted, single-context development. No embedded/industry target is
qualified. Agent source review is verification input, not project-role approval.

## Scope and observable contracts

The user requested appropriate placement/naming of the pasted memory code and a
framework-wide layer review. The [layer map](../../docs/architecture/layers.md)
records the resulting acyclic module graph. Raw operations belong in Memory,
binary descriptors remain in Bytes, and allocator/clone ownership proposals are
separate optional headers. Base contains no runtime ownership interface. The
pool's API, resources and single-context assumptions are unchanged.

`hps/data_structure.h` is canonical; the old include forwards without changing
generator names, fields or bodies. Shared text helpers move unchanged from
`src/string/internal.h` to `src/internal/text.h`; String and Cstr both use this
implementation-only provider. Codec includes its own API first. No other module
rename or dependency change was justified by the review.

The [draft migration](../../docs/migration/memory.md) retains exact source text,
hashes, every public name/signature mapping and observable behavior changes.
Allocator wrappers previously collided with pool allocation symbols; they now
use `hps_allocator_*`. Native numerical declarations become exact-width/size
types, and array failure codes are typed/local. Raw functions have external C
linkage and link through the existing memory target; clone/allocator wrappers
remain internal inline definitions using Base's attribute implementation.

Unsafe word-alias accesses/pointer-integer alignment checks are removed.
Copy/move/fill delegate to libc; movement never orders unrelated pointers.
Comparison retains exact unsigned-byte difference, search retains const, and
volatile zeroing makes no sensitive-erasure claim. Nonempty extents, valid
representations, typed storage and lifetimes remain caller obligations.
Arbitrary representation traversal requires a target where `uint8_t` is an
unsigned character type. No new universal freestanding/constant-time claim is made.

Array product overflow and zero element sizes are rejected before mutation.
Cloner failures clean completed elements in ascending order; the failing cloner
owns partial cleanup. Resource-owning clones require a destructor and initially
unowned destination elements. Rollback does not restore destination bytes.
The headers document validation priority, local codes, callback context, disjoint
ranges and source preservation. No pool-to-typed-allocation adapter is supplied.

## Resources, specialization and threat boundary

All operation wrappers and clone traversal use O(1) automatic bookkeeping, no
temporary payload array, heap call, recursion, VLA, OS API, locks or atomics.
Byte work is bounded by caller-proven count; clone work by count plus provider
and callback work. Integrators must budget those bounds, provider capacity,
callback stack/cost, libc resources and object alignment/lifetime on their target.
The fixed pool's 32 by 128 byte resource budget is unchanged. No measured latency,
stack, performance, erasure or WCET result is claimed.

Lengths and callback/provider implementations are trust boundaries. This code
cannot validate arbitrary pointers, malicious callbacks, invalid object
representations or a forged extent. Count overflow is rejected before copying
or callbacks. Static-provider exhaustion is tested as a bounded failure; the
engine does not retry. Secrets must not use the early-exit comparison or rely on
volatile writes as an approved erasure primitive.

HPS-C-017/HPS-C-018: copy/fill/move/search reuse libc contracts already present in
the project. Exact first-byte difference intentionally specializes comparison
because libc promises only sign. Existing Bytes/String libc calls retain their
separate descriptor/validation contracts and do not gain a pool dependency.
Clone reuses the allocator's checked product helper and the raw copy provider.
Shared text validation has one unchanged implementation. No duplicate inline
compiler-detection system remains in the supplied allocator design.

## Proposed project-policy exception: HPS-DEV-ALLOCATOR-2026-10-19

Type: project exception only, **proposed**, not approved. Owner/author: Codex;
independent reviewer and approving maintainer: unassigned. Policy version: 0.3.0.
Scope: `hps/memory/allocator.h`, `hps/memory/clone.h` and explicit development
callers/tests in this source change. Duration: development proposal only; expires
before production-core adoption or any qualified release unless explicitly
disposed by named project roles. This is not an external-standard deviation.

HPS-MEM-001 prohibits dependency allocator hooks in production core paths. The
supplied design requires such callbacks. Replacing the allocator parameter with
an unrelated context or deleting ownership behavior would silently redesign the
requested import; the original design is instead prepared separately for review.
There is no compiled core call site, umbrella inclusion, built-in heap provider
or pool adapter. Static/caller-owned bounded providers are the proposal's scope;
static backing by itself does not resolve the prohibition.

Risks are hidden allocation/OS behavior in callbacks, invalid alignment/types,
exhaustion, leaked partial ownership and callback execution bounds. Controls are
explicit opt-in headers/proposal marking, provider contracts, checked products,
documented partial cleanup, static typed/byte test fixtures and failure tests.
Whole-product no-heap behavior and arbitrary provider correctness are unverified.
Affected profiles are any future bare-metal/RTOS/hosted production integration.

Eligibility is subject to maintainer project-policy review; no mandatory MISRA
rule is proposed for deviation. Named independent review, explicit approval/date,
provider qualification and release record do not exist. Revalidate on any API,
provider, target/compiler/libc, standard or ownership-contract change. Remove the
proposal/exception requirement by removing allocator hooks from core use, or
obtain a scoped project-policy decision before such adoption. No approval is
inferred from the user's placement request or passing host tests.

## Policy and MISRA traceability

Applicable requirements: HPS-HDR-001 through 006; HPS-C-001/002/004/006 through
019; HPS-MEM-001/002/004/005; HPS-EXEC-001 through 003; HPS-SEC-001/002/003/005/007/009;
HPS-GOV-004/005/006/008/009; HPS-PRO-001/002/003; HPS-VER-001 through 006;
HPS-REL-001/003/004. Existing legacy numerical/test/string findings remain open.

Relevant matrix rows: Directives 4.1, 4.6, 4.8, 4.9, 4.10, 4.11, 4.12, 4.14;
Rules 1.3, 8.2, 8.3, 8.8, 8.10, 8.13, 9.1, 10.1, 10.3, 10.4, 11.3, 11.5,
11.8, 17.2, 17.7, 18.1 through 18.4, 18.6, 20.1, 21.3, 21.15, 21.16 and 22.1.
Matrix evidence links point here; all existing statuses/deviations are preserved.
Pointer conversions, representation access, essential types, libc/callback
contracts, resource ownership and native-target ABI need full guideline analysis.
No MISRA disposition is approved by this record.

## Current verification

Host: x86_64 Linux, GCC/G++ 11.4.0, Clang/Clang++ 14.0.0, Python 3.10.12,
CMake 3.22.1 and Meson 0.61.2. Current working tree: source base above plus this
change. Assertions are enabled with `-UNDEBUG`; both new callable test suites
are registered in the shared runner and all three build manifests.

| Actual check | Configuration and result |
| --- | --- |
| Make shared suites and independent headers | `make test BUILD_DIR=build/memory-layers-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`; all 17 suites and 249 portability checks PASS. |
| CMake Release | `cmake -S . -B build/memory-layers-cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS='-Wall -Wextra -Wpedantic -Werror'`, `cmake --build build/memory-layers-cmake --parallel 4`, `ctest --test-dir build/memory-layers-cmake --output-on-failure`; 2/2 PASS. |
| Meson/Clang sanitizer build | `CC=clang meson setup build/memory-layers-meson -Dwerror=true -Db_sanitize=address,undefined`, then `meson compile -C build/memory-layers-meson`; PASS. Initial `meson test -C build/memory-layers-meson --print-errorlogs` passed portability but the shared executable failed at LeakSanitizer shutdown because this environment runs under ptrace. |
| Address/undefined behavior execution | `ASAN_OPTIONS=detect_leaks=0 meson test -C build/memory-layers-meson hps_tests --print-errorlogs`; all 17 suites PASS, ASan/UBSan enabled. LeakSanitizer is disabled for this rerun and is unsupported here; no leak-sanitizer result is claimed. Meson's Clang/b_lundef configuration warning remains visible; linking succeeded. |
| C/C++ public headers | Seven headers (operations, allocator, clone, data_structure, both compatibility paths and pool), included twice independently with both GCC/Clang C99 and G++/Clang++ C++11/17: 42 strict syntax checks PASS. |
| Mixed C/C++ callers | Four executables, using G++ and Clang++ in C++11 and C++17, link operations/pool objects compiled separately as C99 at `-O2`; all PASS. Calls cover every primitive, allocator wrappers, shallow/callback clone/rollback, containers and simultaneous pool use. |
| Host representation assumption | GCC/Clang C99 `__builtin_types_compatible_p` checks and C++11/17 `std::is_same` assertions establish this host's `uint8_t` is `unsigned char`. Other targets remain unqualified. |
| Source/migration integrity | Source snapshots match the recorded hashes. Moved data-structure/text-helper implementations match HEAD after guard/file-path substitutions. All 221 matrix statuses and other non-evidence fields are preserved. `git diff --check` PASS. |
| Required policy subset | `python3 tools/check_policy.py` returns 1 with 153 retained findings, down from 164 including the supplied allocator draft. No finding is in the new/relocated memory or layer files. Full policy result remains FAIL; it is not a MISRA analyzer. |

The independent header/mixed-language checks use
`-Wall -Wextra -Wpedantic -Werror -Wconversion -Wsign-conversion -Wshadow
-Wcast-qual`; C additionally uses `-Wstrict-prototypes -Wmissing-prototypes`.
Temporary source/argument manifests remain under
`/tmp/hps-memory-interoperability-ocfrwdva/` (`header-commands.json`,
`mixed-commands.json`, `mixed-caller.cpp`, `host-uint8-proof.c`). These direct
host checks are not a portable target evidence bundle. Repository test sources
retain primitive guard/overlap cases and bounded clone/provider failure cases.

Primitive tests cover NULL/empty input, guard preservation, exact +/-255 and
first-mismatch comparisons, const search, overlapping moves in both directions,
identity and distinct arrays, and volatile zero bounds. Clone tests use typed
destination objects and a caller-owned three-by-eight-byte provider. They cover
dispatch/fallback, zero-size forwarding, overflow before callbacks, missing
callbacks, failed resize ownership, shallow/non-owning clones, independent owned
children, exhaustion, forward rollback and a failed cloner's partial cleanup.
The failed destination slot is never read/destructed. Source payload/ownership
and provider slot recovery are checked independently of the engine implementation.

Separate agent reviews found no blocking implementation/test defect under the
documented contracts; resource wording and a zero-size-resize migration sentence
were corrected. No named maintainer approval is inferred. Historical results
do not verify this change. Named policy disposition, licensed MISRA analysis,
target compiler/ABI/libc/storage/stack/callback/erasure evidence, independent
project-role review and release acceptance remain open.

## Retained earlier matrix evidence

The table below preserves earlier evidence links for rows updated by this review.

| Earlier evidence | Rows |
| --- | --- |
| [bytes-reader-2026-10-11.md](bytes-reader-2026-10-11.md) | Rule 18.4 |
| [csep-text-2026-10-13.md](csep-text-2026-10-13.md) | Rule 21.15 |
| [data-structure-2026-10-18.md](data-structure-2026-10-18.md) | Dir 4.1, Dir 4.6, Dir 4.8, Dir 4.9, Dir 4.10, Dir 4.11, Dir 4.14, Rule 1.3, Rule 8.2, Rule 8.3, Rule 8.8, Rule 8.10, Rule 8.13, Rule 9.1, Rule 10.1, Rule 10.3, Rule 10.4, Rule 17.7, Rule 18.1, Rule 18.6, Rule 20.1 |
| [pointer-pool-2026-10-11.md](pointer-pool-2026-10-11.md) | Dir 4.12 |
