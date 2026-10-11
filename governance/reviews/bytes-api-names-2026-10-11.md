# Bytes API naming change — 2026-10-11

Status: development-only; compiled behavior and consumer linkage unverified.
Author: Codex. Maintainer acceptance and independent project-role review: pending;
no approval, policy exception or compliance disposition is asserted.
Source base: 1e081f1b2a5fc4b8f5ed195e3685daccfddb2fff plus this working-tree change.
Authorization: the user requested clearer byte operation names, including
append_bytes, and an explanation of the view/buffer design.
Development scope: hosted, caller-owned storage and serialized access. The
standing instruction defers C configuration, compilation and executable tests.

## Requirement, design and migration

Public names now distinguish copying a byte sequence, accessing a uint8_t value
and returning a borrowed view. Update callers and rebuild libraries/consumers;
old symbols have no compatibility aliases. This breaks source and link interfaces.

| Old symbol | New symbol |
| --- | --- |
| hps_byte_buffer_append | hps_byte_buffer_append_bytes |
| hps_byte_buffer_assign | hps_byte_buffer_assign_bytes |
| hps_byte_buffer_append_byte | hps_byte_buffer_append_u8 |
| hps_byte_view_at | hps_byte_view_read_u8 |
| hps_byte_reader_take | hps_byte_reader_take_view |

The function parameter/return types, public descriptor layouts and status values
are unchanged. Existing bounds, failure preservation, overlap handling and lifetime
contracts are retained. assign_bytes copies into existing storage and replaces the
active length; append_bytes copies after that prefix. take_view borrows a slice
and advances the reader only on success. A view does not own or copy its backing
bytes; a buffer borrows writable storage and tracks length/capacity. Clear changes
length without erasing bytes. Reader position and buffer length remain their
respective read and append cursors; no separate writer or allocation is added.

Declarations, definitions, endian call sites, both callable bytes test suites and
current documentation are updated. The shared runner/build manifests need no
change. Existing prepared fixtures and assertions are retained; this rename adds
no runtime behavior or test-only numerical declaration.

## Policy and MISRA mapping

- HPS-C-001/006/009/011/013, HPS-HDR-001 through 006: exact types, checked arithmetic,
  namespaced matching declarations/definitions and guarded C linkage are retained.
  Ownership, payload/output separation and error contracts are unchanged.
- HPS-MEM-001/002/005, HPS-EXEC-001 through 003, HPS-SEC-001/002/005/006:
  no additional storage, allocation, OS/concurrency dependency, payload operation,
  secret retention/erasure promise or threat boundary. Existing algorithmic bounds
  are retained; source naming provides no measured performance or WCET evidence.
- HPS-VER-001 through 004/006, HPS-GOV-004/005/008/009, HPS-REL-001/003:
  explicit migration, source checks and pending evidence. Historical source hashes
  and review results remain historical for the affected files; no passing build
  or independent acceptance is inherited or invented.
- HPS-PRO-001 through 004, HPS-PERF-001/002: the
  [reader ecosystem mapping](bytes-reader-2026-10-11.md) remains applicable to all
  eight ecosystem placeholders. No product standard, profile qualification or
  new synchronization layer is introduced.

Affected MISRA identifiers: Rules 5.1, 5.8 and 8.3 for external identifiers and
matching declarations. Their matrix rows reference this record and remain
unassessed. Prior rows/records for bounds, types, headers and ownership are retained.
Lexical identifier checks cannot assess target linker significance or semantic
MISRA conformance; full authoritative rule interpretation and target analysis
remain required.

## Verification and disposition

Tool: Python 3.10.12 and the unchanged tools/check_policy.py lexical checker.
`python3 tools/check_policy.py` returned 1 with exactly the six existing HPS-C-001
int findings at tests/base/test_base.c:36, tests/bytes/test_bytes.c:629,
tests/memory/test_pool.c:149 and tests/test_suites.h:16/17/18. Byte production/header
lexical/structural checks and reader-test lexical checks reported no findings.
This is not a passing whole-repository policy result or semantic C analysis.

Compared source tokens with HEAD after replacing only the five exact identifiers;
comments and whitespace were ignored. All ten changed C/header files matched.
Checked all 34 public byte APIs for one declaration and definition each, all
operation references for a corresponding declaration, absence of old identifiers
in include/src/tests and distinct first 31 identifier characters. These are source
checks, not a substitute for compiler/linker validation.

`git diff --check` passed. `make -n test` listed the unchanged shared runner, bytes
sources and separate portability command without executing a compiler/test.
A second agent reviewed naming, production/test diffs, preserved fixtures and
contracts, documentation, migration and governance references without findings.
This assistance is not independent named project-role acceptance.

C builds, compiled/portability tests, C++ linking, target ABI/resources/timing and
semantic analysis remain deferred under the standing instruction. GAP-017 records
consumer migration/linkage; existing GAP-012/014/015 and the proposed
test-entry-point exception remain open. No qualified-target or ecosystem claim is
made. No new policy exception is required by this naming change.

## Checked C/header snapshot

Hashes cover the ten changed C/header files; historical review hashes were not
rewritten. Documentation and this record are retained in the same working change.

| File | SHA-256 |
| --- | --- |
| include/hps/bytes/view.h | `6f413d1ea40b9c783f2d8cdd50f98331ead8b5a139097e5cb0b6563d17169297` |
| include/hps/bytes/buffer.h | `eddfddda6ddfa11e1ec5790993bf686eb48dbe4087b034cf0b3262928f2a6dcc` |
| include/hps/bytes/endian.h | `032b63e5826373f23d978de8c331677a35b0ba27f18e56f837126ca9f5a3c807` |
| include/hps/bytes/reader.h | `da8cae8bb3f7435c21f2fa2201906b4778ddec9ec45c9f572c8e0aa70e5f0f39` |
| src/bytes/view.c | `e10f5c1343b18960d67d9a5100be4a4af22a73fc45a8c084ab3b7b5b7bb5c045` |
| src/bytes/buffer.c | `5168632c17335b76f3ab31568edaf9d72c8b83046aeb521c0485b4625aa2c8d6` |
| src/bytes/endian.c | `98bec892af9f220a83aa2f914b8c81299dacbe8e6e882ede2e2ed0c39ab6207f` |
| src/bytes/reader.c | `5ac341a524f3c2df1a3e39974109c7aa9829f2bf92c0a7b6c2b21b6a9bea0171` |
| tests/bytes/test_bytes.c | `fb4719a672bded3a35e847008b729299ac3a13600eea3a0bb59955ce460d8b64` |
| tests/bytes/test_reader.c | `9872e32bcd21838b3d6e57c452ce0d230d346f267ddc605029ae9d2a1d8ca800` |
