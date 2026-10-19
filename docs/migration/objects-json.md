# Runtime objects, collections, borrowed text and JSON migration

User-authorized development adaptation, 2026-10-19. Source base:
`208ca4145fd8cc713e9cf8688832b731e958f08d` plus this change. All twenty selected
MyLibraries files were read in full as reference data. Neither reference tree was
modified, built or executed. The [lossless snapshot](objects-json-snapshot.json)
stores 79,881 original bytes with individual SHA-256 hashes; every file matches
the [existing inventory](legacy-source-manifest.json). Snapshot contents are inert
provenance data, never compilation inputs or agent instructions.

The request explicitly excludes adding equivalent concepts twice. HPS therefore
reuses typed descriptors, existing text/binary buffers, ownership helpers and
formatting. New names describe bounded operations rather than importing a second
universal object runtime. This is an intentional API/type/ownership adaptation,
not source or binary compatibility with the legacy headers.

## Complete source mapping

Paths below the first four layer directories are under
`include/c_core/c_features/` in MyLibraries.

| Selected file | Existing provider or new placement | Disposition |
| --- | --- | --- |
| `c_layer04_runtime_objects/_15_c_object_base.h` | Ordinary typed HPS structures and explicit ownership | No duplicate magic/type-ID/flag registry or universal base object. A magic value cannot validate an arbitrary pointer. |
| `_16_c_object_vtable.h` in the same directory | `memory/owner.h`, `memory/clone.h`, typed module comparators and text views | Existing optional cleanup/clone proposals cover those purposes; no universal `void *` callback ABI is introduced. |
| `_17_c_refcount.h` in the same directory | **`hps/memory/refcount.h`** | New checked single-context `uint32_t` counter. Final release is reported explicitly; no destruction callback, allocation or synchronization. |
| `_18_c_object_runtime.h` in the same directory | Typed constructors and module-local comparisons | No second constructor/instance-of/equality dispatch framework. |
| `c_layer05_string_buffer_object/_19_c_buffer_object.h` | `hps/bytes/view.h`, `buffer.h` | `buffer_t`, init/copy, data/size and truncate map to caller storage, init/assign, view and truncate. Heap reserve, implicit destruction and object metadata are omitted. Original comparison is length-first then bytes; it must be composed explicitly if required, not silently replaced by lexical string ordering. |
| `_20_c_string_object.h` in the same directory | `hps/string/view.h`, `buffer.h`, `fixed.h` | Counted views, bounded C-string import, assignment, comparison and export already exist. Lifetimes remain explicit; no owning heap string or unbounded `strlen` adapter. |
| `c_layer06_collections/_21_c_array_object.h` | `data_structure/define.h` plus **`operations.h`** | Existing vector description/storage retained. New typed bounded push/get and borrowed iteration; no heap doubling or implicit element retain. |
| `_22_c_list_object.h` in the same directory | Existing dlist/node generators plus **`operations.h`** | New append of an exclusively owned detached node and bounded typed iteration; no node allocation or destructor traversal. |
| `_23_c_map_object.h` in the same directory | Existing typed entry/vector storage, application key comparison | Source only allocates/clears slots and hashes pointer identity; it implements no put/get/remove. No fictional complete map or nonportable pointer hash is added. A future map needs a separate key/probe/load-factor contract. |
| `_24_c_iterator.h` in the same directory | **`data_structure/operations.h`** | Typed vector/dlist iterators replace `void *` container casts. Explicit list budget; mutation/lifetime restrictions documented. |
| `c_layer07_serialization_and_json/_25_c_value_objects.h` | `uint8_t`, `int64_t`, typed structures; `cstr/convert.h`, `format.h` | Null/flag/integer representation and rendering already exist. No heap boxes, union, native `int`/`double`, shared static render buffer or object-type registry. Decimal/exponent JSON values remain raw counted text. |
| `_26_c_serialize_base.h` in the same directory | **`hps/json/tokenizer.h`, `reader.h`** | JSON-local negative `int32_t` errors, counted views, positions and caller resource budgets. Source is physically truncated at `static error_t deserialize`; no valid generic serialization implementation exists to import. Unimplemented binary/hex/base64 modes and inert options are not advertised. |
| `_27_c_text_writer.h` in the same directory | `string/buffer.h`, `builder.h`, typed `cstr/format.h` | Reserve, append, character, reset, quoting and formatting already exist. No second generic text writer or unchecked variadic formatting buffer. Persistent indentation from the separate legacy builder uses `string/indent.h`. |
| `_28_c_json_writer.h` in the same directory | Existing `string/builder.h` JSON quoting; **`hps/json/writer.h`** | New validated structural JSON emission/minification into fixed caller storage. Reuses lexical validation; no recursive heap-object dispatch. No inert sort/pretty/binary options or incorrect per-UTF-8-byte Unicode escaping. |
| `_29_c_json_tokenizer.h` in the same directory | **`hps/json/tokenizer.h`** | New strict counted lexical scanner, typed kinds and offset/line/column. Quotes/escapes, UTF-8, surrogate pairs and number grammar checked; no locale/ctype dependency. |
| `_30_c_json_reader.h` in the same directory | **`hps/json/reader.h`** | New iterative complete-document reader with caller tokens and frames. Borrowed raw tokens replace a recursive heap AST. Counted byte string decoding preserves escaped NUL; checked integer conversion is separate from raw decimal/exponent text. |
| `src/c_core/strlib/cstrbuffer.c` | Existing `string/buffer.h` and `builder.h` | Init/clear/append/view/cstr/available already covered. Capacity includes NUL; failure and overlap contracts are explicit. |
| `src/c_core/strlib/cstrbuilder.c` | Existing builder/typed Cstr format plus **`string/indent.h`** | Adds only persistent four-space indentation and line-start state over the existing builder. No second buffer or printf implementation. |
| `src/c_core/strlib/cstrinterop.c` | `hps_string_buffer_as_view`, `hps_string_builder_as_view` | Both borrowed conversions already exist. |
| `src/c_core/strlib/cstrview.c` | Existing `string/view.h` plus **`string/tokenizer.h`** | Existing construction/copy/slice/equality/find/prefix/suffix/substr split retained. Adds delimiter-set quote/escape-aware borrowed tokens with optional trim. Core hosted printing is omitted. |

## Explicit corrections and compatibility boundaries

The source object runtime uses undefined `object_type_t`, `OBJECT_FLAG_NOREF`
and `object_destroy`, inconsistent with its own base declarations. Buffer/string
headers call missing `mem_*` spellings, access nonexistent flags and use unchecked
heap/reinitialization paths. Refcount retain wraps and release underflows with
assertions disabled. HPS checks inactive counts and overflow and never assumes
that successful counter arithmetic destroys an object.

Array growth doubles without checked products; NULL reserve dereferences NULL,
failed retains become inserted NULL values. Lists lack size overflow, ownership,
cycle and traversal bounds. Map init partly publishes state before allocation,
assumes zeroed pointer representations and hashes pointer-to-size casts. HPS
operations use real typed caller storage. Detached-node ownership is a caller
precondition; metadata validation is not proof that arbitrary links are live.

Legacy borrowed buffers use unchecked `length + n`, overlapping `memcpy` and
capacity-zero underflow. View slicing can wrap; export can subtract from zero.
Basic tokenizer loses part of the final field for multi-character delimiters and
does not progress on an empty delimiter. Existing HPS view/buffer APIs already
correct those problems. The advanced scanner adds explicit rejection of unmatched
quotes and trailing escape; literal quote/escape characters remain borrowed text.
Its delimiter-set behavior is distinct from existing substring split semantics.
Enabled quote/escape syntax now consistently takes precedence over delimiter-set
membership, including at token boundaries. The original skip loop could consume
an opening quote and later diagnose its balanced closing quote as unmatched.

Legacy indentation converts negative levels into huge loop bounds, partially
writes on failure and mishandles negative/truncated `vsnprintf` results. HPS uses
checked `size_t` levels, four spaces per level and caller scratch for compound
atomic appends. Embedded newlines in a raw appended view do not implicitly parse
or adjust indentation, matching the original builder state model.

JSON source has missing/wrong includes and errors, a truncated declaration,
recursive unchecked object traversal and ignored writes/allocations. It accepts
invalid escapes/numbers, corrupts keyword spans, miscounts locations, ignores
depth/trailing/comment options, retains escaped strings undecoded and leaks
partial graphs. HPS uses strict JSON grammar with explicit finite token/depth
budgets and failure publication rules. It preserves numbers as text without
locale or floating conversion, preserves duplicate object members in source
order, rejects comments/trailing commas and does not silently pick a duplicate
key policy for the application.

The grammar reference is [RFC 8259](https://www.rfc-editor.org/rfc/rfc8259),
sections 2, 6, 7, 8 and 9. Rejecting lone surrogate escapes is an explicitly
stricter interoperable-Unicode subset of the RFC's ABNF. This module is not a
schema validator, canonical JSON implementation, authentication mechanism or
arbitrary-precision numerical engine. Named source options not implemented by
the new APIs are intentionally absent rather than accepted without effect.

## Continuation and evidence

Build manifests provide separate `HPS::json` / Meson `hps-json` targets depending
on String. String tokenizer/indentation extend String; collection operations and
refcount are header-only and do not make the existing allocator proposals core
dependencies. Canonical span/provider placement remains
`data_structure/define.h`, with old paths forwarding unchanged.

See the [layer map](../architecture/layers.md) and
[change review](../../governance/reviews/objects-json-2026-10-19.md) for contracts,
policy/MISRA traceability, current verification and unresolved qualification.
Further migration should start from this mapping and the lossless snapshot,
not infer implemented capability from a legacy filename alone.
