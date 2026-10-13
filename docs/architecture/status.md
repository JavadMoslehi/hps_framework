# Shared status codes and diagnostics

Include `hps/status.h`. Framework and module status-returning APIs use
`hps_status_t`, defined over exact-width int64_t. Success is `HPS_STATUS_OK` (zero);
errors are negative typed constants. `hps_pool_status`, `hps_byte_status` and
`hps_string_status` are semantic aliases of this common representation. Flags keep
their separate uint8_t representation.

Link `HPS::status` in CMake or use `hps-status` in Meson. `HPS::memory`,
`HPS::bytes` and `HPS::string` carry that dependency. The base layer remains
header-only; status lookup is a separate compiled library. The registry includes
module headers for compile-time
constants, and does not call their implementations or inspect live module state.

## Numeric allocation

Module identifiers have uint16_t representation. Error partitions each reserve
65,536 numeric slots. A module's error with reason 1..65,535 has numeric value
`-(module_id * 65536 + reason)`; reason zero is unused. Allocation is a reviewed
static namespace, not a runtime operation or a reason to perform unchecked
caller arithmetic.

IDs 0..3 are currently allocated. Future modules may receive IDs 4..65,534;
UINT16_MAX is permanently reserved for HPS_STATUS_MODULE_UNKNOWN and never owns
an error partition.

| Owner | Module constant | ID | Reserved error range | Defined errors |
| --- | --- | --- | --- | --- |
| Framework | HPS_STATUS_MODULE_FRAMEWORK | 0 | -1 through -65535 | -1 invalid argument, -2 out of range, -3 no space, -4 invalid state |
| Memory | HPS_STATUS_MODULE_MEMORY | 1 | -65537 through -131071 | HPS_POOL errors -65537 through -65543 |
| Bytes | HPS_STATUS_MODULE_BYTES | 2 | -131073 through -196607 | HPS_BYTE errors -131073 through -131076 |
| String | HPS_STATUS_MODULE_STRING | 3 | -196609 through -262143 | HPS_STRING errors -196609 through -196613 |
| Unknown | HPS_STATUS_MODULE_UNKNOWN | UINT16_MAX | No allocated partition | No defined errors |

The allocated ranges are disjoint, including errors with similar meanings in
different modules. Success is common and reports the framework module. The
reserved reason-zero gaps (-65536, -131072, -196608 and -262144), positive values
and values beyond the allocated ranges are unknown. Unregistered reasons inside
an allocated range still report its owning module, but their diagnostic message
is `"unknown status"`.
Classification is not validation that a code was declared or returned by an API.

## Lookup contracts

`uint16_t hps_status_module(hps_status_t status)` accepts every representable
status value and returns an allocated module ID or HPS_STATUS_MODULE_UNKNOWN.
It performs no allocation or mutation. Arbitrary values, including INT64_MIN and
INT64_MAX, must be handled without signed overflow or invalid indexing.

`const char *hps_status_string(hps_status_t status)` returns an immutable
NUL-terminated English diagnostic string in the target execution character set
for exactly registered values. Ordinary C string literals do not guarantee ASCII
encoding. Zero maps to `"ok"`; unregistered values map to `"unknown status"`.
The pointer is non-NULL,
has static storage lifetime and remains valid for the program's lifetime. Do not
modify it or free it. Each message has at most 31 characters;
`HPS_STATUS_MESSAGE_CAPACITY` is a typed size_t constant of 32 including NUL.
No writable output buffer, mutable registry, heap, OS service or locks are used.

Lookup does not alter pool diagnostics, byte/string descriptors or payloads.
The status registry itself has no mutable state; this does not change the pool's
single-context contract or qualify other modules for concurrent use. Messages
provide a bounded explanation for diagnostics. Compare typed numeric constants
for program behavior; messages are not protocol fields or a localization API.

## Registration and resources

New modules require a centrally allocated stable ID, unique negative typed
constants and explicit updates to range classification and the immutable message
registry. Register each published error with a message satisfying the
32-character storage bound,
including the terminating NUL. Update documentation and prepared tests for its
range, exact-code lookup and unknown values. Reserved reasons remain available
for future definitions; the current unknown fallback must stay explicit.

Work is bounded by the fixed registered module/error counts; there is no runtime
growth or retry. Four module ranges and twenty-one registered codes, including
success, are present. Registered messages occupy static immutable storage. The
twenty-two source literals, including the unknown fallback, total 459 C bytes
including NULs; the longest occupies 28 bytes. These are source-storage counts,
not measured linked ROM size.
Compiler placement, total code/data size, stack, execution character set
and lookup latency require target qualification. This is a static resource
contract, not measured throughput, proven WCET or a whole-product no-heap claim.

## Migration and validation

The original shared status migration changed pool/bytes status aliases from
uint8_t to int64_t. Success remains zero, but previously positive module errors
now occupy the negative ranges above. Keep comparisons to named constants and
update stored, serialized or foreign-language numeric assumptions. Widen stored
status objects rather than truncate signed errors.

That migration retained function symbols and allocation/byte contracts while
changing their status-return ABI. Rebuild the libraries and every consumer
together when migrating from the original representation; do not mix old
binaries with new headers. Public pool stats, byte view, buffer and reader
layouts are unchanged because they contain no status member. The typed endian
source/header stay unchanged, but inherit the widened return ABI through
`hps_byte_status`.

Registering string module 3 extends the namespace without changing `hps_status_t`,
existing module IDs, published error values or the status-return ABI. Previously
unknown values in `-262143..-196609` now classify as string, and reasons 1..5
have defined diagnostics. Remaining reasons still return `"unknown status"`.
Consumers treating all former unallocated values as unknown must account for
this semantic extension.

`int32_t test_status(void)` runs after base, pool, bytes and reader in the shared
hosted runner; status, enum and string use failure codes five, six and seven.
Module tests have no main. Host builds/tests have now run for this revision;
the [current string review](../../governance/reviews/string-2026-10-13.md) records
the exact scope and results. Host evidence does not establish target acceptance.
Six existing user-requested int test-entry declarations remain pending under
the proposed test-only numerical exception; the new suite does not extend it.

The [change record](../../governance/reviews/status-2026-10-11.md) maps the status
implementation and user-requested readability/duplication policy amendment to
requirements and MISRA rows. The string review records the module 3 extension
and current host verification. Maintainer/reviewer disposition, qualified target
C/C++ ABI assessment, analyzers, target resources and industrial qualification
remain pending. Historical pool/bytes/endian evidence does not validate this
representation change.
