# Sequential bytes reader change — 2026-10-11

Status: development-only; compiled behavior unverified. Author: Codex.
Independent reviewer and approving authority: not assigned; no acceptance asserted.
Source base: 645a86cf5e21d6049ff07f8907030b7351363c97 plus this working-tree change.
Authorization: the user accepted the proposed reader increment and explicitly
required governance, performance and security considerations for the ecosystem
folders. The standing no-build instruction is retained. Development verification
profile: hosted, single owner per reader, immutable shared backing if readers are
independent. Bare-metal/RTOS/industry adoption remains target-specific and unqualified.

## Requirement and implementation mapping

- HPS-C-001/002/004/013/016, HPS-HDR-001 through 006: one view plus size_t position,
  exact-width values/status, eleven named APIs, path-derived guard, @file/@brief,
  explicit dependencies and guarded C linkage. New test_reader returns int32_t,
  matching its filename and the numerical policy; the three old int suite
  signatures and their six findings remain under their existing proposed exception.
  No new exception, configuration switch, numeric typedef or ABI macro is added.
  Exact-width availability, public struct layout and mixed-language linking need
  actual target evidence; no supported target is inferred from declarations.
- HPS-C-006/007/008/009: reader_check establishes position <= length before
  subtraction. Skip checks count <= length-position before adding. Take obtains
  the same proof from a successful checked view slice. Successful u8 access proves
  position < length; endian reads prove each fixed width <=length-position. Thus
  every addition is <=length<=SIZE_MAX. Every failure is detected before modifying
  the reader/output. Delegated output writes precede cursor commit without a later
  fallible step; reader/output/payload disjointness is an explicit prerequisite.
  NULL required outputs take priority over malformed state or exhaustion.
- HPS-C-010/011/012/014, HPS-MEM-001/002/005: init uses a local normalized view,
  resets cursor only on success, and permits source view passed by value from the
  same existing reader. Borrowed slices do not copy or own payload. No unrelated
  pointer casts, pointer-integer conversions, escaping local array, heap/pool/OS
  dependency, VLA, loop, recursion, retry, mutable global or lock. Reader metadata,
  output objects and storage are typed, initialized, live and disjoint per contract.
  Empty take/skip never forms a pointer from NULL or end position.
- HPS-EXEC-001 through 003, HPS-PERF-001/002, HPS-PRO-001 through 004: all paths have
  fixed bounded metadata/call work; reads access at most eight bytes. Metadata is
  sizeof(hps_byte_reader); no payload scratch array is added. Bounds remain enabled.
  There are direct calls, not runtime callbacks. Compiler/libc helpers inherited
  from exact-width arithmetic, actual stack/ROM/ABI, latency distributions and WCET
  require target evidence. No measured high-performance, zero-overhead, DMA or
  secret constant-time claim is made. Caller serialization is retained.
- HPS-SEC-001/002/003/005/006/009: untrusted counts/truncated fields fail before
  access or state/output commit; invalid paths have bounded work. This consumes
  bytes without protocol validation or authentication. Fields are committed per
  operation, so whole-message validation must precede external effects at the
  integration boundary. Backing mutation/rebind/release invalidates borrowed
  readers under the existing contract. No secret logging/erasure/cryptography or
  arbitrary invalid-pointer/RAM-fault containment is introduced or promised.
- HPS-VER-001 through 004/006, HPS-GOV-004/005/008, HPS-REL-001/003: new contract,
  prepared state/boundary tests and review cross-references; no new main in module
  tests. Make/CMake/Meson register reader.c, test_reader.c and the existing shared
  runner calls the suite once. Existing pool/view/buffer layouts and behavior are
  unchanged; reader is additive. Earlier evidence for changed manifest/runner/header
  files becomes historical. No independent approval or C test success is invented.

Relevant MISRA identifiers: Directives 4.1, 4.6, 4.7, 4.10, 4.11, 4.14 and Rules
1.3, 8.2, 8.4, 8.5, 8.6, 10.1, 10.3, 10.4, 10.6, 10.7, 10.8, 12.2 (inherited
endian shifts), 17.7, 18.1, 18.4, 18.8. Matrix links remain unassessed; prior
review links are preserved. This author/source check does not close semantic,
essential-type or external-standard findings.

## Ecosystem scope and unresolved integration requirements

The local ecosystem directory snapshot contains eight empty folder placeholders,
not implemented/qualified products or authoritative target specifications.

| Workspace ecosystem | Existing baseline/overlay mapping | Integration evidence still required |
| --- | --- | --- |
| hps-telecom | Core, HPS-TEL-001/002/003, HPS-PERF-001/002 | Actual interface/version/schema, input admission limits and target latency/resources |
| hps-open5gs | Core, HPS-TEL-001/002/003, HPS-PERF-001/002 | Selected Open5GS integration/dependency/ownership boundaries and protocol scope; no integration is implemented |
| hps-automotive | Core, HPS-AUTO-001/002/003 | System hazard/safety allocation, independent review and qualified target/ABI/fault behavior |
| hps-banking | Core, HPS-PAY-001/002/003/004 where payment scope applies | Selected product/data-flow/security scope, replay/transaction/schema controls and target evidence |
| hps-pos | Core, HPS-PAY-001/002/003/004 | Payment scope and assessor/integrator disposition, sensitive-data/provider/lifecycle contracts |
| hps-industrial | Core execution/security/performance baseline; dedicated overlay unset | Application safety/security requirements, selected standards and platform/fault/real-time contract |
| hps-medical | Core execution/security/performance baseline; dedicated overlay unset | Device/system safety/security requirements, selected standards and independent target validation |
| hps-defence | Core execution/security/performance baseline; dedicated overlay unset | System security/safety/classification requirements, selected standards and target validation |

Mapping is not qualification, applicability approval or certification. No external
standard obligation is invented for the three undefined overlays. No whole-domain
not-applicable disposition is claimed for any folder. GAP-016 retains missing
product contracts; GAP-015 retains unbuilt reader/resource evidence. Each ecosystem
must choose its concrete execution profile and applicable overlays before adoption.

## Threat model and prepared verification

Assets: input bounds, cursor integrity, decoded outputs and borrowed-view lifetime.
Entry points: init, queries, skip/take and seven numeric reads. The trust boundary
is caller-owned view metadata and actual byte storage. Malformed counts/NULL-state,
EOF/truncation and oversized requests are explicitly handled. Forged non-NULL
addresses, false extents, indeterminate storage and races are caller misuse; these
cannot be validated by cursor arithmetic alone. Output aliasing is forbidden.

Prepared tests cover known mixed LE/BE sequences, independent readers, remaining
counts, reset/rebind, canonical empties, exact end states, every truncated field
width/order, nonzero cursor failure, NULL/malformed metadata, SIZE_MAX skip/take
requests and unchanged descriptors/outputs/payload. Metadata-only boundary probes
never form/dereference payload pointers from synthetic extents; they deliberately
exercise arithmetic outside the valid-array contract, not actual large arrays.
Direct metadata writes are test setup/fault injection, not supported application
usage. Numeric tests use independent known bytes.
Test side effects remain outside assert and every suite source has a callable
filename-matching entry point.

No C configure/build, executable tests, portability compiler probes, sanitizers,
fuzzing, semantic MISRA analyzer, C++ link or target memory/timing evidence is run
under the standing instruction. Policy/whitespace/source checks and final hashes
are recorded below for this source snapshot. Qualification and independent review
remain open; the high-performance/security intent is not an evidence claim.

## Performed source-only checks

Tool: Python 3.10.12 and the unchanged tools/check_policy.py lexical checker.
`python3 tools/check_policy.py` returned 1 with exactly six existing HPS-C-001
basic-type findings at tests/base/test_base.c:36, tests/bytes/test_bytes.c:629,
tests/memory/test_pool.c:149 and tests/test_suites.h:16/17/18. No new production,
reader-test, public-header or governance finding was reported. Direct lexical
checks of reader.h, reader.c and test_reader.c plus the public reader-header
structural checks returned no findings. This is not a passing whole-repository
policy result, a semantic C assessment or evidence of MISRA compliance.

`git diff --check` passed. `make -n test` lists reader.c and test_reader.c in the
shared executable with assertions enabled and the separate portability command;
it executed neither compiler nor test program. A second agent reviewed all eleven
APIs, pointer/output restrictions, empty cases, cursor bounds, failure behavior,
prototypes/linkage and all manifests. Final source review checked independent
fixtures, mixed cursor progression, truncation/state tests and the metadata-only
SIZE_MAX arithmetic probes. Documentation records the eight ecosystem mappings,
undefined overlays and missing target evidence. This author-side assistance is
not an independent named project-role acceptance or qualified-target review.

## Checked source snapshot

| File | SHA-256 |
| --- | --- |
| include/hps/bytes/reader.h | `96d00028ecb646327dd3ac2957931828100773c55df26d3ced2e34280e8d1210` |
| src/bytes/reader.c | `7c8b0b94a5ce56d3c9dcd875d82c463f98d16f812665e694d89baf2b524b7e51` |
| tests/bytes/test_reader.c | `fb673ace1324f0733bd29c8f88dcf850646d4b344c0871d7b0c2f510580b50b2` |
| tests/test_suites.h | `756818135be52809845eaef57746ea0ae4e7e5df14a7dc03317f055b6064fa01` |
| tests/main.c | `8ebb6d1c9626fe27bf34a879b0e18c83cbacfa2ab4811bd8adbef79091eec5a1` |
| CMakeLists.txt | `281f5881122cc991d3e907de2d2eaab4938ab8178a5400ac28ee26e17f125c8a` |
| Makefile | `22401c6335512a40cdc446bd2d852e63bdd84186c14bc69c75503bdbd3d97086` |
| meson.build | `d4a788ea3384fce0e96367875377b5318719fb66f58c8df7c6569ea7f0aab949` |
| governance/misra/matrix.csv | `30980d4d0f982e217c8a0fe94a20648b099e7cf81e10002aac8544cfa408bdf3` |
