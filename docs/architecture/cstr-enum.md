# String keys for switch statements

The user's generator now lives in `hps/enum/cstr.h` beside enum traits. It generates
a native enum, a bounded text-to-enum function and reverse immutable name lookup.
Matching is exact, case-sensitive and compares text directly; there is no hash
collision behavior. The table requires unique IDs and immutable NUL-terminated
string literals without embedded NUL. Duplicate text selects its first entry.

```c
#include "hps/enum/cstr.h"

#define CONFIG_KEYS(ENTRY, PREFIX) \
    ENTRY(PREFIX, PORT, "port") \
    ENTRY(PREFIX, MODE, "mode")

HPS_CSTR_ENUM_DEFINE_STATIC(config_key_t, CFG, CONFIG_KEYS)

static void handle_key(const char *key, size_t capacity)
{
    switch (config_key_t_from_cstr(key, capacity)) {
        case CFG_PORT:
            break;
        case CFG_MODE:
            break;
        case CFG_UNKNOWN:
        default:
            break;
    }
}
```

Capacity is the actual readable bound including NUL. NULL, zero capacity, missing
termination and unmatched keys map to UNKNOWN. Reverse lookup maps invalid,
UNKNOWN and COUNT values to `"unknown"`. Names have program lifetime and cannot
be modified/freed. Generated functions use internal static inline linkage and
C89-style declarations; target C89/C++ claims still need qualified dependencies.

The supplied file referenced three `HPS_CINTERNAL_*` macros but defined
`HPS_INTERNAL_*`, preventing table expansion. These now share HPS_DETAIL names.
Its guard/@file were aligned with the new path, the unbounded strcmp scan gained
capacity, and stale commented-out example/multiline line comments were removed.
The original unsupported “MISRA-compliant” claim was removed. The native enum
interface remains deliberate for switch use, with a proposed scoped numerical
policy disposition in the migration review; successful tests do not approve it.
