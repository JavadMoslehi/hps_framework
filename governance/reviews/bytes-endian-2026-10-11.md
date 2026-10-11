# Endian integer bytes change — 2026-10-11

Status: development-only, compiled behavior unverified. Author: Codex.
Independent reviewer/approving authority: not assigned; no acceptance asserted.
Source base: c2d9915e4aae25a7e9ebe4cb71c068bc0d5affd6 plus the shared test-runner
and this working-tree change. Authorization: the user requested the next step
after bytes view/buffer; the previously proposed next step is LE/BE integer
encoding, followed later by bounded cursor readers/writers. The recorded no-build
constraint remains active. Profile: hosted single-owner development verification;
no deployment profile is qualified.

## Design and policy mapping

- HPS-C-001/002/004/013/016, HPS-HDR-001 through 006: exact uint16_t/uint32_t/uint64_t
  values, uint8_t octets/status, size_t offsets, path guard, metadata, explicit
  dependencies, named prototypes and guarded C linkage. The new header/source
  expose twelve functions, with no type aliases, configuration or ABI-changing
  macros. Required exact widths must exist; unsupported targets are not silently
  substituted. Native C/C++ include/link and ABI evidence remain pending.
- HPS-C-006/007/008: each read delegates slicing with a fixed 2/4/8-byte width;
  offset and remaining length are checked before pointer formation. uint32_t
  operands build u16/u32; uint64_t operands build u64. Shift counts are fixed at
  8..24 or 8..56 and each source octet is <=255. Disjoint lanes bound the result
  by UINT16_MAX/UINT32_MAX/UINT64_MAX as appropriate. If integer promotion yields
  signed int, that type necessarily represents the operand's entire original
  range, so the same bounds prevent signed overflow. Right shifts operate on
  unsigned or nonnegative promoted values. Every narrowed encoded octet is masked
  to <=255; the final u16 read cast is bounded by <=UINT16_MAX. Analyzer essential
  types, actual tool/target constants and conversion diagnostics are unresolved.
- HPS-C-009 through 012/014, HPS-MEM-001/002/005, HPS-EXEC-001 through 003: inherited
  caller-owned extents/lifetimes and metadata/payload separation, output commit
  only after successful slice, explicit uint8_t access without reinterpretation,
  fixed 2/4/8-byte local arrays and synchronous append with no escaping address.
  One checked buffer append preserves all state/payload on failure. No new pool
  dependency, global state, lock, retry, loop, recursion or variable-length array.
  Constant bounded work does not establish target stack, timing or compiler-helper
  dependencies. Appends inherit pending memmove qualification.
- HPS-SEC-001/002/003/005/006/009: wire field widths/order are explicit, malformed
  NULL/length/state and truncation fail before payload access/mutation; invalid
  requests consume bounded work. Forged pointers/extents, stale storage and races
  remain caller misuse. Integer field decode does not validate a protocol schema,
  authentication or resource admission. No encryption, erasure or secret-time
  guarantee is claimed; fuzzing/sanitizer/target analysis remain pending.
- HPS-VER-001 through 004, HPS-REL-001/003, HPS-GOV-005: new public header/source,
  documented contract and prepared tests; existing bytes target links endian.c in
  Make/CMake/Meson and existing `test_bytes` calls the new tests. No new main or int
  suite signature is introduced. Existing view/buffer and pool layouts/contracts
  are unchanged. Prior source hashes for changed test/build files are historical,
  including the preceding test-entrypoint record; there is no inherited build
  success. The existing six basic-type findings are kept visible pending their
  proposed test-only exception; the numerical policy/checker is unchanged.

## Threat model and MISRA review scope

Assets are field correctness, output integrity, payload bounds and buffer length.
Entry points are the six reads and six appends. The trust boundary is caller-owned
byte storage and descriptors; software cannot infer real array extents from them.
High-bit/max values, unaligned offsets, malformed/truncated views and exhausted
buffers are supported inputs or explicit failures. Successful appends alter only
the complete field and length; failure changes nothing. Nonempty input bytes must
be initialized. Caller serialization and view invalidation rules remain required.

Relevant MISRA identifiers: Directives 4.1, 4.6, 4.7, 4.10, 4.11, 4.14 and Rules
1.3, 8.2, 8.4, 8.5, 8.6, 10.1, 10.3, 10.4, 10.6, 10.7, 10.8, 12.2, 17.7, 18.1,
18.4, 18.8. Linked matrix rows stay unassessed; existing review links are preserved.
This is an author source review rather than an external-standard disposition.
No new policy exception is requested or asserted for the production addition.

## Prepared verification and evidence

Known LE/BE fixtures test asymmetric high-bit values, zero and all-ones values for
all widths. Both orders exercise valid unaligned offsets, every shorter input,
NULL output, malformed views, empty/end/outside/SIZE_MAX offsets, exact capacity,
one-byte-short capacity, malformed buffer state and unchanged outputs/payloads.
All side effects are outside assert. Tests remain callable from `test_bytes`.

No C configure/build, executable test, portability compilation, analyzer,
sanitizer/fuzzer, mixed-language link, compiler-helper inspection or timing/stack
measurement is performed under the standing instruction. The required policy
checker, whitespace check and source-review results are recorded below for the
final source snapshot. Industrial/qualified-target release obligations remain
open, including all MISRA dispositions and independent project-role review.

## Performed source-only checks

Tools: Python 3.10.12 and the unchanged tools/check_policy.py lexical checker.
`python3 tools/check_policy.py` returned 1 with the six known HPS-C-001 int
signature findings at tests/base/test_base.c:36, tests/bytes/test_bytes.c:629,
tests/memory/test_pool.c:149 and tests/test_suites.h:14/15/16. No new production,
test-helper, header or governance finding was reported. Direct lexical/header
checks of include/hps/bytes/endian.h and src/bytes/endian.c returned no findings.
This is not a passing whole-repository policy result or a semantic C assessment.

`git diff --check` passed. `make -n test` showed endian.c in the shared executable
along with all existing core/test sources and the separate portability command;
this dry run executed neither compiler nor test program. A second agent reviewed
all twelve functions, byte lanes, fixed widths/shifts/masks, failure preservation,
public header structure/linkage and all build registrations. Author-side review
also checked the prepared fixtures and fixed no-space tests to reset payload
sentinels before taking snapshots: a partial write cannot hide by rewriting an
identical previous field. These checks do not supply independent-role approval.

## Checked source snapshot

| File | SHA-256 |
| --- | --- |
| include/hps/bytes/endian.h | `7b2ce4720841a0360adeaad4d03b672bce02d668897488bcf60038c286aab055` |
| src/bytes/endian.c | `6d3b5fafb2d49b1164de0cf3131959c0671a0d69246d9a676407d19f1bc38eae` |
| tests/bytes/test_bytes.c | `3f317745fe2bae5553ef91334f18ddef7e7fc156cd52659a32ba4e4c39605805` |
| CMakeLists.txt | `b639a4bc8a0e12c9f3afe33aac59c9737af5a3de4f82630bf2ecfbdbb155d88f` |
| Makefile | `16714dc83461813bec28b6287de92f18d56c7146a9e9a4cb21fc1f11e074267f` |
| meson.build | `d4c4f23ed69a0f3a2fca64085b00f8a574eb4d97c41fba00fc2d96e3c97f70ed` |
| governance/misra/matrix.csv | `b77199b944990fbde3d758859247bb23b64fcc2f2db9b875136c7282952efca5` |
