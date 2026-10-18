# HPS

HPS is a general-purpose C framework. Its base layer is a header-only foundation
shared by industry-specific layers. It has no runtime, allocation, operating
system, network, or file I/O dependency.

Include `hps/base.h` for the complete C99+ API, or include individual headers.
See [the base API and portability contract](docs/architecture/base.md) before
using optional capabilities on a new target.

```c
#include "hps/base.h"
#include <stdint.h>

HPS_STATIC_ASSERT(HPS_C_IS_C99, "C99 required");

static HPS_INLINE uint32_t count(void)
{
    return 3U;
}
```

## Framework rules

[Engineering governance](governance/README.md) defines the project's mandatory
contribution rules, exact-width types, memory/ownership, security, target profiles
and verification gates. The MISRA C:2023 matrix tracks all 221 baseline guideline
identifiers; implementation compliance is still unassessed. See
[contributing](CONTRIBUTING.md) and [current adoption gaps](governance/status.md).

Run `python3 tools/check_policy.py` for the automated subset without compiling C.

## Build and test

```sh
make test
# Or:
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
# Or:
meson setup build/meson
meson compile -C build/meson
meson test -C build/meson --print-errorlogs
```

The interface library is `HPS::base` in CMake and `hps-base` in Meson.
Linking it adds the include path; there is no compiled base library.

```cmake
add_subdirectory(path/to/hps_framework)
target_link_libraries(your_target PRIVATE HPS::base)
```

For installed CMake use, run `cmake --install build/cmake --prefix <directory>`,
set `CMAKE_PREFIX_PATH` in the consumer, then use `find_package(HPS CONFIG REQUIRED)`
and link `HPS::base`.

The portability suite requires Python 3. It tests independently included headers,
C99/C11/C17/C2x compilation, negative contracts, freestanding compilation, and
synthetic compiler/architecture detection. Run it with another GCC/Clang-compatible
compiler using `python3 tests/base/check_portability.py --compiler clang`.
The native test executable does not need Python.

The shared `hps_tests` executable runs `test_base`, `test_pool`, `test_bytes`,
`test_reader`, `test_enum`, `test_string`, `test_string_extended`,
`test_string_builder`, `test_span`, `test_cstr`, `test_cstr_convert`,
`test_cstr_format`, `test_national_code` and `test_cstr_enum` once, in that order,
and stops on the first failure.
Module test files expose
functions matching their filenames; the only suite runner `main` is in
`tests/main.c`. Make, CMake and Meson now use this executable instead of the three
separate module test executables; the portability suite remains separate.
The user-requested `int` test entry points currently fail the numerical policy
subset and have a [proposed test-only exception](governance/reviews/test-entrypoints-2026-10-11.md).
The fourteen-suite runner's verification for the current API is recorded in the
[result-type migration review](governance/reviews/status-removal-2026-10-18.md).
The [earlier verification record](governance/reviews/csep-text-2026-10-13.md)
preserves historical flags, sanitizer results and target/policy gaps.

## Operation results

Fallible memory, byte and text operations return `int32_t` directly from
`<stdint.h>`: zero means success and negative values identify local errors.
Compare results with the API's `HPS_POOL_*`, `HPS_BYTE_*` or `HPS_STRING_*`
constants. Their values are local to each family and may overlap. Predicates,
lengths and text continue to use their documented output types.

The common status type, module registry, message lookup and separate status
library have been removed. Link the module you use directly. Consumers must
rebuild and update old result typedefs and numeric assumptions; see the
[API/ABI migration](governance/reviews/status-removal-2026-10-18.md).

## Named-value traits

`hps/enum/traits.h` preserves the imported enum API for name lookup, parsing,
formatting and ordered traversal from an X-macro list of native enum values.
It supports separate
declaration/implementation and header-only modes. It is a general utility beside
bytes and memory; its runtime operations are outside the base umbrella.
See [the enum traits contract and CSEP migration](docs/architecture/enum-traits.md).
The original bool/int/char signatures and legacy string behavior are retained at
the user's request. Its suite is included in the current host verification.

## Strings

`hps/string.h` includes borrowed text views and fixed-capacity C-string buffers.
Views support exact slicing, comparison, prefix/suffix tests, forward/backward
search, trimming and bounded C-string import/export. Buffers use caller-owned
storage for assign, append, erase, truncate and clear; capacity includes the NUL
terminator. Failures preserve outputs and payloads; overlapping copies use memmove.
No heap, pool, OS or global mutable state is involved.

Link `HPS::string` in CMake or use `hps-string` in Meson. See
[the string contract and legacy behavior mapping](docs/architecture/string.md).
Counted text and real character-array test storage have a
[proposed scoped policy exception](governance/reviews/string-storage-2026-10-13.md);
release acceptance remains pending. Bounded builders, JSON composition, typed fixed storage, ASCII helpers and alphabet
policies are now included. Native heap growth and Unicode interpretation require
separate contracts.

## C strings, spans and string keys

`hps/cstr.h` provides bounded C-string transformations, parsing/rendering, byte
encoding, split/join, lexical path helpers, diagnostics and typed formatting.
Link `HPS::cstr`/`hps-cstr`; it depends on string. Formatting uses exact-width
arguments and fixed decimal coefficient/scale, with no printf varargs or native
floating point. See [the contract](docs/architecture/cstr.md) and
[legacy family migration](docs/migration/csep-text.md).

`hps/span.h` generates typed borrowed read-only spans with C89-style constructors
and C99 compound-literal support. `hps/base/expression.h` supplies single-evaluation
branch hints and LVALUE. See [span construction](docs/architecture/span.md).
The native string-key enum generator lives beside traits in `hps/enum/cstr.h`;
[its bounded switch example](docs/architecture/cstr-enum.md) includes input capacity.

## Static memory pool

The memory module owns 32 static 128-byte buffers and a private pointer free list.
`hps_malloc`, `hps_free`, `hps_realloc` and `hps_calloc` have the standard allocation
function signatures; they never call the heap. Initialize once with `hps_pool_init()`.
Allocation pops in O(1); free/realloc validate addresses in at most 32 comparisons.
Only byte-buffer use is supported, with explicit exhaustion and error diagnostics.

See [the API and embedded deployment contract](docs/architecture/memory-pool.md).
This replaces the handle/index pool. There is no thread-safe variant. The rewritten
C tests passed in the current host verification. Industry compliance requires the selected
profile and [review evidence](docs/architecture/misra-review.md).

## Bytes

`hps/bytes/view.h` provides borrowed read-only byte views with checked slicing,
`read_u8` access and equality. `hps/bytes/buffer.h` provides fixed-capacity mutable
buffers over caller-owned uint8_t arrays, with `assign_bytes`, `append_bytes`,
`append_u8`, truncate/clear and borrowed views. A view describes readable bytes;
a buffer tracks writable storage, its active length and its capacity. Failures
preserve state; overlapping copies use memmove.
No module allocation, pool dependency or synchronization is involved.

`hps/bytes/endian.h` adds checked u16/u32/u64 reads at view offsets and complete
field appends to buffers, in explicit LE/BE order. No native alignment/byte-order
assumption is used. See [integer byte encoding](docs/architecture/bytes-endian.md).

`hps/bytes/reader.h` adds a sequential cursor over borrowed views, with remaining,
skip, `take_view` borrowed slices and u8/u16/u32/u64 reads. Failed operations
preserve cursor and output; taking a slice copies no payload. See
[the reader contract](docs/architecture/bytes-reader.md) for ownership, bounded
work and ecosystem integration limits. Its suite passed in the current host verification.

Link `HPS::bytes` in CMake or use `hps-bytes` in Meson. See
[the bytes contract](docs/architecture/bytes.md) for lifetimes, bounds, target libc
requirements and API name migration. The current host verification includes all
bytes/codec and reader tests; target library, ABI, resource and timing qualification
remains open.

## Numeric type policy

Numeric declarations use exact-width types from `<stdint.h>` (such as `uint8_t`,
`uint32_t` and `int32_t`) and `size_t` from `<stddef.h>` for lengths and array
indices. Integer types with least/fast width qualifiers are not used. Plain `char` is reserved for C strings
(`char *` / `const char *`). Hosted executable entry points retain the standard
`int main(void)` signature; library numeric declarations do not use basic types.
This project policy supports MISRA's numeric-type guidance; it does not establish
whole-framework compliance.

Flags retain `uint8_t`; fallible module APIs return `int32_t` directly with local
error constants. [The C rules](governance/c-rules.md) also require readable validation and
state transitions, shared logic where contracts match, and explicit reasons for
specialization or repetition.
