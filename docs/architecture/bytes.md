# Byte views and fixed-capacity buffers

The bytes module borrows caller-owned uint8_t arrays. It is independent of the
pool, OS and thread services. It does not allocate, resize storage or free anything.
It has no global mutable state and uses size_t for lengths/offsets/capacities.

## Design

A view answers which initialized bytes may be read: it borrows a readable span
with a pointer and length. A buffer answers where bytes may be written and how
many are active: it borrows writable storage with a pointer, length and fixed
capacity. Both descriptors leave storage ownership and lifetime with the caller.

`hps_byte_view_slice` and `hps_byte_buffer_as_view` create borrowed descriptors
without copying payload. `hps_byte_buffer_assign_bytes` and
`hps_byte_buffer_append_bytes` copy bytes into buffer storage. A reader adds a
read position to a view; `buffer.length` already serves as the next append
position. Clearing a buffer changes this length and does not erase its storage.

## View

Include `hps/bytes/hps_view.h`. An hps_byte_view contains a const uint8_t pointer and
length. Construction and slice are O(1), with no payload copy. Empty results are
normalized to {NULL, 0}; slicing an empty range performs no pointer arithmetic.
An empty slice at the end is valid, an offset beyond the end is not.
`hps_byte_view_read_u8` requires index < length. Equality returns a uint8_t 0/1
result and stops on a mismatch; it is not suitable for secret constant-time
comparisons.

The original array must remain alive, and nonempty input views must cover actual
initialized readable bytes. A descriptor cannot prove pointer validity or extent.
Views do not keep the owner alive or prevent writes through another alias. They
are binary sequences: embedded zero bytes are ordinary data, not a terminator.
No encoding or character/string semantics are attached.

For explicit unsigned integer field encoding, see the
[endian operations](bytes-endian.md) in `hps/bytes/hps_endian.h`.
For sequential checked reads with a cursor, see the
[reader contract](bytes-reader.md) in `hps/bytes/hps_reader.h`.

## Buffer

Include `hps/bytes/hps_buffer.h`. An hps_byte_buffer contains a uint8_t pointer,
length and capacity. Initialize it with an actual caller-owned writable array;
length starts at zero without reading or clearing payloads. NULL storage is valid
only with capacity zero. Empty zero-capacity buffers accept empty
`append_bytes`/`assign_bytes`, but `append_u8` returns NO_SPACE. Buffer descriptors
can be rebound through init after invalidating borrowed views; storage ownership
always stays with the caller.

`assign_bytes` replaces active bytes; `append_bytes` appends the bytes described
by a view; `append_u8` adds one byte. Capacity is fixed. Bounds are validated
before copying; NO_SPACE does not perform
a partial copy or alter length/payload. Append uses source.length <= capacity-length
rather than adding unchecked lengths. Overlapping source/destination, including
self-append, is supported by standard memmove. Descriptor metadata and output
objects must not overlap referenced payloads; copying cannot corrupt metadata.

Truncate can shrink but never grow the active length. Clear sets length zero.
Both retain old bytes: neither is an approved secret erasure primitive. as_view
borrows only initialized active bytes; buffer mutation, rebinding or release of
backing storage invalidates such views under the API contract.

```c
#include "hps/bytes/hps_buffer.h"

#define MESSAGE_CAPACITY 64U
static uint8_t message_storage[MESSAGE_CAPACITY];
static hps_byte_buffer message;

/* Exclusive startup: caller owns both descriptor and storage lifetimes. */
int32_t message_init(void)
{
    return hps_byte_buffer_init(&message, message_storage, MESSAGE_CAPACITY);
}

int32_t message_append_bytes(hps_byte_view payload)
{
    return hps_byte_buffer_append_bytes(&message, payload);
}
```

This example is single-context. Separate buffers with disjoint backing memory may
be used by separate owners, but this module contains no synchronization. Network
lengths must be validated against actual source storage before making a view.
Application-selected capacities/maximum comparisons must also satisfy CPU/deadline
budgets; size_t alone is not admission control.

## Errors, bounds and dependencies

Checked byte APIs return `int32_t` directly from `<stdint.h>`. Zero means
success; local negative values distinguish the errors below. Constants are typed
as `int32_t` in `hps/bytes/hps_view.h`; there is no result typedef, module registry,
diagnostic-string lookup or shared result-library dependency. Codes are local to
the bytes API and may overlap other APIs; compare the named byte constants.

| Condition | Status | Value |
| --- | --- | --- |
| Successful operation | HPS_BYTE_OK | 0 |
| NULL required argument or malformed source/view | HPS_BYTE_INVALID_ARGUMENT | -1 |
| Slice/index/truncate outside active length | HPS_BYTE_OUT_OF_RANGE | -2 |
| Copy/append exceeds destination capacity | HPS_BYTE_NO_SPACE | -3 |
| Buffer length > capacity or NULL data with positive capacity | HPS_BYTE_INVALID_STATE | -4 |

Failed operations preserve payloads, descriptors and scalar/descriptor outputs.
Buffer state validation precedes source validation; API comments specify applicable
errors. Caller pointers must be live, determinate and correctly typed; no recovery
from arbitrary invalid pointers or stale lifetimes is promised.

View construction/slice/read_u8 and buffer init/append_u8/truncate/clear/as_view
use O(1) work. Equality uses O(length) worst-case for equal-length inputs; copy and
append use O(source.length). There are no retries, recursion, dynamic arrays or
module scratch allocations. Metadata requires sizeof(view)/sizeof(buffer) on the
target; caller storage and stack/call costs need target qualification.

The buffer implementation depends on `<string.h>`/memmove. Zero-length copies
skip the call, so NULL empty ranges are never passed to it. The selected libc or
compiler-provided implementation, possible compiler-generated copy helpers,
transitive no-heap behavior and timing/stack must be reviewed for the target.
There is no linked-product no-heap or freestanding-libc-availability guarantee.
The view implementation has no runtime library dependency in its source.

Targets are `HPS::bytes` in CMake and `hps-bytes` in Meson. The shared
`hps_tests` runner invokes `test_bytes(void)` alongside the base, pool, reader,
enum and text suites in all three build systems. Public headers are picked up
automatically by the independent-inclusion portability suite. The
[string review](../../governance/reviews/string-2026-10-13.md) records historical
host results for the earlier representation; it does not verify this migration.

## API name migration

The following external function symbols have been renamed to make the byte
operation or borrowed result explicit:

| Previous name | Current name |
| --- | --- |
| `hps_byte_buffer_assign` | `hps_byte_buffer_assign_bytes` |
| `hps_byte_buffer_append` | `hps_byte_buffer_append_bytes` |
| `hps_byte_buffer_append_byte` | `hps_byte_buffer_append_u8` |
| `hps_byte_view_at` | `hps_byte_view_read_u8` |
| `hps_byte_reader_take` | `hps_byte_reader_take_view` |

Consumers must update calls and rebuild with the matching headers and library.
There are no legacy aliases. The naming change preserved public structure
layouts, status values, parameter types, ownership, bounds and failure contracts
at that step. Symbol compatibility with the earlier names is not preserved. See
[the API naming review](../../governance/reviews/bytes-api-names-2026-10-11.md)
for that historical change scope and deferred validation.

## Result API/ABI migration

The shared result abstraction has been removed at the user's request. All checked
byte, buffer, endian and reader APIs now return `int32_t` directly; remove uses
of the earlier byte result alias from callers. The previous `int64_t` result ABI
and global values -131073..-131076 have changed to local values -1..-4. Function
names, parameters, ownership, bounds checks, validation priorities and failure
preservation are unchanged. View, buffer and reader structure layouts are
unchanged because they contain no result member.

Rebuild the library and every consumer together, including callbacks, wrappers
and foreign-language bindings. Update any persisted numeric assumptions and keep
error results in `int32_t`; do not narrow them into `uint8_t`. Old binaries cannot
be mixed with these headers/library. Policy/MISRA mapping, evidence and pending
target qualification are recorded in the
[result-removal review](../../governance/reviews/status-removal-2026-10-18.md).

## Validation status

Tests cover embedded-zero/high-byte payloads, empty and full buffers,
NULL/malformed metadata, SIZE_MAX bounds, unchanged outputs on failure,
self-descriptor slicing and 2,025 append plus 2,025 assign snapshot-model cases
covering overlapping and disjoint ranges. Existing suites also check direct
`int32_t` function/callback signatures and local result values. Current migration
verification is recorded in the
[result-removal review](../../governance/reviews/status-removal-2026-10-18.md).
Semantic analyzer assessment, qualified target ABI/timing and industrial acceptance
remain open. Earlier bytes, endian and shared-result reviews retain their
historical scope; they do not establish current target qualification.

## View algorithms and last-byte removal

The [23-header migration](../migration/bytes-features-optional.md) extends the
existing providers without a second fixed-byte-buffer generator. View compare
writes canonical unsigned-byte lexical -1/0/1; starts/ends write uint8 flags.
Empty edge patterns match. Find/rfind return a size_t index via checked output
or HPS_BYTE_NPOS on successful absence; empty needles never match, preserving
the legacy draft contract. Reverse search does not decrement zero or narrow
indexes. Read-only inputs may overlap, and output metadata must be disjoint.

Buffer pop_u8 requires writable disjoint output, reads the last active byte and
then decreases length. Empty is OUT_OF_RANGE. Payload bytes are retained and
prior views are invalidated under the existing mutation contract. All failures
preserve descriptor, payload and outputs. Work is O(1) for pop, O(minimum length)
for compare, O(pattern length) for edges and O(view length times needle length)
for search; automatic state is scalar and no payload scratch/heap is introduced.
See the [review](../../governance/reviews/bytes-features-optional-2026-10-19.md).
