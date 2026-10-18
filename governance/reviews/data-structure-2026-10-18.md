# Supplied data-structure generators — 2026-10-18

Status: user-authorized development implementation; named project-role review
and release/target acceptance pending. Author: Codex. Independent named reviewer:
unassigned. Source base: `1a53bb9809f1dff4de70de393b09400d7c8c14a9` (the preceding
status-removal commit) plus this change. Profile: hosted,
single-context development; no deployment target is qualified.

The user supplied `include/hps/hps_data_structure.h` and requested that each of
its eleven types use the existing `HPS_SPAN_DEFINE` pattern, followed by commit
and push. This work uses that supplied file and current HPS code. No source or
artifact is imported from either read-only legacy tree. The unrelated supplied
`hps_allocator.h` draft is outside this change.

## Design and migration

The new public header uses the path-derived `HPS_HPS_DATA_STRUCTURE_H` guard,
`@file`/`@brief` metadata and explicit dependencies. Each generator accepts an
explicit caller-chosen prefix and produces a named structure, `_t` typedef and
internal constructor. It does not add an external API/callback, so an empty C
linkage block is unnecessary. There is no source file, runtime global state,
shared status/error type, error registry, allocation or OS dependency.

| Supplied draft | Implemented generator |
| --- | --- |
| `HPS_DEFINE_VECTOR(T)` | `HPS_VECTOR_DEFINE(type_, name_)` |
| `HPS_DEFINE_SPAN(T)` | `HPS_MUTABLE_SPAN_DEFINE(type_, name_)` |
| `HPS_DEFINE_CSPAN(T)` | `HPS_CSPAN_DEFINE(type_, name_)` |
| `HPS_DEFINE_ROBUFFER(T)` | `HPS_ROBUFFER_DEFINE(type_, name_)` |
| `HPS_DEFINE_SVECTOR(T, N)` | `HPS_SVECTOR_DEFINE(type_, name_, capacity_)` |
| `HPS_DEFINE_SRING(T, N)` | `HPS_SRING_DEFINE(type_, name_, capacity_)` |
| `HPS_DEFINE_RING(T)` | `HPS_RING_DEFINE(type_, name_)` |
| `HPS_DEFINE_STACK(T)` | `HPS_STACK_DEFINE(type_, name_)` |
| `HPS_DEFINE_SLIST(T)` | `HPS_SLIST_DEFINE(type_, name_)` |
| `HPS_DEFINE_DLIST(T)` | `HPS_DLIST_DEFINE(type_, name_)` |
| `HPS_DEFINE_BST(T)` | `HPS_BST_DEFINE(type_, name_)` |

The draft was untracked and had no existing committed consumers. No compatibility
aliases for `HPS_DEFINE_*`, type-spelling-derived names or draft token-concatenation
helpers are retained. Callers explicitly choose their replacement prefix and use
`name_t`/`name_make`; node types additionally use `name_node_t`/`name_node_make`.
All field types and ordering from the supplied structures are preserved. No
native binary/wire/persistence compatibility is promised for this new API.
The existing `hps/span.h` read-only API is unchanged; `CSPAN` delegates to it and
the supplied mutable span gains its own unambiguous family name.

Every family exposes `MAKE` and `STATIC_INIT` helpers. C99 uses the existing
compound-literal expression facility; the fallback uses internal functions.
Runtime arguments are evaluated once, with no promised order between arguments.
Static initialization requires suitable constant expressions. Generated node
constructors accept the payload and explicit link fields, with no traversal.
Static vector/ring constructors take no arguments and return an empty object
with every element and metadata field initialized. Capacities must be positive
integer constant expressions; a generated typedef rejects nonpositive values.
The [architecture contract](../../docs/architecture/data-structure.md) records
each field, constructor order, invariants, ownership and initialization behavior.

The draft described several pointer-backed structures as heap owners and the
static ring as suitable for ISR messaging. Those statements were not implemented
operations or supported guarantees. The implemented API documents caller-owned
storage/nodes and descriptor construction only: no automatic acquisition,
growth, copying of backing arrays, release, insertion, balancing or concurrency
mechanism is introduced. Users must separately establish typed extents,
`length <= capacity`, valid ring indexes, consistent live links/counts and any
tree ordering before using them. Constructors are unchecked, like aggregate
initialization; there is no error return or recovery from forged metadata,
cycles, dangling pointers or inconsistent tree state.

## Bounds, resources and policy mapping

All counts/indexes/capacities are `size_t`; element types are caller-selected
complete object types subject to HPS's numerical policy. C++ consumers are
restricted to trivial C-compatible element types so user-defined constructors or
copies cannot introduce hidden allocation/resource behavior. No basic numerical
declaration or byte-wise reinterpretation is introduced. Static payload arrays
have compile-time extents, not VLAs. Aggregate/value initialization creates typed
zero/null values; no all-bits-zero assumption, padding comparison or native
serialization contract is added. Metadata/payload copies retain normal C/C++
type semantics.

Pointer-backed construction has fixed metadata work; node construction copies
one typed payload and a fixed number of links. Static vector/ring initialization
and by-value copying are O(capacity), bounded by the selected positive capacity.
Payload, metadata, automatic result/return temporaries and object-copy budgets
must be measured for a target. No target timing, WCET, measured performance,
whole-product no-heap, secure-erasure, DMA or ISR/thread safety claim is made.
Callers retain ownership, prove storage/node lifetime and serialize conflicting
access/mutation/release. Const views do not prevent mutation through aliases.
Untrusted external lengths, capacities and links must be validated before use;
these constructors are not parsers or a fault-containment mechanism.

HPS-C-017/HPS-C-018: the read-only span family reuses the existing span generator
and construction helpers. VECTOR and STACK intentionally repeat their identical
descriptor/constructor shape to expose separate public generator/type identities
and keep each expansion directly reviewable. Other descriptor/node declarations
differ in pointer qualifications, field order or link contracts. Static arrays
have different initialization/resource contracts from pointer descriptors; ring
metadata has a different invariant from plain spans. This bounded declaration
repetition avoids a private macro language that hides those public shapes; no
duplicate allocation or mutation algorithm is introduced. Generated helper names
and constructor parameters remain reviewable.

Applicable policy IDs: HPS-HDR-001 through 006; HPS-C-001/002/006 through 018;
HPS-MEM-001/002/005; HPS-EXEC-001 through 003; HPS-SEC-001/002/003/005/007/009;
HPS-GOV-004/005/006/008/009; HPS-PRO-001/004; HPS-VER-001 through 006 and
HPS-REL-003/004. No project policy relaxation is proposed for the production
header. Existing base token-concatenation/compound-literal provisions remain
subject to guideline review, and existing native-int test-suite signatures remain
the separately proposed [test-only exception](test-entrypoints-2026-10-11.md).

Relevant MISRA review rows: Directives 4.1, 4.6, 4.8, 4.9, 4.10, 4.11 and 4.14;
Rules 1.3, 5.1 through 5.9, 8.2, 8.3, 8.8, 8.10, 8.13, 9.1 through 9.3, 10.1,
10.3, 10.4, 11.9, 13.1, 13.2, 17.7, 17.8, 18.1, 18.6, 18.8, 20.1, 20.7,
20.10, 20.11 and 20.12. The matrix points the affected evidence fields at this
review and preserves all dispositions. Token concatenation, generated identifiers,
macro argument effects, typed initialization and target essential types still need
full licensed-guideline/target analysis. No deviation or disposition is approved
by this document. The following table retains all earlier evidence links from
the affected rows:

| Retained earlier evidence | Affected rows previously pointing to that record |
| --- | --- |
| [status-removal-2026-10-18.md](status-removal-2026-10-18.md) | Dir 4.1, Dir 4.6, Dir 4.8, Dir 4.10, Dir 4.11, Dir 4.14, Rule 1.3, Rule 5.1, Rule 5.8, Rule 8.2, Rule 8.3, Rule 10.1, Rule 10.3, Rule 10.4, Rule 17.7, Rule 18.1 |
| [enum-traits-2026-10-13.md](enum-traits-2026-10-13.md) | Dir 4.9, Rule 20.1, Rule 20.10, Rule 20.11, Rule 20.12 |
| [bytes-codec-2026-10-13.md](bytes-codec-2026-10-13.md) | Rule 8.13, Rule 11.9 |
| [csep-text-2026-10-13.md](csep-text-2026-10-13.md) | Rule 9.1, Rule 9.3, Rule 17.8 |
| [bytes-reader-2026-10-11.md](bytes-reader-2026-10-11.md) | Rule 18.8 |

## Verification and remaining gaps

Host: x86_64 Linux; GCC/G++ 11.4.0, Clang 14.0.0, Python 3.10.12,
CMake 3.22.1 and Meson 0.61.2. Assertions remain enabled with `-UNDEBUG`.
`test_data_structure(void)` is registered in `tests/test_suites.h`, the shared
runner and all three build manifests. This new suite uses an ordinary `int32_t`
result and introduces no additional native-int signature exception.

| Actual check | Configuration and result |
| --- | --- |
| Make shared suites and portability | `make test BUILD_DIR=build/data-structure-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`; all 15 suites and 233 portability checks PASS. After the test-only constructor addition below, `make all` and the rebuilt shared executable again PASS. |
| CMake Release | `cmake -S . -B build/data-structure-cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS='-Wall -Wextra -Wpedantic -Werror'`, build with `--parallel 4`, and `ctest --test-dir build/data-structure-cmake --output-on-failure`; 2/2 PASS. After the test-only addition, the affected executable rebuild and `ctest -R '^hps_tests$'` again PASS. |
| Meson/Clang | `CC=clang meson setup build/data-structure-meson -Dwerror=true`, then `meson compile -C build/data-structure-meson` and `meson test -C build/data-structure-meson --print-errorlogs`; build and both tests PASS after the test-only correction. Default C99 and warning level 3 are retained. |
| Function fallback | Final data-structure suite plus a small hosted runner compiled/executed with GCC, `-std=c99 -Wall -Wextra -Wpedantic -Werror -UNDEBUG -Iinclude`, preincluding a header that includes `hps/base/expression.h` then undefines `HPS_LVALUE`; PASS. |
| C++ consumers | The same final suite/runner compiled and executed with `g++ -x c++`, each of `-std=c++11` and `-std=c++17`, and the same strict warnings/assertion flags; PASS. Header is included twice by the suite. |
| Rejected invalid declarations/writes | Separate agent check: seven expected failures in each of C99 and C++11, 14 total; all rejected. Cases: zero/runtime capacities for both static vector and static ring; writes through scalar const span and read-only buffer; replacement of a pointer element through a const span using a pointer typedef. |
| Whitespace | `git diff --check`; PASS. |

The 233 portability checks describe the actual working tree, including the
unrelated allocator draft's independent header checks. That allocator remains
untouched and excluded from this change's commit. No separate portability count
for a checkout without that draft is presented as an executed result. Temporary
direct-check inputs/results were under `/tmp/hps-structures-review-u7eunxf7`;
the runner declares/calls `int32_t test_data_structure(void)` from its sole hosted
`int main(void)`. The suite itself is retained in the repository. Separate agent
source feedback is verification input, not an assigned project-role approval.

The tests exercise every supplied family and helper, direct constructors, null
empty descriptors, wrapped ring metadata, field order, single argument evaluation,
static initialization, live borrowed aliases and mutable/const access. Nested
structure payloads verify full typed initialization and independent embedded-array
copies. Pointer-typedef spans verify element constness while allowing the documented
pointee access. Node tests verify explicit links and copies of structure payloads.
Meson's first strict Clang build found unused generated constructors in the
test-only pointer-span/structure-tree instantiations. The test now exercises those
direct constructors; warning flags and production behavior were unchanged. The
affected Make/CMake executable checks and complete Meson checks were rerun.

The initial working-tree policy check reported 167 findings: 153 retained earlier
findings, 11 in the unrelated supplied allocator draft and three public-header
structural findings in the supplied data-structure draft. The current
`python3 tools/check_policy.py` returns 1 with 164 findings: the same 153 prior
lexical findings and 11 in the untouched, uncommitted allocator draft. This header's
three guard/metadata findings are resolved, with no additional checker finding.
The full policy result remains a failure and is not a MISRA analysis. Historical
status/text/span checks do not verify this change. No sanitizer or additional
target result is claimed for this task.

Named independent review, a licensed MISRA analysis, target ABI/layout and
resource budgets, full target/compiler coverage, deployment profiles and release
acceptance remain open. Hosted checks cannot qualify embedded targets or
establish MISRA compliance, industrial readiness or certification.
