# Typed optional values

Include `hps/data_structure/optional.h` or `data_structure/all.h`. Expand
`HPS_OPTIONAL_DEFINE(type, name)` once at file scope after defining a complete,
copyable assignable type. Use a typedef for pointers; C++ values must be trivial
C-compatible objects. The header reuses existing typed aggregate-initialization
machinery in `data_structure/define.h`; it owns no heap or shared runtime state.

The generated `name_t` stores a uint8 `has_value` flag and one real typed value.
`name_none()` zero-initializes the payload using language semantics, including
actual null pointers. `name_some(value)` copies a present value. Presence zero
and one are the only valid flags; a present NULL pointer differs from none.
Initialize through a constructor before using checked operations.

Generated `check`, `has`, `get`, `get_or`, `set_none` and `set_some` use direct
int32 results with local `HPS_OPTIONAL_OK/INVALID_ARGUMENT/INVALID_STATE/EMPTY`.
NULL required arguments precede invalid tag state; only get reports EMPTY, without
changing its output. Has writes exactly zero or one; get_or copies the present
value or the supplied fallback. All failures preserve state and outputs. Output
objects must be live, correctly typed and disjoint from the optional and borrowed
payloads. Pointer/flag checks cannot prove arbitrary C pointer validity.

Set-none clears presence only, retaining the initialized payload. Set-some copies
before publishing presence. There is no destruction, ownership transfer, implicit
retain/release, sensitive erasure or callback. Copying/discarding values must be
permitted by their own ownership/lifetime contracts. Ordinary nullable pointers
remain sufficient for borrowed references; a second reference-wrapper type is
unnecessary.

Arguments evaluate once under ordinary C function rules, with no promised order.
Get-or evaluates fallback eagerly even when a value is present, an explicit
change from the legacy conditional macro. Use caller control flow for lazy work.
No unchecked unwrap, anonymous-struct expression language or shared error/result
wrapper is introduced.

Storage is sizeof(element) plus one uint8 and native padding per optional, with
fixed type-sized constructor/value-copy temporaries. Flag checks/changes are
O(1); copying a large structure costs its full fixed size. Caller budgets storage
and serialized access; host checks do not qualify target ABI/stack/timing or
concurrency. See the [migration](../migration/bytes-features-optional.md) and
[development review](../../governance/reviews/bytes-features-optional-2026-10-19.md).
