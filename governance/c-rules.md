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
The byte-block pool currently requires `uint8_t` storage and `int32_t` error
results. `size_t` describes object sizes/counts and `ptrdiff_t`
a representable pointer difference; neither specifies
wire format. `intptr_t`/`uintptr_t` require an explicit target-approved exception,
not a convenience cast. Floating-point support requires a separately reviewed
module and policy exception with numerical error bounds.

HPS-C-003: Plain `char` is reserved for C-string interfaces (`char *` or
`const char *`). Use `uint8_t` arrays for binary payloads and protocol octets.
Input strings must include a capacity/maximum length contract and verified NUL
termination. String literals must be treated as immutable. No numeric operations
on character storage, mutable pointers to string literals or unbounded string APIs.
Do not replace character pointers with numeric pointers to bypass type checks.

HPS-C-004: Flags use `uint8_t` with documented values; conditions compare
explicitly to the intended value. Counter/flag storage type is not itself a Boolean
condition. Use ordinary return/output types suited to the API contract: pointers
for borrowed/owned objects with explicit failure/lifetime rules, counts for sizes,
and `int32_t` directly for module operations with several distinguishable errors.
Do not introduce a shared status type or status aliases. Error constants must have
the declared return type and follow HPS-C-019. Do not introduce numerical enum
storage or implementation-dependent enum ABI.

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

## Readability, shared behavior and local error results

HPS-C-017: Make a function's purpose, invariants and state transitions apparent
from descriptive names and local control flow. Keep validation, arithmetic bounds
and mutation order easy to review. Comments explain why a check, conversion,
ownership restriction or resource bound is needed rather than repeat the statement.
Avoid unnecessary indirection or compressed expressions that conceal these facts.
Readability changes must preserve documented failure priority and resource bounds.

HPS-C-018: Avoid maintaining duplicate implementation logic for the same contract.
Factor a shared operation when its inputs, ownership, initialization, error priority,
failure preservation and resource behavior are the same and remain visible at the
call sites. Similar syntax alone does not justify a shared helper. Do not remove
checks, obscure typed storage or add allocation/concurrency to reduce repetition.
Record justified specialization or repetition, including differences in contracts
or target constraints, in the change record. A planned optimization is not measured
performance evidence. Independent test expectations must remain independent of
the implementation they verify; this requirement is not a blanket ban on macros
or repeated declarations.

HPS-C-019: Error-returning module operations use direct `int32_t` results: zero
is success and errors are negative typed constants published by the owning API.
Error values are local to that API and may overlap with another module's values.
Keep named comparisons and document error priority, propagation and failure
preservation. A dependent API may explicitly reuse its provider's error constants
when it shares their meaning; no framework namespace, module IDs, shared registry,
diagnostic-string lookup or registration step is required. Do not interpret a
bare code without its API context or silently propagate a different error domain.
Keep pointer/count/predicate results where their existing contracts fit rather
than turning every result into an error code. Review public return/callback ABI,
stored codes, foreign interfaces and target resources whenever representation or
numeric values change; rebuild affected consumers and document explicit mappings.

The HPS-C-004/HPS-C-019 amendment is prepared at the user's request in the
[removal review](reviews/status-removal-2026-10-18.md). It supersedes the shared
status portion of the [earlier policy change](reviews/status-2026-10-11.md).
HPS-C-017/HPS-C-018 remain unchanged. Project-role/release disposition remains
pending; these edits do not approve an external-standard deviation or establish
release qualification. Historical host results do not verify the changed API/ABI.

## Automated subset

The policy checker covers selected spellings, heap/unsafe API calls, enum/union
keywords, exact-type aliases, hosted exceptions, pool concurrency dependencies
and selected public-header structure/linkage checks.
Semantic alias analysis, valid arithmetic, string termination, loop bounds, ABI,
whole-program MISRA findings and external libraries need qualified tools and review.

The source checker distinguishes the separately built, default-disabled hosted
adapter `src/platform/hosted/debug.c` under HPS-PRO-002: only that file may include
stdio. Numerical, allocation and concurrency checks still apply there. Public
headers and other core files retain the I/O restriction, and core includes of
hosted adapter headers are rejected. Scope self-tests cover both sides of this
boundary. This implements the existing adapter boundary; it approves no new
heap/provider, target or external-standard exception.
