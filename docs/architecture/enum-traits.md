# Enum traits contract

`hps/enum/traits.h` is the renamed CSEP enum traits generator. It belongs beside
bytes and memory in the utility layer because its generated functions perform
runtime lookup, parsing, iteration and formatting. It is excluded from
`hps/base.h`, whose contract covers environment detection and portable declarations.
The user's requested restoration retains the original enum definitions, generated
API and helper behavior; this is a naming and placement migration.

The original C89-style function bodies and declaration/implementation/header-only
modes are retained. This does not establish strict C89 toolchain compatibility:
`stdint.h` and `stdbool.h` are standard from C99 onward, and a C89 implementation
needs compatible headers/extensions. `HPS_ENUM_STATIC_INLINE` preserves the
original compatibility selection: `static inline` for C++/C99, compiler inline
extensions for MSVC/GCC, or `static` otherwise. It can be defined by the caller
before inclusion. No C configure/build, C89 probe or C++ ABI test has been run.

## Native enum values and generation modes

The `enum_type` parameter names the caller's existing type, including a native C
`enum`; it is not required to be an `int32_t` typedef. The generator does not define
the enum. Its nonempty X-macro list supplies each value and NUL-terminated name.
Unique values and names make lookup and navigation unambiguous; duplicate entries
retain the original first-match behavior. List order governs first/last, indexes,
navigation and distance. Numerical order governs min/max and comparison. Sparse
and unsorted enum values work.

```c
#include "hps/enum/traits.h"

typedef enum {
    DEMO_MODE_IDLE = -7,
    DEMO_MODE_ACTIVE = 12,
    DEMO_MODE_PAUSED = 3
} demo_mode;

#define DEMO_MODE_ITEMS(X) \
    X(DEMO_MODE_IDLE, "idle") \
    X(DEMO_MODE_ACTIVE, "active") \
    X(DEMO_MODE_PAUSED, "paused")

HPS_ENUM_DEFINE_INLINE(demo_mode, demo_mode, DEMO_MODE_ITEMS)
```

`HPS_ENUM_DEFINE_INLINE` emits both declarations and definitions with
`HPS_ENUM_STATIC_INLINE` linkage. For separate implementation mode, put the enum,
item list and `HPS_ENUM_DECLARE(demo_mode, demo_mode, DEMO_MODE_ITEMS)` in the public
module header. Include that header in exactly one `.c` file before invoking
`HPS_ENUM_IMPLEMENT(demo_mode, demo_mode, DEMO_MODE_ITEMS)`. The declaration must
precede the implementation because some generated functions call functions whose
definitions occur later. `HPS_ENUM_EXTERNAL` supplies the external linkage token
and defaults to empty. Use one definition mode for each prefix, with unique
prefixes across externally linked modules.

The generator header's guarded C linkage covers its helper functions. Macros
expand at their invocation sites: external generated declarations, definitions
and callback types require caller-side guarded C linkage for C++ consumers.
Mixed-language use and enum representation remain unverified.

## Generated functions

Each mode provides these 34 functions, with the caller's prefix in place of
`prefix`.

| Operation | Generated suffixes |
| --- | --- |
| Names and formatting | `to_name`, `to_cstr`, `to_string`, `format`, `safe_name` |
| Parsing | `from_name`, `from_cstr`, `from_name_ignore_case`, `from_i32`, `try_parse`, `try_parse_ignore_case` |
| Predicates | `is_valid`, `is_defined`, `contains`, `is_unknown`, `is_first`, `is_last` |
| List access | `count`, `values`, `value_at`, `name_at`, `index_of` |
| Extremes | `first`, `last`, `min_value`, `max_value` |
| Navigation | `next`, `previous`, `next_wrap`, `previous_wrap` |
| Other operations | `distance`, `for_each`, `to_i32`, `compare` |

Try operations and predicates return `bool`. The iteration callback also returns
`bool`: false stops traversal, and true continues. `for_each` returns false for a
NULL callback or early stop, and true after complete traversal. Names are borrowed
C strings and context is caller-owned. `compare` retains its original `int`
return type and returns -1, 0 or 1. These return values are not framework status
codes.

`to_name`, `to_cstr` and out-of-range `name_at` return `HPS_ENUM_UNKNOWN_NAME` when
no item matches; its default is `"UNKNOWN"`. `safe_name` selects the supplied
fallback for an unknown value, or the configured unknown name when fallback is
NULL. Returned item names have the lifetime of their defining storage; callers
must not modify or free them. A supplied fallback retains the caller's lifetime.
`HPS_ENUM_UNKNOWN_NAME` must designate a valid C string whenever it is used.

`values` copies at most the caller's element capacity and returns the count
copied; a NULL output or zero capacity returns zero. `index_of` initializes a
non-NULL output to `HPS_ENUM_NPOS`, whose default is `((size_t)-1)`, then replaces
it on success. It also supports a NULL output to query membership. Other failed
value-parsing/navigation operations leave their value outputs unchanged.

`from_i32` first casts the raw number to `enum_type`, then checks membership. It
does not prove the raw number is representable before that cast. `to_i32`, min/max
and comparison cast enum values to `int32_t`; callers must account for the enum's
representation and keep values in that range for meaningful numerical behavior.
`distance` casts both list indexes to `int32_t` and subtracts them, without checking
the count or difference range. The list must be nonempty because first/last and
min/max access an element directly. These are retained legacy contracts, not new
checked conversion guarantees.

## Strings and failure behavior

There is no configured input scan bound. All non-NULL input strings, including
item names and an append destination, must be valid NUL-terminated C strings in
live readable storage. The caller supplies writable output storage of the stated
capacity; output buffers must not overlap source strings. A non-NULL pointer does
not prove these extents or termination. Case-insensitive matching retains the
original `char` helpers and A-Z/a-z folding; distinct names differing only by case
select the first matching item. No locale or Unicode normalization is supplied.

`to_string` delegates to `hps_enum_cstr_copy`. When the source does not fit a valid
nonzero-capacity output, copy writes an empty string and returns false. Append
helpers return false when there is insufficient room and set the last byte of a
valid nonzero-capacity output to NUL. Their length additions use the original
unchecked arithmetic. `hps_enum_cstr_clear` sets the first output byte to NUL.
The character helper accepts `char`, as in the imported source.

`format` clears a valid destination, then appends `[`, the prefix, `: `, the name
and `]` in sequence. A later failure may leave partial formatted output. NULL
arguments or zero capacity return false. Successful output is NUL-terminated;
failure does not generally preserve the destination. Buffer ownership remains
with the caller.

## Migration, resources and review status

The migration maps `CSEP_ENUM_*` to `HPS_ENUM_*`, `csep_enum_*` to `hps_enum_*`, and
`csep_static_inline` to `HPS_ENUM_STATIC_INLINE`. Of the original CSEP includes,
only the static-inline token was used; the C-string and container includes were
unused. Its compatibility definition is now local, so there is no remaining
CSEP dependency. Standard `stddef.h`, `stdint.h`, `stdbool.h` and `string.h`
dependencies remain explicit.

Generated tables have static lifetime, with a separate original table in each
function that needs one. Header-only generation can duplicate code and tables
across translation units. There is no heap, OS service, mutable global registry or
synchronization. Value scans scale with item count; string operations also depend
on actual NUL-terminated input lengths. Resource budgets, target layout and callback
work require separate evidence. These source properties are not measured
performance or proven WCET.

The user explicitly requested retaining the original behavior. That authorizes
this restoration within the task, but does not approve project-policy or MISRA
exceptions. Native enum storage, `bool`/`int`/scalar `char`, unchecked arithmetic,
unbounded input scans and caller-side linkage remain visible review items under
HPS-C-001/003/004/005/006/009/016/018 and HPS-HDR-004/005/006. See the
[enum migration review](../../governance/reviews/enum-traits-2026-10-13.md) for the
scoped record. There is no claim of MISRA compliance, C89/C++ qualification or
release acceptance.
