# Public-header layout migration — 2026-10-21

This is the current include-path map for the user-requested package cleanup.
Source base: `9d597f8178e1bef2fba18b6ffaa334778a90a57b` plus this change.
Earlier migration records and snapshots retain their historical paths and evidence;
this map supersedes their header-placement and compatibility-forwarder guidance.
No legacy reference tree, archive, snapshot or source inventory is changed.

## Consumer changes

`hps/base.h` is the only header directly beneath `include/hps` and is now the
actual Base umbrella. Every other package provides `<package>/<package>.h`;
leaf headers use `hps_<name>.h`. Base's architecture/features subpackages retain
their directories and have named nested umbrellas. The root/span compatibility
headers and all `all.h` paths are removed, with no compatibility shim retained.
Update source includes, regenerate builds and install into a fresh prefix so that
obsolete installed headers cannot hide missed migrations. Files under an existing
`build/...-install/include` tree are generated copies, not the source of truth.
Overlay installation with CMake/Meson does not prune removed headers. The IDE's
`build/bytes-features-optional-install` tree is a historical generated prefix;
fresh verification uses `build/header-layout-install` without rewriting that tree.

Data Structure symbols/macros, configuration switches, retained structure
field order/layout, error numeric values, evaluation rules, ownership, checked
bounds and resource contracts are preserved. The unused conflicting Bytes endian
enum is removed as explicitly recorded below. Include guards change to match the new public paths;
consumers depending on old guard macros must update. This is an intentional
source include-path break, not a new binary interface or error-domain design.
Fresh compilation/linkage evidence is recorded in the
[development review](../../governance/reviews/public-headers-2026-10-21.md).

## Package umbrellas

| Package | Complete umbrella |
| --- | --- |
| Base | `hps/base.h` |
| Base architecture | `hps/base/architecture/architecture.h` |
| Base features | `hps/base/features/features.h` |
| Algorithm | `hps/algorithm/algorithm.h` |
| Bytes | `hps/bytes/bytes.h` |
| Cstr | `hps/cstr/cstr.h` |
| Data Structure | `hps/data_structure/data_structure.h` |
| Enum | `hps/enum/enum.h` |
| JSON | `hps/json/json.h` |
| Memory | `hps/memory/memory.h` |
| Platform metadata | `hps/platform/platform.h` |
| Optional hosted adapter | `hps/platform/hosted/hosted.h` |
| Result | `hps/result/result.h` |
| String | `hps/string/string.h` |
| Validation | `hps/validation/validation.h` |

Each umbrella includes its complete package's public headers. Platform metadata
and the optional hosted adapter are separate packages: the metadata umbrella
never includes hosted diagnostics. Base includes no higher-layer runtime API.
Memory's umbrella also includes the allocator/clone/allocated-array/owner proposal
declarations. Including them supplies no heap/provider and calls no callback;
using those APIs remains subject to the pending HPS-MEM-001 disposition. Narrow
consumers may include individual leaves instead. Existing build targets and the
default-disabled hosted adapter retain their runtime dependency contracts.

## Complete previous-path map

The three standard-name Base providers retain their existing paths. An umbrella
replacement preserves the old header's available package API; consumers wanting
only one type should choose the type-specific leaves in the next section.

| Previous public path | Current public path |
| --- | --- |
| `hps/algorithm/bits.h` | `hps/algorithm/hps_bits.h` |
| `hps/algorithm/scalar.h` | `hps/algorithm/hps_scalar.h` |
| `hps/base.h` | `hps/base.h` |
| `hps/base/all.h` | `hps/base.h` |
| `hps/base/architecture.h` | `hps/base/architecture/architecture.h` |
| `hps/base/architecture/endian.h` | `hps/base/architecture/hps_endian.h` |
| `hps/base/architecture/id.h` | `hps/base/architecture/hps_id.h` |
| `hps/base/architecture/pointer.h` | `hps/base/architecture/hps_pointer.h` |
| `hps/base/attributes.h` | `hps/base/hps_attributes.h` |
| `hps/base/compiler.h` | `hps/base/hps_compiler.h` |
| `hps/base/expression.h` | `hps/base/hps_expression.h` |
| `hps/base/features.h` | `hps/base/features/features.h` |
| `hps/base/features/architecture.h` | `hps/base/features/hps_architecture.h` |
| `hps/base/features/compiler.h` | `hps/base/features/hps_compiler.h` |
| `hps/base/features/language.h` | `hps/base/features/hps_language.h` |
| `hps/base/hps_bool.h` | `hps/base/hps_bool.h` |
| `hps/base/hps_stddef.h` | `hps/base/hps_stddef.h` |
| `hps/base/hps_stdint.h` | `hps/base/hps_stdint.h` |
| `hps/base/language.h` | `hps/base/hps_language.h` |
| `hps/base/macros.h` | `hps/base/hps_macros.h` |
| `hps/base/standard.h` | `hps/base/hps_standard.h` |
| `hps/base/version.h` | `hps/base/hps_version.h` |
| `hps/bytes/buffer.h` | `hps/bytes/hps_buffer.h` |
| `hps/bytes/codec.h` | `hps/bytes/hps_codec.h` |
| `hps/bytes/endian.h` | `hps/bytes/hps_endian.h` |
| `hps/bytes/order.h` | `hps/bytes/hps_order.h` |
| `hps/bytes/reader.h` | `hps/bytes/hps_reader.h` |
| `hps/bytes/view.h` | `hps/bytes/hps_view.h` |
| `hps/cstr.h` | `hps/cstr/cstr.h` |
| `hps/cstr/algorithm.h` | `hps/cstr/hps_algorithm.h` |
| `hps/cstr/all.h` | `hps/cstr/cstr.h` |
| `hps/cstr/convert.h` | `hps/cstr/hps_convert.h` |
| `hps/cstr/core.h` | `hps/cstr/hps_core.h` |
| `hps/cstr/format.h` | `hps/cstr/hps_format.h` |
| `hps/data_structure.h` | `hps/data_structure/data_structure.h` |
| `hps/data_structure/all.h` | `hps/data_structure/data_structure.h` |
| `hps/data_structure/define.h` | `hps/data_structure/data_structure.h` |
| `hps/data_structure/operations.h` | `hps/data_structure/data_structure.h` |
| `hps/data_structure/optional.h` | `hps/data_structure/hps_optional.h` |
| `hps/enum/cstr.h` | `hps/enum/hps_cstr.h` |
| `hps/enum/traits.h` | `hps/enum/hps_traits.h` |
| `hps/hps_allocator.h` | `hps/memory/hps_allocator.h` |
| `hps/hps_data_structure.h` | `hps/data_structure/data_structure.h` |
| `hps/json/all.h` | `hps/json/json.h` |
| `hps/json/reader.h` | `hps/json/hps_reader.h` |
| `hps/json/tokenizer.h` | `hps/json/hps_tokenizer.h` |
| `hps/json/writer.h` | `hps/json/hps_writer.h` |
| `hps/memory/allocated_array.h` | `hps/memory/hps_allocated_array.h` |
| `hps/memory/allocator.h` | `hps/memory/hps_allocator.h` |
| `hps/memory/array.h` | `hps/memory/hps_array.h` |
| `hps/memory/clone.h` | `hps/memory/hps_clone.h` |
| `hps/memory/operations.h` | `hps/memory/hps_operations.h` |
| `hps/memory/owner.h` | `hps/memory/hps_owner.h` |
| `hps/memory/pool.h` | `hps/memory/hps_pool.h` |
| `hps/memory/refcount.h` | `hps/memory/hps_refcount.h` |
| `hps/platform/detection.h` | `hps/platform/platform.h` |
| `hps/platform/hosted/debug.h` | `hps/platform/hosted/hps_debug.h` |
| `hps/platform/libc.h` | `hps/platform/hps_libc.h` |
| `hps/platform/os.h` | `hps/platform/hps_os.h` |
| `hps/platform/posix.h` | `hps/platform/hps_posix.h` |
| `hps/result/check.h` | `hps/result/hps_check.h` |
| `hps/span.h` | `hps/data_structure/data_structure.h` |
| `hps/span/all.h` | `hps/data_structure/data_structure.h` |
| `hps/span/define.h` | `hps/data_structure/data_structure.h` |
| `hps/string.h` | `hps/string/string.h` |
| `hps/string/all.h` | `hps/string/string.h` |
| `hps/string/buffer.h` | `hps/string/hps_buffer.h` |
| `hps/string/builder.h` | `hps/string/hps_builder.h` |
| `hps/string/char.h` | `hps/string/hps_char.h` |
| `hps/string/fixed.h` | `hps/string/hps_fixed.h` |
| `hps/string/indent.h` | `hps/string/hps_indent.h` |
| `hps/string/policy.h` | `hps/string/hps_policy.h` |
| `hps/string/tokenizer.h` | `hps/string/hps_tokenizer.h` |
| `hps/string/view.h` | `hps/string/hps_view.h` |
| `hps/validation/national_code.h` | `hps/validation/hps_national_code.h` |

## Data Structure decomposition

The old `define.h` macros are distributed by type rather than kept behind a
renamed aggregate. Vector and doubly linked list operation generators live with
their descriptor generators; the old `operations.h` implementation is removed.
The optional-value generator has its own leaf. The umbrella includes all thirteen
type headers plus both shared providers; public generator spellings are unchanged.

| Leaf under `hps/data_structure/` | Public family or shared purpose |
| --- | --- |
| `hps_span.h` | `HPS_SPAN_*` read-only borrowed span |
| `hps_mutable_span.h` | `HPS_MUTABLE_SPAN_*` mutable borrowed span |
| `hps_cspan.h` | `HPS_CSPAN_*`, delegating to the read-only span provider |
| `hps_vector.h` | `HPS_VECTOR_*` descriptor and bounded operations/iterator |
| `hps_robuffer.h` | `HPS_ROBUFFER_*` read-only capacity descriptor |
| `hps_svector.h` | `HPS_SVECTOR_*` in-place vector |
| `hps_sring.h` | `HPS_SRING_*` in-place ring |
| `hps_ring.h` | `HPS_RING_*` caller-storage ring |
| `hps_stack.h` | `HPS_STACK_*` caller-storage stack |
| `hps_slist.h` | `HPS_SLIST_*` singly linked list and node |
| `hps_dlist.h` | `HPS_DLIST_*` doubly linked list/node and bounded operations/iterator |
| `hps_bst.h` | `HPS_BST_*` binary tree and node |
| `hps_optional.h` | `HPS_OPTIONAL_*` typed optional value |
| `hps_collection.h` | Typed `HPS_COLLECTION_*` errors and shared operation contract |
| `hps_static_init.h` | `HPS_DATA_STRUCTURE_EMPTY_INIT` and static-array construction contract |

For example, replace a span-only `#include "hps/span.h"` with
`#include "hps/data_structure/hps_span.h"`. Replace a consumer of all descriptor
families with `#include "hps/data_structure/data_structure.h"`. A vector consumer
can include `hps_vector.h` for both its descriptor and operation generator;
`hps_dlist.h` follows the same rule. Construction remains unchecked; checked
operation errors and caller-owned extent/lifetime/serialization rules remain in
force. See the [current type contract](../architecture/data-structure.md).

## Continuation and remaining gates

Use `include/hps`, current architecture documents and this map for subsequent
work. Keep the legacy inventories and lossless snapshots read-only. Historical
build/install trees may retain obsolete names until regenerated. Target/compiler
qualification, named independent review, licensed MISRA analysis and existing
policy exception proposals remain open; header organization grants none of those
approvals. The current [review](../../governance/reviews/public-headers-2026-10-21.md)
separates actual fresh checks from pending or historical evidence.

## Removed conflicting Bytes endian type

Including the complete packages together exposed an existing name collision:
Base defines `HPS_ENDIAN_UNKNOWN/LITTLE/BIG` as detection values 0/1/2, while
Bytes' unused `hps_endian_t` enum declared `HPS_ENDIAN_LITTLE/BIG` as 0/1.
The obsolete Bytes enum/typedef is removed from `hps/bytes/hps_endian.h`.
Base detection keeps its established names and values. The codec selector remains
`uint8_t hps_byte_order` with `HPS_BYTE_ORDER_LITTLE_ENDIAN` = 0 and
`HPS_BYTE_ORDER_BIG_ENDIAN` = 1; all codec APIs already use that selector.

Consumers using the removed enum must intentionally migrate their declarations
and constants to `hps_byte_order`/`HPS_BYTE_ORDER_*`. Do not substitute Base's
detection constants into codec arguments: the numeric domains differ. Persisted
or externally exchanged enum storage requires a separate application layout
decision; no enum-to-byte storage compatibility is asserted. No exported function
signature, codec selector value, error numeric code or runtime byte-order behavior
changes. The correction also removes one existing numerical-enum policy finding.
