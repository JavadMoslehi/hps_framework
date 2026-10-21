# Public C header contract

HPS-HDR-001: Every public header uses a unique path-derived guard, for example
`hps/memory/hps_pool.h` -> `HPS_MEMORY_HPS_POOL_H`. Begin with matching `#ifndef`/`#define`
and end with `#endif /* HPS_MEMORY_HPS_POOL_H */`. Guards must not use reserved
identifiers. `#pragma once` is not a substitute for the portable guard.

HPS-HDR-002: Include a file comment naming the public path (`@file`) and purpose
(`@brief`). Keep the header organized as guard, file comment, explicit includes,
public constants/types, guarded C-linkage declarations, and the closing guard.
Do not invent copyright ownership or license identifiers before a license is chosen.

HPS-HDR-003: Include each standard/project header needed for the header's own
public declarations. Do not depend on an umbrella header or include order to
provide types. Keep includes outside the C-linkage block. No inclusion of `.c`
files, namespace imports, mutable definitions, constructors or initialization
side effects. Expose implementation state only when justified by the module contract.

HPS-HDR-004: Public C function declarations, external object declarations and C
callback function types must have guarded C language linkage for C++ consumers:

```c
#ifdef __cplusplus
extern "C" {
#endif

/* Public C function declarations and C callback types. */

#ifdef __cplusplus
} /* extern "C" */
#endif
```

Use this only in declaration-bearing headers. Macro-only/ordinary object-type
headers need no empty linkage block. Do not wrap third-party includes or C++
classes/templates in it. The implementation remains C; consumers include the
public header rather than writing their own linkage wrappers. An additional
`extern` keyword on each C function prototype is optional and not required by
this project; public mutable globals remain prohibited in the core.

HPS-HDR-005: Each API has a complete named-parameter prototype and documented
input/extent, ownership, lifetime, aliasing, initialization, output/failure and
execution-context contract. Use `(void)` for no parameters. Public names use the
HPS namespace; all definitions include their own public header before other
implementation dependencies. Avoid ABI changes driven by undisclosed macros,
packing pragmas or compiler-specific calling conventions.

Headers declaring `int32_t` error results include `hps/base/hps_stdint.h`
explicitly under HPS-HDR-003. Size/difference declarations similarly include
`hps/base/hps_stddef.h`, and Boolean declarations use `hps/base/hps_bool.h`.
Only those providers include the corresponding native standard headers in core
code. Headers using module-local error constants explicitly include their
defining header, such as `hps/bytes/hps_view.h` for `HPS_BYTE_*`. Document the error
domain and keep constants typed; follow HPS-C-004/HPS-C-019 in
[the C rules](c-rules.md). Do not add a shared status typedef or registry
dependency. Return-type width changes and numeric-code changes need an explicit
API/ABI migration record even when function and error names remain unchanged.

HPS-HDR-006: Guarded language linkage does not establish full C++ or ABI compatibility.
Qualification must specify C/C++ compiler versions/modes and verify independent
and repeated inclusion in both languages, then link a C++ caller with objects
compiled as C. Check layouts, alignment, status representation and callback ABI
on each promised target. C++ compatibility must not enable C-only features merely
because C++ has superficially similar syntax. Earlier host text/span/key
include/link checks are recorded in the
[text migration review](reviews/csep-text-2026-10-13.md); the changed error-result
ABI requires current verification in the
[removal review](reviews/status-removal-2026-10-18.md). Other target support remains
unqualified.

The [C++ working draft's linkage section](https://eel.is/c++draft/dcl.link)
is the reference for language linkage; target interoperability still requires
qualification. These are HPS header conventions, not a claim that MISRA mandates
this exact comment style or C++ support.

## Template

See [the public-header template](templates/public-header.h). It demonstrates a
direct `int32_t` result, typed local errors and a guarded C API. Replace names,
guard and contract with those of the actual module; the example is not an
implemented API and requires no framework registration.

The lightweight checker verifies the expected guard/file metadata and a limited
pattern for linkage blocks around HPS function/extern declarations. It does not
parse every possible C declarator, prove dependency completeness or verify ABI.
