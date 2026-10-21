# Bounded JSON syntax

Include `hps/json/json.h`, or the independent `hps_tokenizer.h`, `hps_reader.h` and
`hps_writer.h` headers. Link `HPS::json` in CMake or `hps-json` in Meson. JSON depends
only on the existing String/Base providers. All fallible operations return direct
`int32_t` with typed module-local `HPS_JSON_*` constants: zero succeeds, negative
values report invalid arguments/state, syntax, space, depth or numeric range.
No global error registration, heap allocation, recursive calls or OS dependency.

This is a development API under the
[migration/review record](../../governance/reviews/objects-json-2026-10-19.md),
with pending character-storage disposition and target qualification.

## Reading and lifetimes

Input is an immutable counted `hps_string_view`, shorter than SIZE_MAX. Its live
extent must exist; a pointer/length cannot prove arbitrary memory valid. Exact
UTF-8 and ASCII-compatible execution characters are required. A streaming
`hps_json_tokenizer` borrows input and emits lexical tokens with raw text and
zero-based byte offset, one-based byte line/column. Raw STRING spans include
quotes/escapes. LF alone advances the line counter. EOF has an empty raw view.
Failures preserve tokenizer and token.

`hps_json_read` validates exactly one complete JSON value using caller-provided
token and frame arrays. Token storage includes punctuation and excludes EOF.
Frames bound simultaneously open arrays/objects; a scalar needs zero frames.
Both declared array capacities must be representable as real typed extents.
Input/tokens/frames/document metadata are disjoint. Syntax/depth and then token
capacity are preflighted before any token/document output write; frames are
scratch and may change on error. The two scans use constant automatic state.

```c
#include "hps/json/hps_reader.h"

static int32_t read_number(int64_t *number)
{
    const hps_string_view input = {"{\"n\":1}", 7U};
    hps_json_token tokens[5U];
    hps_json_frame frames[1U];
    hps_json_document document;
    int32_t result = hps_json_read(input, tokens, 5U, frames, 1U, &document);
    if (result == HPS_JSON_OK) {
        result = hps_json_token_i64(&tokens[3U], number);
    }
    return result;
}
```

The sample's raw input is fixed and known; application lookup must validate token
kinds/member structure rather than assume the fourth token always holds a number.
Document/raw-token lifetimes borrow both input and token storage; no addresses of
these automatic arrays escape the sample. Duplicate object names and source
order remain visible, with no implicit first/last-wins selection. There is no
owning DOM, key registry, schema lookup or automatic destruction.

## Strings, numbers and writing

The scanner checks four JSON whitespace bytes, exact numbers/literals,
string escapes, UTF-8 scalar encoding and paired surrogate escapes. Comments,
trailing commas, BOM, overlong/non-scalar UTF-8 and lone surrogate escapes are
rejected. [RFC 8259](https://www.rfc-editor.org/rfc/rfc8259), sections 2, 6–9,
defines the grammar; rejecting lone surrogates is the documented stricter
interoperable-Unicode subset. No normalization or Unicode collation is promised.

`hps_json_token_decode_string` validates an entire STRING raw token, preflights
output size and writes counted UTF-8 bytes into caller `uint8_t` storage. Embedded
U+0000 is preserved and no C terminator is added. Input/output extents and metadata
are disjoint; every failure preserves output bytes/count. This representation
avoids forcing decoded binary-NUL text into a C-string buffer.

Number tokens retain their entire original spelling, including decimals and
exponents. `hps_json_token_i64` accepts only complete integer spellings, returning
SYNTAX for decimal/exponent text and OUT_OF_RANGE for integers outside int64.
It preserves output on failure. There is no floating conversion, locale, errno,
arbitrary-precision calculation or implicit narrowing.

`hps_json_write_compact` independently validates raw input and a caller frame
budget, checks the initialized destination's fixed capacity including NUL, then
replaces text with lexical spans without inter-token whitespace. It preserves
escapes, number spellings, member order and duplicate names. It is a validated
compact writer, not canonical JSON or an object serializer. Build new text with
existing String JSON quoting and typed Cstr formatting, then validate/write it;
no duplicate text writer or universal value object is required. Input/frames/
destination metadata and payload are mutually disjoint. Output stays unchanged
on every failure; scratch frames may change.

All work is bounded by input length, supplied token/frame capacities, and output
extent. There is no payload-sized automatic array. Caller storage includes
tokens, frames, output and optional decoded bytes; native padding and call stacks
still require target measurement. Operations require serialized use and live
unchanged borrowed storage. Host tests do not qualify concurrency, stack/WCET,
whole-program no-heap behavior or security of application schemas.
