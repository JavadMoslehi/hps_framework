# Remove shared status infrastructure — 2026-10-18

Status: user-authorized development change; project-role review and release/target
acceptance pending. Author: Codex. Independent named reviewer: unassigned.
Source base: `c0218429e5433516586142b003d78d755ceca728` plus the working tree.
Profile: hosted, single-context development; no qualified deployment target.

The user requested complete removal of the shared status system and its
dependencies, replacing its results with ordinary types appropriate to each API.
This authorizes implementation and the corresponding project-policy amendment.
It does not approve an external-standard deviation or close a release gate.

## Final design and policy change

Governance version 0.2.0 becomes 0.3.0. HPS-C-004/HPS-C-019 replace shared status
types, global partitions, module-ID allocation and diagnostic registration with
ordinary API types and local error domains. HPS-C-017/HPS-C-018 readability and
matching-contract reuse rules remain unchanged. Contributor instructions, header
dependencies and the public-header template follow this amendment.

Remove the shared header/source, framework constants, module IDs, message-capacity
contract, range classification, immutable message registry and diagnostic lookup
APIs. Remove their build/export dependencies and dedicated test suite. The earlier
[shared-status proposal](status-2026-10-11.md) is superseded only in this scope;
its evidence and historical policy state remain available.

Byte, pool and text error-returning operations now declare `int32_t` directly
from `<stdint.h>`, with no framework or module status typedef. Existing
`HPS_BYTE_*`, `HPS_POOL_*` and `HPS_STRING_*` error names remain typed constants.
Zero is success; negative errors identify failures within the called API's domain.
String, Cstr, builder/policy and dependent validation operations share the
documented string error domain. Bytes and pool have separate domains, so identical
numeric values across domains are intentional. Existing pointer-returning pool
operations, scalar counts, flags, comparisons and output pointers keep their
documented meanings and types. No allocating message or Boolean error adapter is
introduced; distinguishing the existing failure reasons remains possible.

No parsing, ownership, initialization, error priority, mutation order or failure
preservation change is intended. In particular, pool allocation/free retain their
pointer/void signatures and communicate local errors through
`hps_pool_last_status()`, which now returns `int32_t`; pool stats reads preserve
that last result. Byte/text failures preserve outputs and destination storage.
All original capacities, bounded loops, checked arithmetic and caller-supplied
scratch requirements remain. The pool stays single-context with no heap, OS,
locks, atomics, secret erasure or stale-alias-after-reuse guarantee.

## API/ABI and numeric-code migration

This is a deliberate source/API and binary ABI break, even where function and
constant names remain unchanged. All affected return values, callback signatures,
local result objects and the private pool last-result object move from `int64_t`
through removed aliases to direct `int32_t`. Public data descriptors do not gain
status fields; no wire/native layout compatibility is inferred.

| Error domain | Former negative codes, in declaration order | Replacement local codes |
| --- | --- | --- |
| Bytes: INVALID_ARGUMENT, OUT_OF_RANGE, NO_SPACE, INVALID_STATE | -131073 through -131076 | -1 through -4 |
| Pool: INVALID_ARGUMENT, EXHAUSTED, INVALID_POINTER, DOUBLE_FREE, NOT_INITIALIZED, ALREADY_INITIALIZED, CORRUPT | -65537 through -65543 | -1 through -7 |
| String/Cstr/dependent validation: INVALID_ARGUMENT, OUT_OF_RANGE, NO_SPACE, INVALID_STATE, NOT_TERMINATED | -196609 through -196613 | -1 through -5 |
| Success in each domain | 0 | 0 |

Consumers must replace `hps_status_t`, `hps_byte_status`, `hps_pool_status` and
`hps_string_status` declarations with `int32_t` and include `<stdint.h>` directly.
Replace generic success comparisons with the called API's local success constant
or a documented comparison with zero. Preserve the domain when storing or
propagating results; a bare local number cannot identify its originating module.
Persisted status numbers or foreign interfaces need an explicit application-level
translation using this mapping, not truncation or subtraction on arbitrary input.

`hps_status_module()`, `hps_status_string()`, `HPS_STATUS_*`, `hps/status.h`,
`HPS::status` and `hps-status` are removed, with no compatibility shim. Update
application error handling/logging in its own API context and drop the old
link/export dependency. Rebuild all affected libraries and C/C++ consumers
together; mixing old objects with new declarations is unsupported. Historical
registered module IDs and strings are no longer a supported diagnostic contract.

The old registry's storage and lookup work disappear, and result objects narrow,
but no measured ROM/RAM, stack, throughput or latency improvement is claimed.
Actual ABI, register/helper use and target resource effects remain unverified.

## Requirement and guideline mapping

- HPS-C-001/002/004/006/007/008/009/011/012/013/016/017/018/019:
  exact-width results/constants, preserved arithmetic and error contracts,
  explicit domains, ordinary signatures and visible mutation order.
- HPS-HDR-001 through 006: direct type dependencies, typed local constants,
  independently includable headers and guarded C linkage; consumers need ABI checks.
- HPS-MEM-001/002/004/005/006 and HPS-EXEC-001/002/003: unchanged static capacities,
  ownership and single-context operation; target budgets/timing are still open.
- HPS-SEC-001/002/003/005/006/009: failure context stays explicit, no allocation,
  unbounded diagnostics or secret-bearing message facility is added; existing
  misuse and fault-containment limitations remain.
- HPS-GOV-004/005/008/009, HPS-VER-001 through 004/006 and HPS-REL-002/003:
  versioned policy change, current evidence, retained history, API/ABI mapping and
  pending named project-role/release decisions.

Relevant MISRA rows are Directives 4.1, 4.6, 4.7, 4.8, 4.10, 4.11, 4.14 and Rules
1.3, 5.1, 5.8, 8.2 through 8.6, 10.1, 10.3, 10.4, 10.8, 15.5, 15.7,
16.1 through 16.7, 17.7 and 18.1. The Rule 16 references record removal of the
former registry switches; they do not assess all remaining project switches.
Affected matrix rows point here and remain unassessed. Exact-width spellings,
successful builds or lexical checks are not semantic MISRA dispositions.
Earlier [bytes](bytes-2026-10-11.md), [pool](pointer-pool-2026-10-11.md),
[string](string-2026-10-13.md) and [text](csep-text-2026-10-13.md) source/test
records retain their original scope and hashes. Their affected compiled/ABI
evidence is historical after this change.

## Verification and disposition

Before the change, `python3 tools/check_policy.py` returned 1 with 153 existing
lexical findings (retained during this session in
`/tmp/hps-status-policy-before.txt`). This establishes the baseline, not a passing
policy result. Existing native enum/basic-type and character-storage proposals
remain pending; the status removal does not approve them or change their scope.

Current checks performed for this working change:

| Check | Configuration/scope | Result |
| --- | --- | --- |
| Make tests | `make test BUILD_DIR=build/status-removal-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`; GCC 11.4 | PASS: all 14 remaining suites and 225 portability configurations |
| CMake/CTest | CMake 3.22.1; GCC 11.4; Release; `-Wall -Wextra -Wpedantic -Werror`; `build/status-removal-cmake` | PASS: build and 2/2 CTest entries |
| Meson tests | Meson 0.61.2; Clang 14; C99; `-Dwerror=true`; `build/status-removal-meson` | PASS: build and 2/2 test entries |
| Fresh installed C consumer | CMake install under `build/status-removal-install`; `find_package(HPS CONFIG REQUIRED)`; strict C99 consumer links `HPS::memory`, `HPS::bytes`, `HPS::cstr`, `HPS::validation`; `build/status-removal-consumer` | PASS: configure/build/link/run |
| C++ header/consumer linkage | G++ 11.4; C++17; `-Wall -Wextra -Wpedantic -Werror`; independent/repeated inclusion of all ten changed text headers and C++ consumer linked with fresh GCC C99 archives; `build/status-removal-cxx` | PASS: text/fixed macros, Cstr, validation, local negative errors and output preservation |
| Hosted pool example | Strict GCC C99 compile/run of `src/main.c` | PASS |
| Removal inventory | `rg` over `include`, `src`, `tests`, manifests and `cmake`; fresh installed headers/archives/exports and `nm` symbol inspection | No shared status reference, installed artifact, exported target or symbol remains |
| Source-preservation review | Seventeen remaining production/internal files, excluding the hosted example, compared with HEAD after status-alias replacement and whitespace normalization | Equivalent: no production logical change beyond result-type replacement; headers also replace error constants; example adds direct stdint dependency |

The build directories above are local reproducibility artifacts, not a qualified
release bundle. The dedicated registry test was removed with its API; module
suites remain the evidence for error priority, failure preservation, pool ownership
and local error handling. Historical compiled evidence is not reused here.

The final `python3 tools/check_policy.py` returned 1 with 153 existing lexical
findings, retained during this session in `/tmp/hps-status-policy-after.txt`.
Compared with the before-change baseline, the finding multiset is unchanged after
normalizing shifted source line numbers: no new or removed findings. Governance
link/matrix checks produced no findings. `git diff --check` passed. The overall
repository policy result remains nonzero; this is not a passing policy gate or
a semantic MISRA assessment.

Target ABI/resources/timing, qualified semantic analysis, all-guideline MISRA
assessment and named project-role/release acceptance remain open. No external
standard compliance, industrial qualification, certification, proven WCET or
measured performance claim follows from this change. GAP-018 remains open for
consumer migration and target/release assessment.
