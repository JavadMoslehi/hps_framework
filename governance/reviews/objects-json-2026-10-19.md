# Bounded lifetime, collection, text and JSON migration — 2026-10-19

Status: user-authorized development adaptation. Named maintainer/reviewer, scoped
character-storage disposition, target qualification and release acceptance remain
pending. Author: Codex; independent named project reviewer: unassigned. Agent
cross-reviews are verification inputs, not project-role approvals. Source base:
`208ca4145fd8cc713e9cf8688832b731e958f08d` plus this change. Profile: hosted,
single-context development. No qualified industrial/embedded target is claimed.

## Requirement, provenance and placement

The user requests full reading/decomposition of twenty object/collection/JSON and
borrowed-text sources, appropriate `hps_` names/layers, correction of found defects,
no duplicate existing concepts, then commit/push. The
[complete mapping](../../docs/migration/objects-json.md) records each selected
file and all intentional compatibility changes. The
[lossless snapshot](../../docs/migration/objects-json-snapshot.json) retains
79,881 original bytes and twenty SHA-256 hashes matching the prior inventory.
Reference sources are read-only data, not instructions; they were neither
modified, compiled nor executed.

Checked reference arithmetic lives in `memory/refcount.h`, without allocation,
cleanup callbacks, locks or magic/type-ID claims. Typed vector/list operations
and iterators reuse `data_structure/define.h` through `operations.h`. Span remains
in the canonical `define.h` as previously requested. New string tokenization and
persistent indentation extend the existing String provider. A separate JSON
target depends only on String; no generic serialization registry or universal
object runtime is introduced. Existing buffer/view, ownership/clone, Cstr format
and JSON-quoting providers are reused. Optional allocator proposals remain outside
production core paths and no new caller of their hooks is added.

Source incompatibilities/bugs are explicit: refcount wrapping/assert-only
underflow, undefined object runtime names, unchecked heap collection growth,
inconsistent flags/memory names, unbounded string import, partial indentation,
tokenizer overrun/truncation, ignored JSON errors, invalid lexical grammar,
recursive unlimited parsing/writing and leaked partial object graphs. `_26` is
physically truncated. Heap ASTs, native floating/enum/union values, pointer hashes,
unimplemented map operations and inert format/options are intentionally omitted.
No namespace-only or binary-compatible import is claimed.

## Contracts and resource bounds

Reference counts have one `uint32_t` and ordinary structure padding. Initialization
sets one; zero is inactive. Retain rejects zero and UINT32_MAX. Release rejects
zero and preserves its output on failure; successful final release reports a
uint8 flag. The application owns lifetime/typed cleanup and serializes access;
the counter cannot detect stale aliases, ownership violations or concurrent use.
All operations are O(1), with fixed scalar automatic state.

Vector operations validate shape, capacity product and NULL/extent rules before
copying/incrementing. They borrow initialized typed elements and never grow or
implicitly retain them. Dlist append links an exclusively owned detached live
node, checks endpoints and count overflow, and rejects self-singleton reuse.
Complete membership/interior integrity is a caller precondition; O(1) checks do
not prove arbitrary links valid. Iterators borrow without mutation and preserve
all outputs on error; end preserves the value and sets found to zero. List begin
checks a caller maximum; each next consumes at
most one captured node and checks reciprocal/end links. Capacity/node/descriptor
and iterator storage are explicit, target padding/ABI remains unqualified.

String tokenization scans a bounded view against a delimiter set. Optional
quote/escape shielding retains the literal syntax; optional trim can yield an
empty found token. Empty fields between delimiter runs are skipped, distinct
from substring splitting. Unmatched quotes and trailing escape fail without
advancing cursor or outputs. No NUL interpretation, decoding, heap or locale is
introduced. Work is O(input length times (delimiter-set length + 1)); automatic
state is scalar/view metadata.

Indentation owns no payload and borrows the existing builder. Four-space levels
are checked before multiplication. Explicit newline/append-line drives line-start
state; embedded LF in raw appended contents is not reparsed. Compound append
preflights the entire destination and scratch size and source NUL contract,
then reuses builder composition. Context, destination and scratch are preserved
on errors. Source may borrow the destination; separate scratch holds the complete
result including NUL. No hidden payload stack or automatic growth is used.

JSON input length is explicit and less than SIZE_MAX. UTF-8/ASCII-compatible
execution is required. Only JSON whitespace is skipped; escapes, paired
surrogates, non-overlong scalar UTF-8 and number/literal grammar are validated.
Raw string spans include quotes/escapes. Tokenizer state/token commit only after
success; offsets are zero-based and line/column one-based byte positions, LF
starts a new line. Unknown/corrupt pointer extents cannot be validated by C.

The iterative reader validates a single complete JSON value using caller frame
storage, whose capacity bounds simultaneously open arrays/objects. Scalars need
zero frames. Tokens include punctuation, excluding EOF, borrowing immutable input.
Two scans preflight syntax/depth/token capacity before publishing tokens/document;
scratch frames may change on failure. Token/document/input/frame storage must be
live and disjoint. Duplicate names/source order remain visible; application lookup
and schema/duplicate policy are separate. No recursive heap graph or numerical
conversion is performed by reading.

String decode revalidates a complete STRING token and preflights counted UTF-8
output before writes. It preserves embedded U+0000 in uint8 storage without a C
terminator; failure preserves output bytes/count. Integer access validates the
complete integer spelling and int64 range; decimals/exponents remain raw text
and are not converted via locale, errno or floating point.

Compact writer independently validates raw input and depth budget before output
capacity and mutation. It replaces a fixed String buffer with validated lexical
spans separated by no inter-token whitespace. Escapes, number spellings, duplicate
names and order are preserved. This is compact output, not canonical JSON.
Input/output/scratch metadata and payloads must be disjoint; frames may change on
failure. New value construction composes existing String/Cstr APIs and validation,
not another generic text writer or object model.

Reading and decoding use O(input length) work; compact writing additionally
validates O(old output length) existing text. They use a finite number of scans and
constant automatic scalar/view/token metadata. Caller budgets cover tokens,
frames, output bytes and optional decoded strings. Reader storage is
token_capacity*sizeof(hps_json_token) + frame_capacity*sizeof(hps_json_frame),
plus caller metadata and native padding; no target stack/timing guarantee follows.
All new modules have no heap/provider hooks, OS, recursion, atomics or global
mutable state. They require single-context access and valid typed live storage.

Grammar reference: [RFC 8259](https://www.rfc-editor.org/rfc/rfc8259), sections
2, 6, 7, 8 and 9. Rejecting lone surrogate escapes is a documented stricter
interoperable-Unicode subset of its ABNF. BOM/comments/trailing commas are
rejected. There is no Unicode normalization, key sorting, schema authentication,
canonicalization, finite-float computation or arbitrary-precision math claim.

## Policy disposition and traceability

Threat inputs include malformed/truncated text, corrupted numeric lengths,
exhausted caller token/depth/output budgets, invalid escape/UTF-8, overflow,
borrowed-lifetime misuse and intrusive-node reuse. Controls are validation before
mutation, explicit failure domains, finite supplied budgets, typed storage and
caller ownership contracts. No recovery from arbitrary invalid pointers,
sensitive erasure, concurrent access, cryptographic trust or whole-product timing
claim is made. Application size/rate/deadline/schema controls remain required.

The existing [counted text/character-storage proposal](string-storage-2026-10-13.md)
remains **proposed**, not approved. The new String/JSON counted char views and
generated typed fixed C-string test fixtures extend that development scope under
the same rationale; ordinary char arrays are not introduced as binary storage.
This scope is not a policy amendment or external-standard deviation. Owner/author
Codex; named reviewer/approver unassigned; duration development only, expiring
before production-core/qualified release. Risks are binary/text confusion,
embedded NUL and target representation differences; controls include counted
views, explicit NUL restrictions only for C-string outputs and uint8 decoding.
Any API/ownership/charset/compiler/target/library change requires revalidation.
Resolve the proposed disposition before corresponding production acceptance.

HPS-C-017/018 rationale: existing descriptor generators, text validation and
builder copy engine are reused. JSON uses one string/Unicode scanner for lexing
and decoding and one document grammar scan for read/write preflight. Lexical
and structural validity remain separate contracts. Multiple scans intentionally
trade work for preserving caller outputs, without hidden payload storage.
Vector/dlist specializations keep array bounds and intrusive-node ownership
visible; they do not erase types behind generic container casts. No new shared
error/status registry, allocation model or general macro language is added.
String decoding retains three bounded scans: shared whole-token validation,
decoded-size preflight and emission. The shared scalar-token validator also
serves integer access and rejects leading/trailing text; keeping that common
integrity contract costs one extra scan rather than hidden output-sized scratch.
JSON integer accumulation specializes the counted JSON-token spelling/error
contract rather than copying into a temporary NUL buffer for the Cstr parser.
Integer spelling is preflighted once before range accumulation so decimal and
exponent rejection takes priority over an oversized integer prefix.

Applicable IDs: HPS-HDR-001 through 006; HPS-C-001 through 019;
HPS-MEM-001/002/005; HPS-EXEC-001/003; HPS-SEC-001/002/003/005/006/007/009;
HPS-GOV-004/005/006/008/009; HPS-PRO-001/002/003; HPS-VER-001 through 006;
HPS-REL-001/003/004. Existing numerical/enum/test/character-storage and optional
allocator findings are neither approved nor suppressed.

Relevant MISRA rows: Directives 4.1, 4.6 through 4.12, 4.14; Rules 1.1, 1.3,
5.1, 5.4, 5.6, 8.2, 8.3, 8.8, 8.10, 8.13, 9.1, 10.1, 10.3, 10.4,
11.3, 11.5, 11.8, 12.2, 13.1, 13.2, 13.5, 17.2, 17.7, 18.1, 18.2,
18.4, 18.6, 20.1, 20.7, 20.10, 20.11, 20.12, 20.14, 21.3, 21.15,
21.16 and 22.1. Matrix statuses/deviations remain unchanged. Links below retain
the earlier evidence; licensed rule/essential-type/target analysis remains open.

## Current verification

Host: x86_64 Linux; GCC/G++ 11.4.0, Clang/Clang++ 14.0.0, Python 3.10.12,
CMake 3.22.1 and Meson 0.61.2. Evidence covers the source base above plus this
working tree. C99 tests retain assertions through `-UNDEBUG`; new suites expose
direct `int32_t` entry points and are registered in every manifest/shared runner.

| Executed check | Configuration and result |
| --- | --- |
| Strict Make | `make test BUILD_DIR=build/objects-json-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`: 24 shared suites, 357 portability, 105 detection, 15 annotation and 942 JSON oracle/mutation cases PASS. Current rerun includes quote/separator correction and JSON extent guards. |
| CMake Release/hosted | Configure `build/objects-json-cmake` with `-DCMAKE_BUILD_TYPE=Release -DHPS_BUILD_HOSTED_DEBUG=ON -DCMAKE_C_FLAGS='-Wall -Wextra -Wpedantic -Werror'`, build `--parallel 4`, `ctest --test-dir build/objects-json-cmake --output-on-failure`: 6/6 PASS, including 25 shared suites, four Python checks and hosted subprocess contracts. |
| Meson/Clang ASan+UBSan | `CC=clang meson setup build/objects-json-meson -Dwerror=true -Dhosted_debug=true -Db_sanitize=address,undefined -Db_lundef=false`, compile and `ASAN_OPTIONS=detect_leaks=0 meson test -C build/objects-json-meson --print-errorlogs`: 6/6 PASS. All 25 shared suites are instrumented; ordinary Python-generated compile checks use their explicit recorded flags. LeakSanitizer cannot run under environment ptrace and is disabled; no leak-check result is claimed. |
| Instrumented JSON oracle | `ASAN_OPTIONS=detect_leaks=0 python3 tests/json/check_json.py --compiler clang --sanitize`: all 942 deterministic cases PASS with the library/harness built under ASan+UBSan. A bounded length-prefixed batch transport replaces per-case processes; initial unbatched instrumented execution was stopped for excessive host-process overhead. Default oracle rechecks through CMake/Meson also PASS after this transport-only change. |
| Independent C++ public headers | 71 public headers independently/repeatedly included under G++ and Clang++ C++11/17 with `-Wall -Wextra -Wpedantic -Werror`: 284 strict checks PASS. Synthetic C detection checks do not qualify foreign ABIs. |
| Installed mixed C/C++ consumer | `cmake --install build/objects-json-cmake --prefix "$PWD/build/objects-json-install"`, separate `find_package(HPS CONFIG REQUIRED)` C++17 consumer links `HPS::json` and `HPS::memory` and runs PASS. Four direct G++/Clang++ C++11/17 link/runs against the freshly installed C archives also PASS, exercising JSON read/i64/compact, typed vector iteration, refcounts, indentation and conflicting quote delimiters. Consumers live under `build/objects-json-consumer/`. |
| Dependency isolation | `nm -u` on all six core archives finds no direct libc heap, optional allocator-hook or hosted diagnostic dependency. This is direct-symbol evidence, not transitive whole-product/libc no-heap qualification. |
| Policy checker | `python3 tools/check_policy.py --self-test`: 14 scope, 16 lexical, eight header and six invalid-governance fixture cases PASS. Repository check remains exit 1 with exactly the same 153 prior findings, byte-for-byte compared with the pre-change output. No new finding or suppression. |
| Provenance/traceability | All twenty original/snapshot hashes match the existing inventory; non-evidence fields of all 221 MISRA rows are unchanged. Reference files remain untouched. `git diff --check` PASS; staged whitespace/integrity checks precede commit. |

The deterministic JSON oracle uses CPython JSON grammar after rejecting native
NaN/Infinity constants and unpaired surrogates to match the documented Unicode
subset. Seed 8259 generates nested documents, four mutation classes and explicit
lexical/UTF-8 cases. It checks validity and parsed-value preservation after compact
writing; duplicate-member/order/spelling preservation is separately asserted in
the C suite. This is bounded regression/fuzz evidence, not exhaustive fuzzing or
proof of equivalence for every possible input. Harness storage is 1024 tokens,
64 frames and two typed 1025-character buffers plus decoded byte storage; it is
host-test transport, not library resource allocation or target stack evidence.

Independent agent reviews checked collection pointer-typedef constness and list
budgets, indentation preflight/aliasing, JSON grammar transitions/UTF-8/arithmetic
and two-pass failure contracts. They found quote/escape delimiter skipping ignored
syntax precedence; the implementation now consistently gives enabled syntax
precedence, with regression cases. Clang initially rejected unused generated
fixture helpers and adjacent BOM literal spelling; useful helper assertions and
an unambiguous literal corrected both without disabling warnings. Regression
tests cover huge decimal/exponent integer syntax priority, representable array
budgets, complete decoded-output preservation and UTF-8 boundary scalars.

Remaining gaps: named role acceptance and proposed counted-character disposition,
licensed MISRA/essential-type/whole-program analysis, broader adversarial/fuzz
campaigns, selected target charset/ABI/libc, stack/linker/resource/timing evidence,
application schema/duplicate-key/rate/ownership policies and vulnerability/license
release decisions. Host/sanitizer results do not close these gates or establish
MISRA compliance, industrial qualification, measured performance or WCET.

## Retained earlier matrix evidence

| Earlier evidence | Rows |
| --- | --- |
| [utilities-runtime-2026-10-19.md](utilities-runtime-2026-10-19.md) | Dir 4.1, Dir 4.6, Dir 4.7, Dir 4.8, Dir 4.9, Dir 4.10, Dir 4.11, Dir 4.12, Dir 4.14, Rule 1.1, Rule 1.3, Rule 5.1, Rule 5.4, Rule 5.6, Rule 8.2, Rule 8.3, Rule 8.8, Rule 8.10, Rule 8.13, Rule 9.1, Rule 10.1, Rule 10.3, Rule 10.4, Rule 11.3, Rule 11.5, Rule 11.8, Rule 12.2, Rule 13.1, Rule 13.2, Rule 13.5, Rule 17.2, Rule 17.7, Rule 18.1, Rule 18.2, Rule 18.4, Rule 18.6, Rule 20.1, Rule 20.7, Rule 20.10, Rule 20.11, Rule 20.12, Rule 20.14, Rule 21.3, Rule 21.15, Rule 21.16, Rule 22.1 |
