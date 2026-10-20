# Typed spans and expression helpers

`hps/data_structure/define.h` contains the header-only typed borrowed-span generator
alongside the other typed container generators.
It is outside the base umbrella because base describes the environment.
`hps/data_structure/all.h` includes the module API. The former `hps/span/define.h`,
`hps/span/all.h` and `hps/span.h` paths remain forwarding compatibility includes.
Generator names and behavior are unchanged.
For mutable spans and the other container families following this generator
pattern, see [typed data-structure descriptors](data-structure.md).

Define at file scope with an object type and a unique name:

```c
#include "hps/base/hps_stdint.h"
#include "hps/data_structure/define.h"

HPS_SPAN_DEFINE(uint16_t, hps_words)
static const uint16_t words[] = {1U, 2U};
static const hps_words_t initial = HPS_SPAN_STATIC_INIT(words, 2U);
```

Generated `hps_words_t` has `const uint16_t *data` and `size_t length`, counting
elements. `hps_words_make(data, length)` returns the initialized structure and
has internal `static HPS_INLINE` linkage, avoiding duplicate external definitions.
`HPS_SPAN_MAKE(hps_words, data, length)` uses a C99 compound literal when available
and the generated constructor in C89/C++. Both evaluate each argument once.
Construction is unchecked: NULL requires zero length, and callers prove actual
typed extent, initialized elements, lifetime and serialization. It allocates and
copies no payload. Empty pointers are retained as supplied. Static initialization
requires constant expressions. This is a typed aggregate, not a checked bytes API.

The user's names `HPS_SPAN_DEF`/`HPS_MAKE_SPAN` were normalized to
`HPS_SPAN_DEFINE`/`HPS_SPAN_MAKE`; `len` became `length`, matching HPS descriptors.
The generated function gained static linkage. C89 syntax does not establish
availability of every selected stdint type, compiler extension or target ABI.

`hps/base/expression.h`, included by base, owns `HPS_LIKELY`, `HPS_UNLIKELY`
and C99-only `HPS_LVALUE`. Both hints normalize to zero/one and evaluate their
expression once, including the fallback; this corrects the sample fallback's
different return-value semantics. Builtin expectation uses existing HPS compiler
capability probes. Hints convey no measured performance guarantee. Compound
literals obey C block/object lifetime rules; pointers must not escape that lifetime.
LVALUE is absent in C89/C++, and span construction selects its portable helper.
