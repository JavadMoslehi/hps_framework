# Integer codec wrappers — 2026-10-13

Status: development-only; compiled behavior and consumer linkage unverified.
Author: Codex. Maintainer acceptance and independent project-role review remain
pending; no approval or compliance disposition is asserted.
Source base: a216dba1aa5543166cbda92c353a22c377e8ad7b plus the user's working-tree
codec extraction and this change. Authorization: complete the remaining integer
conversions following hps_codec_bytes_to_u16 and declare them in codec.h.
Profile: hosted development with caller-owned storage and serialized access;
no target/profile qualification. The no-build constraint recorded in the
[endian review](bytes-endian-2026-10-11.md) is retained for this source-only change.

## Behavior and migration

codec.h declares three bytes-to-integer and three integer-to-bytes functions for
16/32/64-bit values and explicit little/big byte order. The original u16 behavior
is preserved: NULL input or unknown order returns zero. The other decoders follow
that contract; encoders do nothing for NULL output or unknown order. Zero is also
a valid decoded value, so callers requiring distinct errors or capacity checks
must use the existing checked view/buffer APIs. These are scalar/void conversions,
not status-returning APIs; no status ID, constant or diagnostic registry changes.

Callers prove initialized readable inputs or writable outputs of 2/4/8 bytes and
live storage. Only uint8_t elements are accessed. No heap, OS, shared runtime state,
local payload array, loop, blocking operation or synchronization is added to the
codec. Each selected helper reads/writes exactly its fixed width. Invalid selector
dispatch precedes any byte access. A pointer parameter cannot verify capacity or
recover from arbitrary invalid pointers. Conflicting storage access remains the
caller's responsibility; no measured timing, stack or secret-time claim is made.

The user's extraction makes the LE/BE hps_decode_uN and hps_encode_uN helpers
private to codec.c. Consumers use hps_codec_bytes_to_uN or hps_codec_uN_to_bytes
with the corresponding HPS_BYTE_ORDER selector and link the bytes library.
endian.c uses those wrappers after its existing checked slice or before its
existing synchronous buffer append; status/error priority and failure preservation
are retained. Make, CMake and Meson include codec.c. The existing test_bytes suite
calls the new direct tests; no new suite signature or hosted main is added.

order.h now has its own path-derived guard rather than suppressing reader.h.
hps_byte_order and hps_codec_type use uint8_t aliases and typed constants while
preserving their names and 0/1 values. This replaces implementation-dependent enum
storage; rebuild consumers for the changed selector ABI. The unused pre-existing
codec context union and unrelated hps_endian_t enum are retained and remain policy
findings, not approved exceptions.

## Policy and MISRA mapping

- HPS-C-001/002/004/006/007/008/011/013/014/016 and HPS-HDR-001 through 006:
  exact-width declarations, explicit NULL checks, matching named prototypes,
  defensive switch defaults, guards, metadata, dependencies and C linkage.
  Byte operands are widened before fixed shifts below their width; disjoint lanes
  bound the u16 narrowing and encoding masks bound each byte. Target promotions,
  essential types, runtime helpers and C/C++ ABI remain unverified.
- HPS-C-009/010/012/017/018, HPS-MEM-001/002/005 and HPS-EXEC-001 through 003:
  documented caller extents/lifetimes and failure behavior; unchanged typed byte
  access, no storage ownership transfer. Width/order-specific helpers are retained
  to expose different operand widths, fixed shifts and extents directly. A generic
  implementation would add indexing/width dispatch; endian adapters instead share
  the same codec arithmetic. This is a readability/resource rationale, not a
  measured optimization.
- HPS-SEC-001/002/003/006/009: assets are integer correctness, byte bounds and
  output integrity. Inputs and selectors cross the caller/API boundary. Selectors
  are checked; actual storage capacity is a precondition. Unknown-order work is
  bounded and leaves output unchanged. Wire-schema validation, authentication,
  sanitizer/fuzz evidence and target fault containment remain outside this change.
- HPS-VER-001 through 004/006, HPS-GOV-004/005/008 and HPS-REL-001/003:
  source-only evidence, explicit migration and residual gaps. Previous hashes or
  review results for changed files are historical; no passing build is inherited.

Relevant MISRA matrix rows: Directives 4.1, 4.6, 4.7, 4.10, 4.14 and Rules 1.3,
5.1, 5.8, 8.2, 8.3, 8.4, 8.5, 8.6, 8.13, 10.1, 10.3, 10.4, 10.6, 10.7, 10.8,
11.9, 12.2, 14.4, 15.5, 16.1, 16.4, 18.1 and 19.2. Previously empty affected
rows link this record; existing links to prior byte/status/test reviews are
preserved. All these dispositions remain unassessed. Full authoritative guideline
interpretation, semantic analysis and named project-role review remain pending.

## Verification

Direct tests reuse independent known LE/BE high-bit fixtures for all widths and
exercise zero/max decoding, NULL input/output, unknown-order zero/no-write behavior
and sentinel-protected exact-width writes. Existing endian tests retain their
checked bounds, failure and capacity scenarios. Calls with effects occur outside
assertions. These tests are prepared but unexecuted.

Python 3.10.12 and the unchanged tools/check_policy.py were used.
`python3 tools/check_policy.py` returned 1 with eight existing findings: the enum
at include/hps/bytes/endian.h:19, the union at src/bytes/codec.c:13, and six int
signatures at tests/base/test_base.c:36, tests/bytes/test_bytes.c:714,
tests/memory/test_pool.c:149 and tests/test_suites.h:16/17/18. The test signatures
retain their [proposed exception](test-entrypoints-2026-10-11.md); no exception is
approved here. Header/lexical checks of codec.h and order.h report no findings;
endian.c likewise has no lexical finding. This is not a passing whole-repository
policy result or semantic MISRA assessment.

`git diff --check` passed. Source assertions verified one matching declaration and
definition for every public codec function, every endian width/order mapping,
distinct order/reader guards and exactly one codec.c entry in each build manifest.
`make -n test` listed codec.c in the shared test executable without executing a
compiler or test. A second agent reviewed the wrappers, arithmetic, headers,
adapters, manifests and prepared fixtures without a functional finding; that
assistance is not named independent project-role acceptance.

C configuration/build, executable/portability tests, sanitizers, semantic analysis,
mixed-language linking and target resource/timing evidence remain deferred.

## Checked source snapshot

| File | SHA-256 |
| --- | --- |
| include/hps/bytes/codec.h | `f7f46651af8d7aeeea7aa6a55595b19a0ffc8e2d66d18f05edb933c6eba017d9` |
| include/hps/bytes/order.h | `8a2db63660af08d2a4ee2bd95b067a4dea42ea9ebc4e00b0162bc81f56314e42` |
| src/bytes/codec.c | `61f4f922ed906aec02c28cd8e73441fa766d4d098b58a71a484f3e0d69219edc` |
| src/bytes/endian.c | `83fcafc34ab4cc281e3c80a1ef46cba631015b17d6fa68b33c2d414db4537c41` |
| tests/bytes/test_bytes.c | `aa4613482751d456aa46b3dde740c0eda5ec85e86e7cb9da7333554ba973f1b7` |
| CMakeLists.txt | `8026c2344e7024dde6234485fa54413c98a56f839ea93a5fa2927cb52a8d8dea` |
| Makefile | `539b380b6ca4593a5d0097cac016a635ce44bd7b01f0be1f750fb0b0e7d67922` |
| meson.build | `ac4260869c7f6f094c050c32499b147ad3ba9a537de9127801c06c301b59dc35` |
| governance/misra/matrix.csv | `d5b9a62de7a494f772dfa9d40d21613073df15e38b971a26909003006bdbfef4` |

## Final development commit audit — 2026-10-13

The earlier snapshot above is historical. Subsequent user edits removed the
unused context union, selected static HPS_INLINE and made zero-bit byte lanes
explicit. Current codec.c SHA-256 is
`85cacf3b3ec873140e34074a6234b1ec2687f1b06e7bd80432a499c3287bd6e9`.
The original byte fixtures/header/order snapshots remain unchanged. Framework
manifests now additionally register the restored enum suite.

A source audit finds all 40 public bytes functions have exactly one definition
and all byte sources/suites are included in Make, CMake and Meson. This completes
the current view/buffer/codec/endian/reader implementation scope. The writer side
is buffer.length plus checked append operations, as documented in the
[reader contract](../../docs/architecture/bytes-reader.md); a separate writer,
signed codec, varint or bit-access API is optional future scope.

No functional source defect was found in that audit. Bytes-specific policy
findings remain the unrelated endian enum and int test_bytes signature; the
now-restored enum traits introduce their separately recorded policy findings.
Compiled byte tests, consumer linkage and target resources remain unverified
under the no-build constraint. This audit supports a development commit, not
an assertion that tests or qualification passed.
