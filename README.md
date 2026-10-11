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

## Numeric type policy

Numeric declarations use exact-width types from `<stdint.h>` (such as `uint8_t`,
`uint32_t` and `int32_t`) and `size_t` from `<stddef.h>` for lengths and array
indices. Integer types with least/fast width qualifiers are not used. Plain `char` is reserved for C strings
(`char *` / `const char *`). Hosted executable entry points retain the standard
`int main(void)` signature; library numeric declarations do not use basic types.
This project policy supports MISRA's numeric-type guidance; it does not establish
whole-framework compliance.
