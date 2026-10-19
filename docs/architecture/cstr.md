# Bounded C strings and text composition

Include `hps/cstr/all.h`, link `HPS::cstr` (CMake) or `hps-cstr` (Meson).
The former `hps/cstr.h` path remains a forwarding compatibility include.
This layer builds on `HPS::string` and returns its local `int32_t` result codes:
zero for success and `HPS_STRING_*` errors -1 through -5. It has no shared result
registry dependency. The return representation and numeric error values change
with the [string result migration](string.md); consumers must be rebuilt.
Its 45 functions group the implemented CSEP C-string operations into four headers.

| Header | Operations |
| --- | --- |
| `hps/cstr/core.h` | Bounded length/termination/space, clear, comparison/case, exact range copy, substring/character search, reverse/replace/filter/remove, trim and padding |
| `hps/cstr/convert.h` | Checked signed/unsigned integer parse/render, hex/binary/text/decimal byte formats, decode, inverse bits, bool text |
| `hps/cstr/algorithm.h` | Character classes, borrowed split/count/index, join/group, lexical filenames/min/max, title/value and deterministic integer/byte dumps |
| `hps/cstr/format.h` | Typed formatting/append and exact fixed-decimal rendering |

Every raw string input has a real readable capacity **including NUL**. Missing
termination is an error; no unbounded scans or silent truncating writes remain.
Counted views borrow initialized text and may include NUL, but C-string outputs
reject embedded NUL except JSON escaping, which renders it as text. Metadata,
payload lifetimes, alias restrictions and operation-specific error priorities are
documented in the independently includable headers. All errors preserve outputs.
No heap, stdio, locale, OS, pool or hidden global state is introduced.

Core mutation uses initialized `hps_string_buffer` objects. Most transformations
work in place; overlapping copy ranges use memmove. Removal/filter/padding patterns
that are reread during mutation must be disjoint from the whole writable storage.
Builder transactional edits and formatting instead take disjoint caller-owned
scratch storage, allowing source views to borrow the unchanged destination.
Scratch capacity includes NUL and its contents are preserved on failure.

Integer parsing consumes the entire nonempty bounded input and checks overflow
before multiply/add. Supported unsigned radices are 2/8/10/16; signed parsing is
decimal with optional sign. INT64_MIN/UINT64_MAX remain valid values. Hex decode
retains the legacy odd-leading-nibble convention (`ABC` becomes `0A BC`). TEXT
copies byte representations; it is not a Unicode/ASCII codec. Binary decode
requires complete eight-bit groups. Filename helpers are purely lexical.

Formatting uses initialized `hps_cstr_format_arg` structures with an explicit
uint8_t tag, counted text, int64_t/uint64_t values, a flag and decimal scale.
The supported grammar is `%s`, `%c`, `%d/%i`, `%u`, `%x/%X`, `%b`, `%f` and `%%`;
integer `l/ll` modifiers select the same exact-width fields. Missing/extra/wrongly
tagged arguments and unsupported flags/precision/width/%n are errors. A sizing
pass validates everything before writing scratch or destination. This replaces
the legacy printf ABI and varargs, rather than exposing unsafe compatibility
aliases. The exact fixed decimal `coefficient / 10^scale` supports scale0..18,
including INT64_MIN; it has no floating-point rounding or locale dependency.
Native float/double and FILE printing were not imported into core.

The string layer additionally exposes 45 view functions, 10 buffer functions,
26 bounded builder functions, 21 ASCII-octet helpers and 59 alphabet policies
through three policy operations. `hps/string/fixed.h` generates typed owned
character arrays and binds separate descriptors; it does not serialize a native
struct or embed a movable self-pointer. ASCII builder/policy operations explicitly
require an ASCII-compatible execution character set. Alphabet policies never
validate full email/IP/date/payment grammar. Reserve on a bounded builder checks
capacity; it never allocates. The original heap-owning lifecycle was replaced by
caller ownership, explicit binding and capacity failure, with no hidden pool use.

The Iranian national-code checksum is a separate domain utility in
`hps/validation/national_code.h`, linked as `HPS::validation`/`hps-validation`.
It checks syntax and the legacy checksum only, with no issuance/identity claim.
String keys for switch statements belong to `hps/enum/cstr.h`.

[The complete legacy family/file mapping](../migration/csep-text.md) distinguishes
adapted APIs, consolidated variants, removed unsafe/heap/hosted contracts and
empty legacy placeholders. Current result-API tests, C/C++ interoperability checks
and remaining limitations are recorded in the
[result-API replacement review](../../governance/reviews/status-removal-2026-10-18.md).
The earlier [migration review](../../governance/reviews/csep-text-2026-10-13.md)
records historical implementation evidence and the proposed character and
native-enum policy dispositions; it does not verify the current result ABI.
Target ABI/resources/timing, whole-program analysis and policy acceptance remain
unqualified. Algorithmic bounds are not measured performance or proven WCET.

## Legacy truthy text conversion

`hps_cstr_parse_bool(input, capacity, result)` reuses bounded C-string import,
ASCII whitespace classification and basic-Latin case-insensitive comparison.
After skipping a leading run of basic ASCII whitespace, only the complete suffix true/yes/on/1
produces flag one. All other terminated text produces zero, including unknown
strings, empty input and truthy words followed by whitespace. This deliberately
preserves the source predicate rather than claiming strict boolean validation.
NULL/zero-capacity arguments and missing termination give documented errors and
preserve the output. See the [mapping](../migration/bytes-features-optional.md)
and [review](../../governance/reviews/bytes-features-optional-2026-10-19.md).
