# Sequential byte reader

Include `hps/bytes/hps_reader.h` and link the existing bytes library (`HPS::bytes`
in CMake or `hps-bytes` in Meson). A `hps_byte_reader` stores a borrowed
`hps_byte_view` and a `size_t position`. The reader advances over caller-owned
bytes without allocating, copying payload or using the static pool.

## Operations

| Operation | Result on success |
| --- | --- |
| `hps_byte_reader_init` | Binds a view, normalizes empty input and sets position to zero |
| `hps_byte_reader_remaining` | Writes the remaining count without moving position |
| `hps_byte_reader_skip` | Advances position by a checked count without accessing payload |
| `hps_byte_reader_take_view` | Writes a borrowed slice and advances by its length |
| `hps_byte_reader_read_u8` | Reads one byte and advances by one |
| `hps_byte_reader_read_u16_le/be` | Reads a two-byte integer and advances by two |
| `hps_byte_reader_read_u32_le/be` | Reads a four-byte integer and advances by four |
| `hps_byte_reader_read_u64_le/be` | Reads an eight-byte integer and advances by eight |

Initialize before any other operation. Calling init again with a valid live view
restarts the reader; init(reader, reader->view) is allowed because the view is
passed by value. Each operation commits independently: a later failed read does
not undo earlier successful reads. Whole-message parsing and its application
commit/rollback policy remain the caller's responsibility.

The writer side remains the existing [buffer and integer append operations](bytes-endian.md).
`buffer.length` already identifies the next append position; no duplicate writer
cursor is introduced. Signed/variable-length fields, seeking and protocol-specific
validation are outside this reader's contract.

## Bounds and errors

The invariant is `position <= view.length`. Before subtracting remaining bytes,
the reader validates this invariant and the view's NULL/length combination.
Skip checks count <= length-position. `take_view` delegates to checked view slicing;
numeric reads delegate to checked view/endian reads. A successful operation proves
that the cursor increment is <=length-position, so the new position is <=length
and the addition cannot wrap SIZE_MAX.

| Condition | Status and outcome |
| --- | --- |
| NULL reader or required output | HPS_BYTE_INVALID_ARGUMENT; output NULL takes priority |
| Init with NULL view data and positive length | HPS_BYTE_INVALID_ARGUMENT; original reader preserved |
| Existing position > length or NULL data with positive length | HPS_BYTE_INVALID_STATE; all state/outputs preserved |
| Requested count/field exceeds remaining input | HPS_BYTE_OUT_OF_RANGE; cursor/output preserved |
| Valid request | HPS_BYTE_OK; cursor advances only for skip/take/read |

Zero-byte skip/take is valid at any valid position, including end of input.
Take returns `{NULL, 0}` for empty results and performs no NULL/end pointer
arithmetic. Reading a positive-width value at the end fails without changing its
output. No function dereferences indeterminate metadata or arbitrary invalid
addresses safely: valid typed pointers and initialized objects are prerequisites.

## Ownership and resource contract

Nonempty views must describe an actual live array of initialized uint8_t bytes.
The reader must be disjoint from backing payload. Every scalar/view output must
be disjoint from the complete reader object and payload; output aliasing is a
caller precondition, not a runtime range comparison. Separate reader objects may
share immutable live storage and keep independent positions. Each reader and its
storage require serialized access; no thread/ISR-safe claim is made.

Reader and taken views borrow storage. Mutation, rebind or release of a source
buffer invalidates its borrowed readers/views under the [bytes contract](bytes.md).
Checks cannot prove actual extents, detect stale lifetimes or contain arbitrary
RAM faults. Parsed fields still need application schema, version, range,
authentication and resource-admission validation before external effects.

Metadata is `sizeof(hps_byte_reader)` (one view plus position and target padding).
There is no payload scratch array, heap, OS call, callback, loop, retry, recursion,
global mutable state or lock. All operations do constant bounded work; number
reads access at most eight bytes. Take avoids payload copying, but does not qualify
DMA/cache/lifetime behavior. Read dependencies are view/endian functions with no
explicit libc calls; compiler runtime helpers, ABI, stack and latency remain
target evidence obligations. O(1) is neither measured throughput/WCET nor a
constant-time secrecy guarantee.

## Ecosystem and validation scope

This common primitive follows the core C, memory and security baseline for the
workspace's automotive, banking, defence, industrial, medical, Open5GS, POS and
telecom ecosystem placeholders. Product standards, threat models and target
profiles are still integration requirements. Existing automotive, payment and
telecom overlays apply where their scope is selected; industrial, medical and
defence overlays are not defined yet. The
[change review](../../governance/reviews/bytes-reader-2026-10-11.md) records every
ecosystem mapping and the remaining obligations. No ecosystem is qualified by
this module or by the existence of its directory.

Tests in `tests/bytes/test_reader.c` expose `int32_t test_reader(void)`
through the shared runner, with no module main. They cover mixed byte/field/slice
sequences, independent cursors, empty/end states, malformed arguments, truncation,
failure preservation and SIZE_MAX arithmetic. The exact-width suite return avoids
extending the existing proposed int exceptions. The current shared host suite
passed with GCC and Clang; commands and evidence scope are in the
[string review](../../governance/reviews/string-2026-10-13.md). Semantic analyzers,
reader-specific fuzzing, qualified target C/C++ ABI and performance/resource
checks remain pending.

Synthetic-extent arithmetic probes and direct metadata mutation are deliberate
test setup/fault injection outside normal application usage. They neither access
invalid payloads nor establish support for an actual SIZE_MAX-byte array.
