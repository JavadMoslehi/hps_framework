# Typed data-structure descriptors

`hps/data_structure/define.h` extends the file-scope generator pattern from
the [read-only span generator](span.md) in the same header to the eleven families
supplied in the user's draft.
Each definition takes an explicit element type and a unique prefix. It emits
`name_s`, `name_t` and an internal `static HPS_INLINE name_make(...)` constructor.
`hps/data_structure/all.h` is the module umbrella. The former
`hps/data_structure.h` and `hps/hps_data_structure.h` paths remain forwarding
compatibility headers; new consumers use the module paths. Generator names and
behavior are unchanged by this include-path migration.
For example:

```c
#include <stdint.h>
#include "hps/data_structure/define.h"

HPS_VECTOR_DEFINE(uint16_t, hps_words)
HPS_SVECTOR_DEFINE(uint16_t, hps_local_words, 8U)
HPS_SLIST_DEFINE(uint16_t, hps_word_list)

static uint16_t words[4U];
static hps_words_t initial = HPS_VECTOR_STATIC_INIT(words, 0U, 4U);
static hps_local_words_t local_words = HPS_SVECTOR_STATIC_INIT;
static hps_word_list_node_t first = HPS_SLIST_NODE_STATIC_INIT(7U, NULL);
static hps_word_list_t list = HPS_SLIST_STATIC_INIT(&first, 1U);
```

The element type must be a complete object type usable in the generated fields
and constructors. C++ consumers must use trivial C-compatible element types;
nontrivial constructors/copies could add unbounded work or hidden allocation.
Node values must be assignable by value; array-valued nodes are unsupported.
Use a typedef for pointer/array declarators, compound type spellings
or types containing commas. In particular, a pointer typedef lets `const type_`
qualify the element pointer itself in a read-only view. Prefixes are literal
identifier tokens, unique across all instantiations
in a translation unit. The generators use token concatenation; they do not derive
public names from the spelling of the element type.

| Generator | Generated container fields, in declaration/constructor order |
| --- | --- |
| `HPS_VECTOR_DEFINE(type_, name_)` | `type_ *data`, `size_t length`, `size_t capacity` |
| `HPS_MUTABLE_SPAN_DEFINE(type_, name_)` | `type_ *data`, `size_t length` |
| `HPS_CSPAN_DEFINE(type_, name_)` | `const type_ *data`, `size_t length` |
| `HPS_ROBUFFER_DEFINE(type_, name_)` | `const type_ *data`, `size_t length`, `size_t capacity` |
| `HPS_SVECTOR_DEFINE(type_, name_, capacity_)` | `type_ data[capacity_]`, `size_t length` |
| `HPS_SRING_DEFINE(type_, name_, capacity_)` | `type_ data[capacity_]`, `size_t head`, `size_t tail`, `size_t length` |
| `HPS_RING_DEFINE(type_, name_)` | `type_ *data`, `size_t head`, `size_t tail`, `size_t length`, `size_t capacity` |
| `HPS_STACK_DEFINE(type_, name_)` | `type_ *data`, `size_t length`, `size_t capacity` |
| `HPS_SLIST_DEFINE(type_, name_)` | `name_node_t *head`, `size_t length` |
| `HPS_DLIST_DEFINE(type_, name_)` | `name_node_t *head`, `name_node_t *tail`, `size_t length` |
| `HPS_BST_DEFINE(type_, name_)` | `name_node_t *root`, `size_t length` |

`HPS_CSPAN_DEFINE` delegates to the existing read-only `HPS_SPAN_DEFINE`; its
`HPS_CSPAN_MAKE` and `HPS_CSPAN_STATIC_INIT` helpers use the same representation
and construction semantics.
`HPS_SPAN_DEFINE` retains its established read-only meaning. The mutable draft
span therefore uses the explicit `HPS_MUTABLE_SPAN_*` family.

For each family, `HPS_<FAMILY>_MAKE(name_, ...)` constructs a value and
`HPS_<FAMILY>_STATIC_INIT(...)` supplies an aggregate initializer. Arguments follow
the field order in the table. Static vector/ring constructors take no data or
metadata arguments: `name_make(void)`, `HPS_SVECTOR_MAKE(name_)` and
`HPS_SRING_MAKE(name_)` create an empty object. Their static initializers are
object-like macros, with no parentheses. Other initializers require expressions
suitable for the intended storage duration; static-storage objects require
constant initializers.

C99 `MAKE` helpers use `HPS_LVALUE` compound literals where available, and other
modes call the generated function. Runtime arguments are each evaluated once;
the order between separate arguments is unspecified. Do not pass conflicting
side effects. C compound literals follow ordinary object/block lifetime rules.
The generated helper functions have internal linkage and create no external
library symbol or shared mutable state. A successful host compile in another
language mode is limited syntax evidence, not target qualification.

Lists and trees additionally generate `name_node_s`, `name_node_t` and
`name_node_make(...)`. The `HPS_SLIST_NODE_MAKE`, `HPS_DLIST_NODE_MAKE` and
`HPS_BST_NODE_MAKE` helpers take the prefix first, followed by the node fields.
Corresponding `*_NODE_STATIC_INIT` helpers omit the prefix. Field order is
`value, next` for singly linked nodes, `value, prev, next` for doubly linked nodes,
and `value, left, right` for tree nodes. Constructing a container does not create,
copy or traverse any nodes. Node static initializer macros accept expression
values, so a scalar/pointer constant expression is the portable static-storage
use. A brace-only nested initializer cannot be passed as their value argument;
use an explicit aggregate initializer for such a structured static node value.
Runtime node constructors can copy assignable structure values.

Construction is unchecked, like aggregate initialization. Counts/capacities count
elements, not bytes. The caller proves `length <= capacity` where capacity exists,
valid typed storage extents and initialization before any element is read.
Null storage requires zero corresponding extent. A ring with nonzero capacity
requires in-range head/tail indexes: head is the next read position, tail is the
next write position and equals head advanced by length with wrap. An empty
zero-capacity ring uses zero indexes/length. A nonempty list must be acyclic,
contain exactly length live nodes and have consistent endpoints/reciprocal links.
A tree has exactly length distinct reachable nodes, with no cycles/shared
children; its search ordering belongs to the application. Empty linked containers
use null endpoints/root and zero length. Constructors do not validate cycles,
stale pointers or tree ordering.
Invalid input has no status/error channel; validate untrusted metadata before
construction or use.

Pointer-backed containers are descriptors for caller-provided storage. They add
no allocation, growth, reservation, release, insertion, traversal or balancing
operation. Names such as vector/stack/tree establish a layout, not an allocator
or ownership transfer. Callers retain storage/node ownership and control their
lifetime, mutation, release and aliasing. A const view prevents writes through
that view; it does not prevent another alias from changing the backing objects.
Serialize conflicting access, mutation and release in one execution context.
No thread, ISR, lock-free or signal-safety guarantee is made.

Static vector/ring objects contain their own typed arrays. `capacity_` must be a
positive integer constant expression; zero, negative and runtime capacities are
unsupported. Their constructors initialize every element using C aggregate
initialization (C++ value initialization) and set every counter/index to zero.
This creates typed zero/null values without assuming all-bits-zero pointer or
object representations. Padding is unspecified and must not be compared or
serialized. Construction and by-value copies can cost O(capacity); their full
object size, constructor temporaries and stack budget belong to the integrating
target. No measured cost, target RAM budget, erasure, hardware alignment or WCET
claim follows. Other constructors copy O(1) metadata or one node payload; they
copy no backing array.

The [change review](../../governance/reviews/data-structure-2026-10-18.md) records
draft naming migration, policy/MISRA mapping, actual checks and remaining gaps.
