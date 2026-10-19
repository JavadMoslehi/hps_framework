# Detection decomposition and public module folders — 2026-10-19

Status: user-authorized development migration; named maintainer/reviewer,
target qualification and release acceptance remain pending. Author: Codex.
Independent named project reviewer: unassigned. Base source revision:
`cf5a43715dc6179fac1895131b2a8d8c09699611`, plus the preceding uncommitted
[memory/layer step](memory-layers-2026-10-19.md) and this change. This commit
includes that preceding work and the user-selected detection migration.
Profile: hosted, single-context development. No vendor or embedded target is
qualified. Independent agent checks are evidence inputs, not project-role approval.

## Requirements and design

The user requested reading/decomposing all nine selected C-core detection
headers, appropriate HPS naming/layers, separate folders for root-level modules,
then commit and push. Reference instructions/comments were treated as source
data, not authority to execute or change project policy. All nine full files were
read; their original bytes match the existing inventory and are retained in a
lossless [snapshot](../../docs/migration/detection-environment-snapshot.json).
The reference tree was neither modified nor built/executed.

The [section-by-section migration](../../docs/migration/detection-environment.md)
records every file/section, public naming, numeric identity semantics and explicit
behavior corrections. Base reuses existing standard/compiler/CPU/feature
providers; a separate optional `hps/platform/` reports visible OS/libc/POSIX
evidence without system includes. No duplicated `C_` detection system is added.
`HPS_` names macros/metadata constants; `hps_` names ordinary public C APIs.

Existing compiler/architecture IDs remain stable. Newly recognized vendors and
CPU families have additional IDs; explicit unknown metadata stays unknown.
Frontend versions and raw vendor releases have separate contracts. Standard
language flags and optional extension syntax probes are separate; the latter
enable no declaration or policy exception. Existing assertion, thread-storage
and annotation behavior remains unchanged. Branch expressions reuse the common
individual builtin probe, avoiding duplicate capability logic (HPS-C-017/018).

The source's bool re-export/fallback is explicitly omitted under HPS-C-001/004;
no bool/true/false or reserved-name definition is added. Flags remain direct
`uint8_t` where stored. Source OS/CPU family guesses are not API, libc or pointer
width evidence. Unknown OS does not mean embedded, missing Linux markers do not
mean musl, MSVC does not establish its linked C runtime, and POSIX family does
not establish an advertised POSIX version. Apple SDK and libc/unistd evidence
must be visible before first detection inclusion when required. The allowlisted
`HPS_CONFIG_LIBC` affects metadata only, rejects unknown/conflicting choices and
changes no ABI. It is not a hidden target implementation switch.

Public implementations move to `base/all.h`, `cstr/all.h`, `string/all.h`,
`span/define.h` and `data_structure/define.h`; Span/Data Structure gain `all.h`
umbrellas. Every root public header is now a one-include compatibility forwarder.
Moved generator bodies/layouts, capacities, ownership, operation names and error
codes remain unchanged. Active includes use canonical paths; historical reviews
and source snapshots retain their original scope. The
[layer map](../../docs/architecture/layers.md) and
[platform contract](../../docs/architecture/platform.md) record dependencies and
configuration. Fresh installation tests avoid modifying old `build/*-install`
snapshots visible in the user's editor.

## Bounds, threats and policy disposition

New detection headers contain only preprocessing metadata: no executable state,
runtime loops, heap/OS APIs, callbacks, locks or atomics. Existing pool resource
budgets and single-context ownership are unchanged. Compiler/SDK/library macros
and explicit configuration are integration trust boundaries, not validated
hardware facts. Incorrect target inputs can produce incorrect metadata; synthetic
tests do not qualify foreign generated code, ABI, library behavior or resources.
There is no new pointer manipulation, dynamic allocation, concurrency or runtime
error domain in this detection step.

No new project numerical/boolean exception is proposed. Existing base/compiler
extensions and test/enum/string policy findings remain unresolved. The preceding
allocator/clone proposal remains restricted by HPS-MEM-001 and is not approved
by this migration or the user's commit/push request. No MISRA, industrial,
security certification, performance or WCET claim is made.

Applicable policy: HPS-HDR-001 through 006; HPS-C-001/002/004/009/012/013/015
through 019; HPS-MEM-001/002/005; HPS-EXEC-001 through 003; HPS-SEC-001/002/007;
HPS-GOV-004/005/006/008/009; HPS-PRO-001/002/003; HPS-VER-001 through 006;
HPS-REL-001/003/004. Runtime contracts in the preceding memory step retain their
own detailed policy mapping and proposed disposition.

Relevant MISRA rows: Directives 1.1, 4.1, 4.6, 4.9, 4.10 and 4.11;
Rules 1.1, 1.2, 1.3, 5.1, 5.4, 5.6, 8.2, 8.3, 8.8, 8.10, 10.1, 10.3, 10.4,
20.1, 20.4, 20.5, 20.6, 20.7, 20.8, 20.9, 20.10, 20.11, 20.12, 20.13, 20.14.
Matrix evidence points here with all existing dispositions/deviations preserved;
earlier evidence is retained below. Target essential-type, predefined macro,
reserved identifier, expression expansion and licensed guideline analysis remain
open. Nothing here approves a mandatory-guideline deviation.

## Executed verification

Host: x86_64 Linux, GCC/G++ 11.4.0, Clang/Clang++ 14.0.0, Python 3.10.12,
CMake 3.22.1 and Meson 0.61.2. Evidence scope is the current final working tree,
not historical text/memory builds. Assertions remain enabled with `-UNDEBUG`.

| Actual check | Configuration and result |
| --- | --- |
| Strict Make | `make test BUILD_DIR=build/detection-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`; 17 shared suites, 293 portability checks and 105 focused detection checks PASS. |
| CMake Release | `cmake -S . -B build/detection-cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS='-Wall -Wextra -Wpedantic -Werror'`, build `--parallel 4`, then `ctest --test-dir build/detection-cmake --output-on-failure`; 3/3 PASS. |
| Meson/Clang | `CC=clang meson setup build/detection-meson -Dwerror=true`, compile, then `meson test -C build/detection-meson --print-errorlogs`; 3/3 PASS. Default C99/warning level 3 retained. |
| Independent repeated C++ headers | All 55 public headers with G++/Clang++ in C++11 and C++17, strict warnings: 220 independent repeated-inclusion checks PASS. Every root shim is included in the matrix; no bool/true/false macro injection. |
| Mixed C99/C++ consumers | Eleven String/Cstr/Memory sources compiled independently as C99 by GCC/Clang (22 objects); four C++11/17 callers including canonical umbrellas and all seven shims linked/executed PASS. C-only builtin/syntax probes remain zero in C++. |
| Fresh installed package | `cmake --install build/detection-cmake --prefix "$PWD/build/detection-install"`; separate C++17 `find_package(HPS CONFIG REQUIRED)` consumer links `HPS::cstr`/`HPS::memory`, includes new paths/all shims and executes PASS. Sources/commands are under `build/detection-consumer/`. Old installations are untouched. |
| Source integrity | All nine read-only originals match both retained snapshot and original inventory; five moved headers reproduce previous contents after path/guard substitutions. Earlier user drafts use lossless JSON encoding to preserve original whitespace without adding source-format findings. Existing status/deviation fields of all 221 MISRA rows are preserved. |
| Required policy subset | `python3 tools/check_policy.py`: exit 1, 153 retained findings; no new detection/layer finding. Full result remains FAIL; this lexical check is not MISRA analysis. |
| Whitespace | `git diff --check`: PASS; staged check is performed before committing. |

The C++ checks use `-Wall -Wextra -Wpedantic -Werror` and the respective language
standard. Temporary script/report: `/tmp/hps_cpp_layer_review.py` and
`/tmp/hps-cpp-layer-review-riywfxcn/report.json`. The retained detection suite
checks derivative priorities/versions, C/C++ and standard/extension separation,
unknown CPU width, explicit unsupported byte order, x32 pointer width, overlapping
OS families, unknown libc, conflicting hints and advertised versus requested
POSIX levels. Foreign synthetic configurations are never executed. Existing
portability tests cover independently/repeatedly included C99/C11/C17/C2x
headers, C89 subsets, freestanding Base and negative contracts.

Separate agent reviews found one explicit-byte-order precedence defect during
development; it was corrected before the final tests, with regression cases for
x86/Windows plus unsupported/mixed order. No blocking source/test defect remains
under the documented host/caller contracts. Named project-role review, vendor
toolchains/SDKs, embedded/RTOS targets, target ABI/resources/libc/callback evidence,
full MISRA analysis, proposed policy dispositions and release acceptance remain
open. No new sanitizer result is claimed for this detection/folder task.

## Retained earlier matrix evidence

Earlier evidence for updated rows is preserved below; none is relabeled current
target qualification by linking this review.

| Earlier evidence | Rows |
| --- | --- |
| [data-structure-2026-10-18.md](data-structure-2026-10-18.md) | Rule 5.1, Rule 5.4, Rule 5.6, Rule 20.7, Rule 20.10, Rule 20.11, Rule 20.12 |
| [memory-layers-2026-10-19.md](memory-layers-2026-10-19.md) | Dir 4.1, Dir 4.6, Dir 4.9, Dir 4.10, Dir 4.11, Rule 1.3, Rule 8.2, Rule 8.3, Rule 8.8, Rule 8.10, Rule 10.1, Rule 10.3, Rule 10.4, Rule 20.1 |
