# Module test entry-point change — 2026-10-11

Status: development change; compiled behavior unverified; policy exception proposed.
Author: Codex. Independent reviewer and approving authority: not assigned.
Source base: c2d9915e4aae25a7e9ebe4cb71c068bc0d5affd6 plus this working-tree change.
Authorization: the user explicitly requested callable module test functions such
as `int test_bytes(void)` instead of separate module `main` definitions. This
authorizes preparing the change; it does not establish independent governance
approval. The previously recorded no-build constraint is retained.

## Requirement and design

HPS-C-013 and HPS-VER-001/002/004: `test_base`, `test_pool` and `test_bytes` match
their filenames and share declarations in the internal `tests/test_suites.h`.
Only `tests/main.c` defines the native runner's hosted `main`. It invokes each
suite once, base then pool then bytes, and stops on a nonzero result with process
status 1, 2 or 3 respectively. Assertion failures terminate the process. Pool tests
retain their initial uninitialized-state requirement; the base suite does not use
the pool. No suite is retried or run concurrently.

Make, CMake and Meson link all suites plus the memory and bytes libraries into
`hps_tests`. All assertion-based suites compile with NDEBUG undefined, including
Release configurations. Existing individual test executable names/CTest/Meson
registrations migrate to `hps_tests`. The independent base portability probes
append their own hosted `main` wrapper calling `test_base`; the in-memory source's
relative private header include resolves using the explicit tests/base include
path. Generated probe entry points remain separate from module test source files.

No production source/header, library API/ABI, payload contract, static pool budget,
ownership, concurrency model, heap/OS dependency or arithmetic is changed. Runner
status literals are representable in hosted int; no new numeric state is stored.
This affects only hosted development verification, with no qualified target claim.

## Proposed project-policy exception: HPS-TEST-INT-001

Policy version: 0.1.0. Type: project exception, not an external-standard deviation.
HPS-C-001/005 currently permit basic int only for hosted `main`; the three suite
definitions and their three private prototypes depart from that rule. The exact
scope is `int test_base(void)`, `int test_pool(void)` and `int test_bytes(void)` in
their corresponding module C files and `tests/test_suites.h`. It does not extend to
production declarations, variables, parameters or arbitrary helper functions.

Reason: the user specified these int signatures. Conforming `int32_t` signatures
were considered but would not reproduce the requested interface. Return values
are bounded small status literals (base 0, 1, 2, 3, 5; pool/bytes 0). Hosted int can
represent them without width assumptions or conversion. A common private header
keeps caller/definition declarations identical, and module behavior is unchanged.
The remaining risk is an unresolved project type-policy exception, alongside
uncompiled linkage/control flow and future suite additions requiring review.

HPS-GOV-006/008/009: this proposal does not edit the normative numerical policy or
suppress its checker findings. No approval, named independent reviewer or external
MISRA deviation is claimed. No approved validity period exists. Revalidation is
required if signatures, return ranges, runner order, targets or tools change.
Remove the proposal if exact-width test returns are selected; formal acceptance
requires named review/authorization and an explicitly scoped policy change.

Relevant MISRA rows: Directive 4.6 and Rules 8.2, 8.4, 8.5, 8.6. Linked rows remain
unassessed. The production-and-dependency matrix scope is unchanged: these links
are a review cross-reference for the test harness, not production compliance
evidence or a completed external-standard assessment.

## Verification and residual limitations

`python3 tools/check_policy.py` returned exit status 1 with exactly six HPS-C-001
basic-type findings: tests/base/test_base.c:36, tests/bytes/test_bytes.c:219,
tests/memory/test_pool.c:149 and tests/test_suites.h:14/15/16. No other source or
governance findings were reported. These are the requested int signatures; no
passing policy result is claimed.

`git diff --check` passed. `make -n test` showed the shared executable, all four
test translation units, all three core translation units, enabled assertions and
the separate portability command; this was a dry run, not execution evidence.
Python 3 AST parsing of tests/base/check_portability.py passed without importing or
executing that compiler-driving script. A second agent reviewed source membership,
assertion settings, header resolution, suite ordering and failure handling; this
is author-side review assistance, not an independent project-role approval.

No C configure/build, executable test, portability compiler probe, analyzer or
target qualification is performed. Earlier validation/source-review snapshots
are historical for the changed test/build files (HPS-GOV-005, HPS-REL-001/003).

## Checked source snapshot

| File | SHA-256 |
| --- | --- |
| tests/main.c | `9e1ab0266effb4144c635960042d8f3974c101b965c504250b09d3b00aa68a44` |
| tests/test_suites.h | `72288cb9006c45d2e2e53e1a6da3fdcdf325af8b1f1f4703737f8988e43b33ee` |
| tests/base/test_base.c | `6367e4b5b316ec8089063949541aac210e29981c6fa57e6858513912237ff760` |
| tests/memory/test_pool.c | `4d123e46a6971cad6aae53b6d0eb3f7d3ddc6df58f6291b39777ebd99e95d9e1` |
| tests/bytes/test_bytes.c | `a599394cfe74433c8cf67fe6ae08d1061af72e812de94fa1f2dc442d248fa844` |
| tests/base/check_portability.py | `862dfe3e0d0d2b817ed8aa372a1d76b878a4bf55cbf652fa9b8fd38db10ba2fe` |
| CMakeLists.txt | `ec2712534e46ee0a434436688fb342ad02efc891867fcb4d4ad400d2f8b17e08` |
| Makefile | `f57ae24d76fcc5f5575db8538568188d4da29486e2f92a7e87f31892268bd8ce` |
| meson.build | `16d10e66a4523b2314517d0fe958f333b8f6259feed25deed0634fb1b7eb277b` |
| governance/misra/matrix.csv | `7d191bbd7a6ba74d1a8c51215f38cf20f853aa82f2c45d83ec13b8a4a8f4099e` |
