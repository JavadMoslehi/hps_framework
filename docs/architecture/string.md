# Bounded string foundation

`hps/string/string.h` exposes the completed bounded string foundation: a borrowed text
view and a caller-owned fixed-capacity C-string buffer. Their public contracts are
in `include/hps/string/hps_view.h` and `include/hps/string/hps_buffer.h`; implementations
are in `src/string/view.c` and `src/string/buffer.c`. This utility layer sits beside
bytes and memory, outside the environment-detection responsibilities of
`hps/base.h`. It uses local `int32_t` result codes and the C library's `memcmp`
and `memmove`; it does not use the pool, allocate storage, or call OS services.
The former root forwarding header has been removed; see the
[include migration](../migration/public-headers-2026-10-21.md).

The initial foundation below contains 23 functions. The subsequent user-authorized
[extension and C-string migration](cstr.md) adds the remaining view/search/split
families, bounded builders, typed fixed storage, character policies, conversions
and formatting. Heap ownership, persistence schemas and Unicode remain separate
contracts. The original foundation contracts described here still apply.

## Public operations

The following suffixes follow the indicated prefix. Each function returns
`int32_t`; predicates, comparisons and searches write their values
through explicitly typed output parameters.

| Prefix | Purpose | Function suffixes |
| --- | --- | --- |
| `hps_string_view_` | Construct a counted or bounded C-string view | `init`, `init_cstr` |
| `hps_string_view_` | Borrow an exact subrange | `slice` |
| `hps_string_view_` | Equality, ordering and edge matching | `equal`, `compare`, `starts_with`, `ends_with` |
| `hps_string_view_` | Search | `find`, `rfind`, `contains` |
| `hps_string_view_` | Borrow a trimmed window | `trim`, `trim_whitespace` |
| `hps_string_view_` | Export a complete terminated string | `copy_cstr` |
| `hps_string_buffer_` | Bind empty storage or adopt existing text | `init`, `init_cstr` |
| `hps_string_buffer_` | Copy complete text | `assign`, `append` |
| `hps_string_buffer_` | Remove text | `erase`, `truncate`, `clear` |
| `hps_string_buffer_` | Borrow text and query space | `as_view`, `cstr`, `remaining` |

## Views, lifetime and search results

`hps_string_view` stores `const char *data` and `size_t length`. A nonempty view
refers to that many initialized readable characters in one live object. Its text
can lack a trailing NUL and contain embedded NUL; length remains authoritative.
NULL data is valid only at zero length. Successful empty outputs normalize to
`{NULL, 0}`. Length must be below `HPS_STRING_NPOS`, which equals `SIZE_MAX` and is
reserved for a search result with no match.

Construction and slicing borrow storage without copying it. Slicing requires
`offset <= length` and `count <= length - offset`; it rejects an oversized count
rather than clamping it. View descriptors are passed by value, so a slice or trim
result may replace the original descriptor. Such operations change the borrowed
window, never the characters. `trim` removes characters belonging to its supplied
set from both ends. `trim_whitespace` uses space, tab, newline, carriage return,
vertical tab and form feed in the execution character set.

Equality and edge predicates write exactly zero or one. Comparison writes -1,
zero or one using unsigned-byte lexicographic order, then length to break a common
prefix tie. It does not perform locale folding or Unicode normalization and is
not a comparison for secrets.

`find` requires a starting offset at most the view length. An empty needle matches
that offset, including the end. `rfind` starts at the smaller of its supplied
position and the last possible match position; `NPOS` selects the end. An empty
reverse-search needle matches the smaller of position and view length. A search
that finds no match succeeds and writes `NPOS`; malformed input and invalid
forward-search offsets return errors without changing the result. Consequently,
`contains` reports true for an empty needle even in an empty view.

The caller owns backing storage and proves its actual extent, initialization and
lifetime. Const access prevents writes through the view API, not another alias.
Caller serialization is required when storage can be mutated or released. Buffer
mutation, rebinding or release invalidates borrowed buffer results under this
contract. Descriptor and scalar output objects must be disjoint from payload
storage; buffer descriptors and output metadata must also be disjoint from each
other. Runtime checks cannot recognize arbitrary invalid pointers or stale
lifetimes.

## C strings, buffers and failure preservation

Every C-string capacity is the total accessible character count **including the
terminating NUL**. Bounded import reads at most that capacity and excludes the NUL
from the resulting view. A NULL input or zero capacity is invalid; a missing NUL
within the bound returns `HPS_STRING_NOT_TERMINATED`. No unbounded scan is used.

`hps_string_buffer` stores `char *data`, `size_t length` and `size_t capacity`.
Storage belongs to the caller and capacity is at least one. Initialization writes
only the first NUL and establishes an empty buffer. `init_cstr` adopts existing
terminated text without writing its payload. Each later operation checks that
data is non-NULL, `length < capacity`, the character at length is NUL and no
earlier character is NUL. A broken invariant returns `INVALID_STATE`.

Available payload space is `capacity - length - 1`. Assign replaces active text;
append preserves the old prefix. Both validate buffer state, source shape,
available capacity, and then the absence of embedded NUL in that order. Thus an
oversized source containing NUL returns `NO_SPACE` before its characters are
scanned. Views support embedded NUL, but complete C-string exports and buffer
copies reject it so that the recorded length equals the C-string length.

Assign, append and `copy_cstr` copy the complete source or fail; they never
truncate to available space. Capacity proofs use subtraction/comparison before
forming destination pointers or adding lengths. `memmove` supports overlapping
payloads, including a buffer assigning or appending its own text or a substring.
The source's original characters define the copied result. Descriptor/payload
aliasing remains prohibited.

`erase` requires both an offset within the active text and a count within its
remaining suffix; it moves the tail including NUL. A zero count is a successful
no-op. `truncate` only shrinks and writes a new terminator; `clear` truncates to
zero. These operations do not wipe discarded characters. Assign/append/export
preserve storage after their new terminator. `as_view` normalizes an empty result;
`cstr` still returns the initialized storage pointer, including its empty-string
terminator.

Every failed public operation preserves descriptor fields, payload and output
objects. This guarantee is per call: a sequence of successful operations followed
by a failure does not roll back earlier successful calls. All scalar/descriptor
outputs designate writable objects of their declared types; callers check results
before using them.

String, C-string and dependent text operations return `int32_t` directly.
`HPS_STRING_OK` is zero; `HPS_STRING_INVALID_ARGUMENT`,
`HPS_STRING_OUT_OF_RANGE`, `HPS_STRING_NO_SPACE`, `HPS_STRING_INVALID_STATE`
and `HPS_STRING_NOT_TERMINATED` are -1 through -5 respectively. These codes are
local to the text APIs; callers compare the named constants in that context.
There is no shared registry or diagnostic lookup dependency.

Replacing the previous 64-bit result representation with `int32_t` changes the
return ABI, and replacing the previous partitioned error numbers changes their
numeric values. Rebuild consumers and update any stored error codes explicitly;
do not mix objects compiled against the previous declarations. Output types,
failure priority, preservation guarantees and resource bounds are unchanged.

## Using caller-owned storage

Initialize a buffer with `hps_string_buffer_init(&buffer, storage, capacity)`
where `storage` is a caller-provided writable character pointer to its real
bounded array. The owner retains that array throughout all buffer/view use. The
following helper imports bounded C-string input and assigns it to an already
initialized buffer; failed import or assignment preserves the buffer.

```c
#include "hps/string/string.h"
#include "hps/base/hps_stdint.h"

static int32_t demo_assign_cstr(hps_string_buffer *buffer,
                                const char *text, size_t capacity)
{
    hps_string_view source = {NULL, 0U};
    int32_t result =
        hps_string_view_init_cstr(&source, text, capacity);
    if (result == HPS_STRING_OK) {
        result = hps_string_buffer_assign(buffer, source);
    }
    return result;
}
```

## Explicit legacy migration differences

The source inventory and hashes are in
[the migration record](../migration/legacy-sources.md) and
[its manifest](../migration/legacy-source-manifest.json). Reference paths below
are relative to the `csep` and `mylibraries` roots recorded there. Original files,
ZIPs and reference build directories remain read-only. This foundation reuses
selected view/buffer concepts with the following explicit contract changes; it
does not rename every legacy function into an API-compatible replacement.

| Concern | Legacy source behavior | HPS foundation |
| --- | --- | --- |
| Representation | CSEP view has `const char *data` and `size_t length`; MyLibraries view/buffer use `uint8_t` pointers and `len` | Counted text uses actual character pointers and `length`; binary data belongs to bytes |
| Results and input descriptors | CSEP commonly takes descriptor pointers and returns `bool`, `int`, indexes or descriptors; MyLibraries has mixed return conventions | Local `int32_t` result codes plus typed outputs; view descriptors passed by value; flags are `uint8_t`, comparison is `int32_t` |
| Empty and invalid construction | CSEP empty view borrows `""`; NULL C-string becomes empty; `init(out, NULL, positive_length)` can write empty before reporting failure | Empty output is `{NULL, 0}`; NULL C-string input is invalid; failure preserves outputs; `length == NPOS` is rejected |
| C-string import | Both legacy view families scan with `strlen` without an explicit input bound | `init_cstr` requires a real capacity including NUL and rejects missing termination within it |
| Slicing | CSEP clamps count and uses NPOS for the available suffix; MyLibraries clamps start/count | Exact offset/count bounds; oversized requests return `OUT_OF_RANGE` |
| C-string export | CSEP clears then truncates to capacity; MyLibraries clears the full output on a successful copy and has an unchecked zero-capacity subtraction | Complete exact copy, checked space, NUL rejection and failure preservation; bytes after the new terminator stay unchanged |
| Comparison | CSEP can return the raw nonzero `memcmp` result | Comparison normalizes to -1/0/1 with unsigned-byte ordering |
| Search | CSEP provides substring/reverse searches and NPOS; MyLibraries `cstrview_find` searches one character and returns signed index/-1 | Selected substring/reverse searches use local result codes plus `size_t` index/NPOS; empty-needle semantics are explicit |
| Buffer capacity | MyLibraries header describes capacity excluding NUL, while implementation reserves NUL inside capacity | Capacity consistently includes NUL; minimum capacity one; remaining payload space excludes the terminator |
| Copy arithmetic and overlap | MyLibraries append adds lengths before checking space and uses `memcpy` | Subtraction proves available space; overlapping assign/append use `memmove`; no partial writes |
| Buffer text validity | MyLibraries can append views containing NUL and does not validate length/termination state | Active prefix contains no NUL; state is checked before mutation and source NUL is rejected |
| Dependencies and larger features | MyLibraries view includes printing/tokenizer code; CSEP has additional split/find/character helpers and fixed-string generators | No stdio, formatting, tokenizer, heap builder or packed layout in the initial foundation. Subsequent bounded formatting/builders and delimiter-set tokenizer extensions have their own migration contracts. |

The CSEP comparison sources are
`inc/csep/string/string_view/csep_string_view.h` and
`src/csep/csep_string_view.c`. MyLibraries sources are
`include/c_core/strlib/borrow_ownership/cstrview.h`,
`include/c_core/strlib/borrow_ownership/cstrbuffer.h`,
`src/c_core/strlib/cstrview.c` and `src/c_core/strlib/cstrbuffer.c`.
The legacy `repo/tests/hal/utils/test_stringview.c` targets the application's
mutable `newapp/hal/utils/stringview.h`, not `csep_string_view_t`; it is reference
material rather than the HPS view suite.

## Bounded work, tests and policy disposition

View init/slice perform constant work. Import is bounded by input capacity;
equality/order/edge comparisons by the compared text lengths. Naive substring
search has worst-case work proportional to view length times needle length;
generic trim is proportional to view length times set length. Empty cases have
explicit short paths. Buffer operations additionally scan their active prefix
to verify state; assign/append then scan/copy source length, and erase copies a
bounded tail. Work bounds describe source algorithms, not measured latency or
proven WCET. Helpers use fixed-size automatic metadata and no payload scratch
arrays; actual ABI, stack/ROM budgets and library implementations need target
evidence.

`tests/string/test_string.c` covers all 23 APIs, bounded/unterminated input,
empty/NULL/NPOS handling, embedded NUL, unsigned ordering, exact-fit/overflow
boundaries, state/output preservation and overlapping copies. The overlap model
uses independent immutable source snapshots across all substrings and three
capacity bounds. The same suite checks the direct `int32_t` return width, typed
local constants and exact success/error values. Current commands, tools, results,
C/C++ interoperability checks and remaining limitations are recorded in the
[result-API replacement review](../../governance/reviews/status-removal-2026-10-18.md).
The earlier [string review](../../governance/reviews/string-2026-10-13.md) records
historical implementation evidence; it does not verify the current result ABI.
The existence of tests alone is not passing evidence.

The core implementation mode is C99 with the required exact-width integer types.
The legacy frameworks' C89-style intent and base-layer dialect detection do not
establish C89 support for this module. Public guarded C linkage also needs the
chosen consumer/toolchain ABI evidence before a target support claim.

Counted nonterminated text pointers and actual character-array test storage have
a [proposed scoped policy exception](../../governance/reviews/string-storage-2026-10-13.md)
under HPS-C-003. It is not approved project-role acceptance or an external-standard
deviation; checker findings remain visible. Contracts and engineering work map
to HPS-C-004/006/009/010/011/012/017/018/019, HPS-HDR-001 through HPS-HDR-006,
HPS-MEM-001/002, HPS-EXEC-001 and HPS-SEC-002/009. Tests cannot supply that approval,
MISRA compliance, target qualification, certification or measured performance.

## Tokenization and indentation extensions

`hps/string/hps_tokenizer.h` adds delimiter-set scanning with optional literal
quote/escape shielding and edge trimming. Enabled syntax takes precedence over
delimiter membership, including at token boundaries; no unquoting occurs.
Malformed quoting/trailing escape preserves cursor and outputs. Existing
substring `split_next` remains the provider for empty-field substring splitting.

`hps/string/hps_indent.h` adds persistent checked four-space levels and explicit
line-start state over an initialized builder. Compound appends stage the complete
result in disjoint caller scratch; source may borrow the builder. All failures
preserve context, destination and scratch. Explicit newline/line calls drive
state; raw embedded LF is not interpreted. These additions allocate no storage.
See the [twenty-file mapping](../migration/objects-json.md) and
[review](../../governance/reviews/objects-json-2026-10-19.md).
