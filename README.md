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
`test_reader` and `test_status` once, in that order, and stops on the first failure.
Module test files expose
functions matching their filenames; the only suite runner `main` is in
`tests/main.c`. Make, CMake and Meson now use this executable instead of the three
separate module test executables; the portability suite remains separate.
The user-requested `int` test entry points currently fail the numerical policy
subset and have a [proposed test-only exception](governance/reviews/test-entrypoints-2026-10-11.md).
This runner change has not been compiled or executed.

## Status diagnostics

`hps/status.h` defines the shared `int64_t` status type `hps_status_t`: success is
zero and errors are negative. Framework, memory and bytes errors occupy distinct
registered module ranges. `hps_status_module` identifies an allocated range;
`hps_status_string` returns an immutable NUL-terminated diagnostic message of at
most 31 characters, or `"unknown status"` for an unregistered code. No allocation
or runtime registration is involved.

Link `HPS::status` in CMake or use `hps-status` in Meson; memory/bytes targets
include this dependency. Module status aliases and constants retain their names
but now use this signed representation and module-specific negative values.
Consumers must rebuild and update stored numeric status assumptions. See
[the status contract and migration](docs/architecture/status.md). Its prepared C
tests have not been compiled or run.

## Static memory pool

The memory module owns 32 static 128-byte buffers and a private pointer free list.
`hps_malloc`, `hps_free`, `hps_realloc` and `hps_calloc` have the standard allocation
function signatures; they never call the heap. Initialize once with `hps_pool_init()`.
Allocation pops in O(1); free/realloc validate addresses in at most 32 comparisons.
Only byte-buffer use is supported, with explicit exhaustion and error diagnostics.

See [the API and embedded deployment contract](docs/architecture/memory-pool.md).
This replaces the handle/index pool. There is no thread-safe variant. The rewritten
C tests have not been built or run. Industry compliance requires the selected
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
work and ecosystem integration limits. Its C tests have not been built or run.

Link `HPS::bytes` in CMake or use `hps-bytes` in Meson. See
[the bytes contract](docs/architecture/bytes.md) for lifetimes, bounds, target libc
requirements, API name migration and prepared-test status. C builds/tests remain
deferred.

## Numeric type policy

Numeric declarations use exact-width types from `<stdint.h>` (such as `uint8_t`,
`uint32_t` and `int32_t`) and `size_t` from `<stddef.h>` for lengths and array
indices. Integer types with least/fast width qualifiers are not used. Plain `char` is reserved for C strings
(`char *` / `const char *`). Hosted executable entry points retain the standard
`int main(void)` signature; library numeric declarations do not use basic types.
This project policy supports MISRA's numeric-type guidance; it does not establish
whole-framework compliance.

Flags retain `uint8_t`; API statuses use `hps_status_t` or its semantic module
aliases. [The C rules](governance/c-rules.md) also require readable validation and
state transitions, shared logic where contracts match, and explicit reasons for
specialization or repetition.
