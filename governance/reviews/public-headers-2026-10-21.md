# Public-header package layout review — 2026-10-21

Status: user-authorized development reorganization. Author: Codex; named
maintainer and independent project reviewer unassigned. Source base:
`9d597f8178e1bef2fba18b6ffaa334778a90a57b` plus this change. Profile: hosted,
single-context development; no deployment target is qualified. Source audits
performed by collaborating agents provide review inputs, not assigned project-role
approval. No existing deviation, exception or release status is approved here.

## Requirement and observable change

The user requests removing root headers except the Base umbrella, placing each
header in the appropriate layer, splitting Data Structure by type, applying
matching named package umbrellas everywhere, then committing and pushing the
result. Only `include/hps/base.h` remains at the root, directly aggregating Base.
Other packages use `<package>/<package>.h` umbrellas and `hps_<name>.h` leaves.
Base architecture/features retain nested directories and named nested umbrellas.
Root/span compatibility includes and every `all.h` are removed. This intentionally
breaks old source include paths and path-derived guard spellings; the
[complete migration map](../../docs/migration/public-headers-2026-10-21.md)
records every old path, the per-type split and continuation instructions.

Data Structure has thirteen type leaves plus common collection-error and
static-initialization providers. Vector/dlist operations move beside their own
descriptors. No renamed `define.h` or `operations.h` aggregate preserves the
original monolith. The named umbrella includes all fifteen public leaves.
Data Structure macro definitions, C names, type layouts, errors, mutation order,
evaluation/initialization rules, ownership and resource contracts are preserved.
No function-return width, numeric-code or configuration ABI change is intended.
Callers must update includes and regenerate/reinstall from fresh build inputs.

Complete-header coexistence exposed a pre-existing Bytes/Base collision:
the unused Bytes `hps_endian_t` enum declared `HPS_ENDIAN_LITTLE/BIG` = 0/1,
but Base's detection macros use those names with values 1/2 (unknown = 0).
The unused Bytes enum/typedef is removed from `hps/bytes/hps_endian.h`; Base
metadata and the existing codec
`uint8_t hps_byte_order`/`HPS_BYTE_ORDER_*` selector values 0/1 are preserved.
This is an explicit source type/constant removal, not an enum-storage-compatible
rename. Existing exported function signatures, error codes and codec behavior
are unchanged. Consumers of the removed type must migrate deliberately using the
[source/API instructions](../../docs/migration/public-headers-2026-10-21.md#removed-conflicting-bytes-endian-type).

All package umbrellas include their package's public declarations. Memory's new
complete umbrella includes the existing ownership proposal headers; inclusion
provides no allocator, calls no provider and changes no compiled core dependency.
Their HPS-MEM-001 disposition remains pending. Base and other independent modules
still do not depend on them. Platform metadata retains its preprocessing-only
boundary; its umbrella omits the separately selected hosted package umbrella.
The hosted debug target remains default-disabled and explicitly linked.

Current architecture/README examples, source/test/checker paths and build/install
inputs use the current names. Historical migration reports, byte snapshots and
review evidence are retained with a supersession pointer in the continuation
inventory; historical successful tests are not current verification.
The span suite source moves from `tests/span/` to `tests/data_structure/` with
the same callable `test_span` entry point and registration in all build manifests.
Overlay installs do not prune removed files; the current verification prefix is
`build/header-layout-install`, leaving earlier generated prefixes historical.

## Policy and MISRA traceability

Applicable requirements: HPS-HDR-001 through 006; HPS-C-001/004/009/012/013/015/
017/018/019/020; HPS-MEM-001/002/005; HPS-EXEC-001; HPS-PRO-001/002/003;
HPS-GOV-004/005/006/008/009; HPS-VER-001/002/003/004; HPS-REL-003/004.
Header checks target guard uniqueness, metadata, explicit dependencies, repeated
independent inclusion, declaration linkage and installed-consumer migration.
Runtime checks target preservation of the existing type and error contracts.

HPS-C-017/018 rationale: each type's declarations and matching operations are
locally reviewable, with its relevant ownership/storage contract. Collection
errors and language-correct empty initialization each have one provider; cspan
uses the single span implementation. Type-specific generator macros retain their
existing distinct fields/resource bounds rather than adding an untyped generic
construction layer. Shared constructor-purpose comments repeat across leaves so
each independently included header carries its own preconditions; no repeated
runtime algorithm is introduced. The original macro expansion bodies are retained.

Relevant matrix rows: Directives 1.1, 4.6, 4.9, 4.10; Rules 5.1, 5.4, 8.2,
8.3, 8.10, 10.1, 20.1, 20.7, 20.10, 20.11, 20.12, 20.14. Selected evidence links
point to this record, with previous links retained below. All 221 rows keep their
categories, scope, owner, method, existing status and deviation fields. This is
traceability, not licensed semantic analysis or a MISRA compliance claim.

No heap, OS, mutable global, synchronization or new storage dependency is added.
Existing single-context ownership, static capacities, checked arithmetic and
failure-preservation contracts remain applicable. Existing enum, native-int suite,
character-storage and allocator exception proposals remain unresolved. Include
order/configuration and arbitrary pointer validity remain caller/target duties.

## Current verification

Host: x86_64 Linux; GCC/G++ 11.4.0, Clang/Clang++ 14.0.0, Python 3.10.12,
CMake 3.22.1 and Meson 0.61.2.
Evidence covers the source base above plus this working tree. Baseline
`python3 tools/check_policy.py` before the layout change exited 1 with 153
existing findings; the current result is exit 1 with 152. The removed finding is
the obsolete Bytes enum declaration, not a suppression. These are failing policy
results; the baseline is used only to identify additions/removals.

| Check | Current result |
| --- | --- |
| Declaration/macro preservation audit | All 56 pre-existing Data Structure public macro definitions/alternate bodies match the split definitions byte-for-byte. Of 55 relocated leaves reviewed, 53 match after intended path/guard normalization; Bytes endian removes the documented obsolete enum and Bytes order adds a warning about the distinct detection/selector numeric domains. Agent source-audit input; not project-role acceptance. |
| Strict GCC/CMake with optional hosted adapter | Configure `build/header-layout-cmake` with Debug, `-DHPS_BUILD_HOSTED_DEBUG=ON` and `-DCMAKE_C_FLAGS='-Wall -Wextra -Wpedantic -Werror'`, build, then `ctest --test-dir build/header-layout-cmake --output-on-failure`: 8/8 PASS. |
| Strict GCC/Meson with optional hosted adapter | Configure `build/header-layout-meson` with `-Dwerror=true -Dhosted_debug=true`, compile and run tests: 8/8 PASS. Set `CCACHE_DIR`/`CCACHE_TEMPDIR` under `build/header-layout-ccache` because the default `/run/user` cache is outside the writable sandbox; this changes only the build cache location. |
| Strict Make with default modules | `make test BUILD_DIR=build/header-layout-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`: PASS, with hosted diagnostics disabled by default. |
| Independent/repeated C and C++ public-header inclusion | GCC and Clang each PASS 236 checks across all 85 public headers: C99/C++11 independent/repeated inclusion, package/header coexistence, direct Data Structure constructor/operation fixtures and mixed C/C++ linkage. |
| Fresh installed package and consumer | `cmake --install build/header-layout-cmake --prefix "$PWD/build/header-layout-install"`, followed by `python3 tests/base/check_headers.py --compiler cc --include-dir build/header-layout-install/include`: 236/85 PASS. Independent C++11 CMake consumer in `build/header-layout-consumer` uses `find_package(HPS CONFIG REQUIRED)`, imported HPS targets for every compiled core library, installed headers and `-Wall -Wextra -Wpedantic -Werror`; build/run PASS. |
| Policy checker/self-tests and baseline comparison | `python3 tools/check_policy.py --self-test`: 16 implementation-scope, 18 standard-provider, 16 lexical, eight header and six invalid-governance fixture checks PASS. Repository command exits 1 with 152 known findings. Path-normalized old/new finding multisets add zero and remove only the obsolete Bytes enum finding. |
| Migration links, matrix integrity and whitespace | Current-document links PASS; all 221 MISRA rows retain non-evidence fields unchanged. `git diff --check` PASS. |

Remaining gates include independent named review, licensed MISRA/essential-type
analysis, all unresolved policy proposals, actual target/compiler/ABI/charset/libc
qualification and resource/stack/timing evidence. No industrial qualification,
certification, measured performance, WCET or whole-product no-heap claim follows.

## Retained earlier matrix evidence

| Previous evidence | Rows |
| --- | --- |
| [standard-types-2026-10-20.md](standard-types-2026-10-20.md) | Dir 1.1, Dir 4.6, Dir 4.9, Dir 4.10, Rule 5.4, Rule 8.2, Rule 8.3, Rule 8.10, Rule 10.1, Rule 20.1, Rule 20.7, Rule 20.10, Rule 20.11, Rule 20.12, Rule 20.14 |
| [bytes-features-optional-2026-10-19.md](bytes-features-optional-2026-10-19.md) | Rule 5.1 |
