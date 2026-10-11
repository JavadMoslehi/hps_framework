# HPS

HPS is a general-purpose C framework. Its base layer is a header-only foundation
shared by industry-specific layers. It has no runtime, allocation, operating
system, network, or file I/O dependency.

Include `hps/base.h` for the complete C99+ API, or include individual headers.
See [the base API and portability contract](docs/architecture/base.md) before
using optional capabilities on a new target.

```c
#include "hps/base.h"

HPS_STATIC_ASSERT(sizeof(hps_byte) == 1, "C byte size");

static HPS_INLINE hps_size count(void)
{
    return 3;
}
```

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
