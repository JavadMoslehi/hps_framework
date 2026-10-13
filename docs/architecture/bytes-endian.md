# Integer byte encoding

Include `hps/bytes/endian.h` and link the existing bytes library (`HPS::bytes`
in CMake or `hps-bytes` in Meson). This extends the
[borrowed bytes module](bytes.md) without changing its descriptor layouts.
It requires exact-width uint8_t, uint16_t, uint32_t and uint64_t on the target.
uint8_t availability implies eight-bit C bytes for this representation.

## Operations

| Family | Example | Bytes | Behavior |
| --- | --- | --- | --- |
| Read unsigned integer | `hps_byte_view_read_u32_be(view, offset, &value)` | 2/4/8 for u16/u32/u64 | Decode at an explicit offset, preserve the view |
| Append unsigned integer | `hps_byte_buffer_append_u32_be(&buffer, value)` | 2/4/8 for u16/u32/u64 | Encode after the active prefix, advance length on success |

Each family has u16/u32/u64 functions with `_le` and `_be` suffixes. LE writes the
least significant octet first; BE writes it last. For example, UINT32_C(0x81234567)
is `67 45 23 81` in LE and `81 23 45 67` in BE. Existing
`hps_byte_view_read_u8` and `hps_byte_buffer_append_u8` operations already cover
u8. The [sequential reader](bytes-reader.md) adds a
checked cursor over these reads; buffer.length already tracks sequential appends.
Signed numbers, floating point, variable-length integers and protocol schemas
remain outside these operations.

These operations use uint8_t elements and explicit shifts/masks. They work at
unaligned byte offsets without casting storage to a wider object type or choosing
code based on native endian detection. No heap, pool reservation, OS service,
global mutable state or synchronization is added.

## Bounds, errors and lifetime

Reads require a live view over initialized bytes and a writable result of the
exact declared type. Result objects must be disjoint from the descriptor and
payload. NULL output or malformed NULL/nonzero-length view gives
HPS_BYTE_INVALID_ARGUMENT. The read checks offset <= length and field width <=
length-offset before forming a field pointer; truncation/outside offsets give
HPS_BYTE_OUT_OF_RANGE. Failed reads preserve outputs. Successful reads do not
change the descriptor or an external cursor.

Appends require an initialized buffer. NULL buffer gives HPS_BYTE_INVALID_ARGUMENT;
malformed metadata gives HPS_BYTE_INVALID_STATE; insufficient capacity gives
HPS_BYTE_NO_SPACE. The complete field is encoded in a fixed local uint8_t array
and submitted through one checked `hps_byte_buffer_append_bytes` call. Failure
preserves the descriptor and every payload byte, including the active prefix and
unused capacity. Success writes
only the new field and advances length by its width. No partial field is appended.

The local encoded array is consumed synchronously and its address is not retained.
All [view/buffer ownership and serialization contracts](bytes.md) still apply;
an actual backing extent, live pointers and metadata/payload separation remain
caller obligations. Nothing here validates arbitrary C addresses or concurrent
storage mutation. These are integer-format primitives, not message validation.

## Arithmetic and resources

Read shifts use widened uint32_t operands for u16/u32 and uint64_t for u64. Shift
counts are fixed and below their respective width; byte lanes are disjoint. Each
shifted octet and the combined value fit within UINT32_MAX/UINT64_MAX. If a target
promotes an exact-width unsigned operand to signed int, that promotion can only
occur when int represents its whole range, so the same bounds prevent signed-shift
overflow. The u16 composite is <=UINT16_MAX before narrowing. Each encoded octet
is masked to 0xFF before the uint8_t conversion.

Every call has bounded constant work: at most eight input/output bytes. Appends
use at most eight local payload bytes plus view/call metadata; reads use a local
view and status. No loop, recursion or variable-length storage is introduced.
Actual stack use, latency and possible compiler runtime helpers, particularly
for 64-bit arithmetic on smaller targets, require target qualification. Append
inherits the buffer's memmove dependency and its pending qualification. Constant
work is not a measured performance, constant-time security or WCET claim.

## Validation status

Tests are part of the existing callable `test_bytes(void)` suite, with no module
`main`. Known wire bytes, high-bit/zero/max values, unaligned offsets, truncation,
SIZE_MAX bounds, NULL/malformed arguments, exact capacity and failure preservation
are covered. The current shared host suite passed with GCC and Clang; the
[string review](../../governance/reviews/string-2026-10-13.md) records the commands
and evidence scope. Host results do not establish qualified target ABI, MISRA
essential-type results, timing or industrial qualification.
The [endian change record](../../governance/reviews/bytes-endian-2026-10-11.md)
retains its original snapshot and historical verification status.
