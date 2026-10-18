# CSEP text migration map — 2026-10-13

The user authorized reusing both old frameworks, then explicitly requested the
CSEP `inc/csep/cstr` and `inc/csep/string` families with appropriate code/name
changes. Reference paths below are relative to the read-only `csep` root in
[the hashed inventory](legacy-sources.md). Original directories/ZIPs are never
modified or built. HPS outputs/tests belong to the current project.

This migration adapts the implemented functionality to bounded HPS contracts;
it is not API/ABI-compatible source renaming. Public headers document actual
capacities, aliasing, error priorities and resource bounds. Repeated range,
left/right and numeric-width aliases are consolidated into explicit parameters.
The [C-string contract](../architecture/cstr.md) lists the resulting headers.

## Source families and resulting placement

| Legacy file/family | HPS API/disposition |
| --- | --- |
| `cstr/core/csep_cstr_base.h` | core length/is_empty/is_terminated/has_capacity_for/copy_length/clear/clear_tail; buffer remaining; unused object-writer callback removed |
| `cstr/core/csep_cstr_case.h` | copy_range KEEP/LOWER/UPPER; builder ASCII folding for in-place whole strings |
| `cstr/core/csep_cstr_compare.h` | compare(limit, ignore_case); equality is comparison==0; view_equal for counted text |
| `cstr/core/csep_cstr_copy.h` | copy_range(offset,count or NPOS suffix), consists_of, repeat; exact output replaces safe/nonsafe truncating aliases |
| `cstr/core/csep_cstr_search.h` | find/rfind/find_char/rfind_char, copy_range and borrowed view slices for suffix results |
| `cstr/core/csep_cstr_replace.h` | buffer erase(offset,count), core reverse/replace_char/filter; range aliases consolidated |
| `cstr/core/csep_cstr_trim.h` | trim/trim_substrings/filter/remove_substring/pad/pad_to; one-character views use the same padding/removal contract |
| `cstr/convert/csep_cstr_encoding.h` | parse_u64 radix16, encode/decode bytes modes, invert_binary parity; exact whole-output validation |
| `cstr/algorithm/csep_cstr_format.h` | typed format/append_format, render_i64/u64/decimal, boolalpha and eight byte rendering modes; float/double values become exact coefficient/scale arguments; arbitrary printf grammar/varargs removed |
| `cstr/algorithm/csep_cstr_join.h` | bounded join; buffer append and core copy_range for indexed concatenation |
| `cstr/algorithm/csep_cstr_split.h` | group, split_count/split_at/split; view cursor split for iterative tokens |
| `cstr/extra/csep_cstr_dump.h` | dump_i64/dump_bytes and typed format for scalar/decimal fields; unstable pointer-address logging removed |
| `cstr/extra/csep_cstr_path.h` | filename(remove_extension), select(maximum), title_value; no filesystem dependency |
| `cstr/extra/csep_cstr_validate.h` | validate character class; national-code checksum moved to `hps/validation/national_code.h` |
| `cstr/extra/csep_cstr_macro.h` | explicit bounded core functions/buffer binding replace casts, sizeof-on-pointer hazards and nonsafe copy macros |
| `string/csep_char.h` and `src/csep/csep_char.c` | all 21 `hps_char_*` ASCII-octet functions, uint8_t flags/values and int32_t invalid-nibble sentinel |
| `string/string_view/csep_string_view.h` | checked view construction, access, exact slicing, raw/Cstr copy, compare/predicates, char/set/substring searches, bounded Cstr conveniences and list/cursor split |
| `string/string_builder/csep_string_builder.h` | caller-owned bounded builder, edits/replacement/composition/JSON; buffer accessors/query and cstr numeric formatting instead of heap/stdio/varargs |
| `string/string_buffer_fixed/*common.h`, `*runtime.h`, `*storage.h` | typed fixed storage generator plus separately bound buffer/builder; common checked runtime operations replace two large duplicated generators |
| `string/string_buffer_fixed/*policy.h` | all 59 legacy alphabets retained under typed IDs and single-evaluation accept/assign/append; full grammar validation remains a domain responsibility |
| Core/convert/algorithm/extra/string umbrella headers | `hps/cstr.h` and `hps/string.h`; explicit independently usable dependencies |
| `cstr/convert/*ascii.h`, `*format.h`, `*parse.h`, `extra/*ascii.h`, `*hash.h`, `*print.h`, `*text.h`, `algorithm/*escape.h`, `*tokenize.h` | Guard/comment-only legacy placeholders: no absent method is represented as an imported implementation; existing split/JSON composition cover implemented behaviors elsewhere |
| `string_buffer_dynamic`, `string_reader`, `string_writer`, `string_span` | Zero-byte placeholders in source; generic typed span was separately supplied by the user and lives in `hps/span.h` |

## Deliberate behavior and ownership changes

Unbounded strlen/strcmp, pointer-only inputs, unsafe vsprintf and copy truncation
are replaced by real capacities, checked arithmetic and complete writes.
Text operations now return `int32_t` directly with local `HPS_STRING_*` errors
(zero success, negative errors); Cstr and validation reuse the string error domain.
The [2026-10-18 review](../../governance/reviews/status-removal-2026-10-18.md)
records removal of the earlier shared status ABI, numeric-code mappings and
consumer migration. Numeric predicates use uint8_t, comparison uses int32_t, and
indexes use size_t/NPOS. Scalar char mutation wrappers
use counted one-character views or explicitly bounded ASCII-octet values.
All failure paths preserve destination, descriptor and scalar outputs. Scratch
supplied for transactional edits/formatting is disjoint and unchanged on failure.

Builder new/delete/deinit/owned-copy/take/reserve/shrink do not keep their heap
ownership contract. The adapted builder binds caller storage; init/adopt/rebind
and clear express lifetime/state, reserve only checks space, and resize never
grows storage. Caller ownership replaces escaping allocated strings. There is no
implicit heap or pool fallback. Ordinary clear and tail clearing are not secure
erasure. Swap exchanges borrowed descriptors. C-string/view/byte convenience
aliases compose bounded import or explicit view with assign/append/edit operations.
Search/query families use the same view/buffer APIs. JSON escaping accepts counted
embedded NUL and preserves other octets without UTF-8 validation.

Fixed storage keeps actual character arrays with no metadata pointer in the owned
object. Capacity now consistently **includes NUL**; legacy generators specified
usable capacity and added one. Use HPS capacity `old usable capacity + 1`, with a
compile-time checked object budget. A copied object needs descriptor rebinding.
No native struct dump is declared a portable persistence format. Normalize legacy
record bytes in an application serializer with its explicit schema/encoding.
Alphabet policies preserve legacy membership, including aliases, but policy IDs
are HPS configuration values rather than stored/wire ABI. Policy copies validate
membership explicitly before mutation; unvalidated buffer APIs do not secretly
enforce a configured alphabet.

Original C-string empty split has zero fields; its adapter retains this. View
split has explicit keep_empty0/1 and may expose an empty field. Hex odd-leading
nibble, title fill-chunk behavior and lexical min/max right-side tie selection
are preserved and tested. Case operations are locale independent; ASCII builder
and policy helpers have an explicit charset build gate. Raw text encoding remains
byte representation copying rather than a Unicode conversion.

Native float/double/FILE/arbitrary printf interfaces are intentionally replaced or
removed from the bounded core, not reported as migrated hosted implementations.
Exact fixed decimals and typed tokens provide deterministic scalar rendering.
Binary floating point or an OS writer needs a separately reviewed adapter if a
future application requires it. The unused old writer callback alone establishes
no executable feature. National-code validation preserves the legacy checksum,
with bounded input and typed arithmetic; it is not identity/issuance verification.

The user's separate Cstr-enum generator is in `hps/enum/cstr.h` and uses direct
matching, corrected helper names and bounded capacities. Its native enum switch
interface is preserved under a proposed scope disposition. The existing enum
traits generator's behavior stays unchanged.

## Verification and continuation

The 2026-10-13 migration tests covered 15 callable suites in one runner, all build manifests, C89 span
construction, direct string-key switch, numerical limits, exact bounded inputs,
failure preservation, overlap, splitting, JSON and 15,104 policy membership cases
from a restricted AST interpretation of the read-only legacy expressions. This
fixture checks migration parity, not independent proof of the original policy.
Historical compiler commands/results, provenance rechecks and proposed policy
dispositions are in [the review](../../governance/reviews/csep-text-2026-10-13.md).
The shared status suite is removed in the subsequent 2026-10-18 change; its
review records current checks for the remaining suites and changed ABI. The older
string review is a historical foundation snapshot, not current evidence for
these expanded files. Keep this mapping, the manifest and original packages
available when continuing on another machine.
