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

Include `hps/bytes/view.h`. An hps_byte_view contains a const uint8_t pointer and
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
[endian operations](bytes-endian.md) in `hps/bytes/endian.h`.
For sequential checked reads with a cursor, see the
[reader contract](bytes-reader.md) in `hps/bytes/reader.h`.

## Buffer

Include `hps/bytes/buffer.h`. An hps_byte_buffer contains a uint8_t pointer,
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
#include "hps/bytes/buffer.h"

#define MESSAGE_CAPACITY 64U
static uint8_t message_storage[MESSAGE_CAPACITY];
static hps_byte_buffer message;

/* Exclusive startup: caller owns both descriptor and storage lifetimes. */
hps_byte_status message_init(void)
{
    return hps_byte_buffer_init(&message, message_storage, MESSAGE_CAPACITY);
}

hps_byte_status message_append_bytes(hps_byte_view payload)
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

`hps_byte_status` is a semantic alias of the shared `int64_t` `hps_status_t`.
Bytes owns module ID two; its typed constants occupy that module's reserved
negative range. See [shared status diagnostics](status.md) for classification,
immutable messages and registration.

| Condition | Status | Value |
| --- | --- | --- |
| Successful operation | HPS_BYTE_OK | 0 |
| NULL required argument or malformed source/view | HPS_BYTE_INVALID_ARGUMENT | -131073 |
| Slice/index/truncate outside active length | HPS_BYTE_OUT_OF_RANGE | -131074 |
| Copy/append exceeds destination capacity | HPS_BYTE_NO_SPACE | -131075 |
| Buffer length > capacity or NULL data with positive capacity | HPS_BYTE_INVALID_STATE | -131076 |

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

Targets are `HPS::bytes` in CMake and `hps-bytes` in Meson, with the corresponding
status library dependency. The shared `hps_tests` runner invokes `test_bytes(void)`
alongside the base, pool, reader, status, enum and string suites in all three build
systems. Public headers are picked up automatically by the independent-inclusion
portability suite. Current host build and test results are recorded in the
[string review](../../governance/reviews/string-2026-10-13.md).

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

## Status ABI migration

The current shared-status change preserves function names, status alias spelling
and byte ownership/bounds/failure behavior. It changes `hps_byte_status` from
uint8_t to int64_t and replaces positive error values 1..4 with -131073..-131076.
Use named constants; do not narrow errors into uint8_t or continue serializing the
earlier numeric values. Byte view, buffer and reader structure layouts are
unchanged because they contain no status member.

Rebuild the library and every consumer together, including callbacks, wrappers
and foreign-language bindings that return or store this alias. The existing
typed endian header and implementation are unchanged, but their status-return
ABI widens through the alias as well. Old binaries cannot be mixed with the new
headers/library. See [the shared-status change record](../../governance/reviews/status-2026-10-11.md).

## Validation status

Tests cover embedded-zero/high-byte payloads, empty and full buffers,
NULL/malformed metadata, SIZE_MAX bounds, unchanged outputs on failure,
self-descriptor slicing and 2,025 append plus 2,025 assign snapshot-model cases
covering overlapping and disjoint ranges. The current shared host suite passed
with GCC and Clang; exact commands and evidence scope are in the
[string review](../../governance/reviews/string-2026-10-13.md). Semantic analyzer
assessment, qualified target ABI/timing and industrial acceptance remain open.
The [bytes change review](../../governance/reviews/bytes-2026-10-11.md) covers its
original snapshot; the
[shared-status review](../../governance/reviews/status-2026-10-11.md) records the
original representation change and its historical verification status.
