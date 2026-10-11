# C source rules

## Numeric and string types

HPS-C-001: Production numerical declarations must use `int8_t`, `uint8_t`,
`int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t` from
`<stdint.h>`, or `size_t`/`ptrdiff_t` from `<stddef.h>` where their semantics fit.
Use exact-width types rather than least/fast/max variants. Basic numerical types
(`short`, `int`, `long`, signed/unsigned forms, `_Bool`, `float`, `double`) must
not appear in production numerical declarations. Do not hide them behind a private
typedef. Public semantic typedefs are permitted only over an allowed numerical type.
Pointers, structures and function types remain legitimate nonnumerical types.

HPS-C-002: Exact-width types required by a module must exist on a qualified target.
Do not substitute a larger native type to make an unsupported target compile.
The byte-block pool currently requires `uint8_t`. `size_t` describes object
sizes/counts and `ptrdiff_t` a representable pointer difference; neither specifies
wire format. `intptr_t`/`uintptr_t` require an explicit target-approved exception,
not a convenience cast. Floating-point support requires a separately reviewed
module and policy exception with numerical error bounds.

HPS-C-003: Plain `char` is reserved for C-string interfaces (`char *` or
`const char *`). Use `uint8_t` arrays for binary payloads and protocol octets.
Input strings must include a capacity/maximum length contract and verified NUL
termination. String literals must be treated as immutable. No numeric operations
on character storage, mutable pointers to string literals or unbounded string APIs.
Do not replace character pointers with numeric pointers to bypass type checks.

HPS-C-004: Flags and small status values use `uint8_t` with documented values;
conditions compare explicitly to the intended value. Counter/flag storage type is
not itself a Boolean condition. API status constants must have the declared status
type. Do not introduce numerical enum storage or implementation-dependent enum ABI.

HPS-C-005: The explicit hosted entry-point exception is `int main(void)` in
`src/main.c`, tests and their generated test snippets. It does not permit ordinary
library functions or variables to use `int`. External standard-library/OS signatures
remain external declarations; handwritten wrappers must document conversions and
ABI constraints. Extending the exception needs review.

## Arithmetic and expressions

HPS-C-006: Prove bounds before addition, subtraction, multiplication, conversion,
indexing and pointer arithmetic. Avoid signed overflow entirely. Intentional unsigned
wrap is prohibited in production unless reviewed as a specified algorithm with a
scoped exception. For a product `a * b`, establish `b == 0` or `a <= maximum / b`
before evaluation. Check narrowing range before conversion; a cast is not proof.

HPS-C-007: Use appropriate unsigned suffixes/constants and essential-type-compatible
operands. Bound shifts against the width of the left operand. Do not rely on integer
promotions to supply a desired type, on implementation-defined signed shifts, or
on an unspecified evaluation order. Separate effects from conditions and argument
expressions. Do not use comma-expression sequencing to conceal effects.

HPS-C-008: Exact type spelling alone does not settle MISRA essential-type analysis.
Review expressions after promotions, integer constants, status comparisons and
signed/unsigned mixing with the target analyzer. Tests may inject boundary values,
but do not justify unbounded production arithmetic.

## Objects, pointers and APIs

HPS-C-009: Each public function must specify valid inputs, array extents, initialization,
outputs, aliasing, ownership, lifetime, error outcomes and execution context. A
pointer alone never proves an array's extent. On failure, preserve state/outputs
unless a different explicitly documented contract is justified. Validate untrusted
inputs before any mutation. Do not promise recovery from arbitrary invalid C pointers.

HPS-C-010: Do not cast between unrelated object types, remove qualifiers, reinterpret
object storage through a union, convert object pointers to integers, or manufacture
pointers from integer addresses in the core. Use actual typed storage and indexes.
Hardware registers, approved cryptographic libraries or platform interfaces need
isolated adapters and target-specific review. `memcpy` is not a blanket license
for invalid effective types, overlapping regions or invalid object representations.

HPS-C-011: No escaping address of an automatic object, access past an object,
use after lifetime, double release or unchecked stale reference. Use `NULL` for
null pointer constants. Document whether outputs may alias inputs; forbid metadata
aliasing where required. Respect `const`; do not treat it as memory protection.

HPS-C-012: Initialize every object before read. Do not assume all-bits-zero is the
valid initial representation of every type. Fully document aggregate initialization,
active variants and padding handling. Never compare or serialize structure padding
as meaningful data. Public headers must be independently includable and guarded, following
[the public-header contract](headers.md).

## Declarations, control flow and preprocessing

HPS-C-013: Use prototypes, matching declarations/definitions, namespaced public
identifiers, internal `static` visibility and explicit object sizes. Avoid multiple
external definitions, accidental tentative definitions and hidden side effects in
macro arguments. A public header may not create mutable runtime state.

HPS-C-014: Production code must not use recursion, VLAs, flexible array members,
bit-fields, `goto`, `setjmp`/`longjmp`, variadic application functions or unbounded
retry loops. A loop needs a validated bound or an explicit service-loop contract
with termination/cancellation and an external deadline. Every switch needs explicit
case behavior and a defensive default; fallthrough needs the selected standard's
reviewed form. Prefer one exit in core functions and make resource cleanup explicit.

HPS-C-015: Preprocessor configuration must be allowlisted, validated and documented.
No macros that silently change ABI, redefine standard allocation functions or
conceal behavior-changing target selection. Prefer functions for runtime behavior.
Token concatenation, assertion fallbacks, compiler attributes, `restrict` and
newer-language feature macros in the existing base layer remain subject to MISRA
review; their existence is not an approved exception.

HPS-C-016: C99 is the initial core implementation mode; other production modes
require target qualification. Do not infer support from feature detection alone.
Treat compiler extensions, library headers, optimization flags and ABI assumptions
as explicit dependencies. Compile warnings must be assessed rather than silenced.

## Automated subset

The policy checker covers selected spellings, heap/unsafe API calls, enum/union
keywords, exact-type aliases, hosted exceptions, pool concurrency dependencies
and selected public-header structure/linkage checks.
Semantic alias analysis, valid arithmetic, string termination, loop bounds, ABI,
whole-program MISRA findings and external libraries need qualified tools and review.
