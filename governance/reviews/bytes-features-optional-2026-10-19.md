# Bytes, compile features and typed optional migration — 2026-10-19

Status: user-authorized development adaptation. Author: Codex; named maintainer
and independent project reviewer unassigned. Target qualification, existing
character-storage disposition and release acceptance remain pending. Agent
source audits are inputs, not assigned role approvals. Source base:
`5d6c51e9671e74eba8ff3e4af6c21c0467a8df21` plus this change. Profile: hosted,
single-context development; no industrial/embedded target is qualified.

## Scope, provenance and compatibility

The user requests the same complete reading, HPS naming/layer decomposition,
deduplication, correction, verification and commit/push for twenty-three selected
byte-buffer, features, optional and type headers. Their live paths were
unavailable during full reading; exact members of the read-only original ZIP
were inspected instead. All selected byte sizes/SHA-256 values and the archive
SHA-256 match the existing inventory. No archive extraction, reference edit,
build or execution occurred. The
[lossless 38,003-byte snapshot](../../docs/migration/bytes-features-optional-snapshot.json)
and [per-file/symbol mapping](../../docs/migration/bytes-features-optional.md)
preserve provenance and deliberate API/type/behavior changes for continuation.

Most concepts already exist. Bytes retains its current descriptors, storage,
reader, assignment/equality and integer-codec providers, extending existing
buffer/view APIs only with pop, ordering, edges and subsequence search. Base and
Platform remain the sole detection providers; five compiler-target x86 feature
flags extend the existing architecture feature header. Raw/expanded literal
stringification lives in existing Base macros. Typed optional values belong in
`data_structure/optional.h`, independently selectable and included by `all.h`.
They reuse the existing typed initialization provider. Boolean text conversion
extends existing Cstr conversion and reuses String whitespace and Cstr comparison.
No new library, heap/OS/pool/provider-hook dependency or global status registry.

The source's implementation/algo and older optional headers are entirely
commented; declarations/drafts are not working runtime capability. Unchecked
truncation, overlaps, length arithmetic, long indexes, unsigned-wrap loops/hash,
native char flags, incompatible anonymous structs, dialect-dependent void access,
inconsistent ref wrappers and wrong result_err slot are not silently retained.
Guessed standard integer/pointer typedefs, native bool aliases, mutable literal
casts, memset typed initialization, qsort/printf/callback algorithms and universal
null/result frameworks are not imported. This is not namespace-only source or
binary compatibility; no existing HPS error numeric value/layout changes.

## Bounds, storage, ownership and failures

Bytes comparison produces canonical -1/0/1 using unsigned-byte lexical order.
Equal, compare, edges and search share one validated-range byte engine. Edge
patterns can be empty and match; empty needles never match, preserving the draft
search meaning. Search success writes a size_t index or SIZE_MAX/NPOS on absence.
No legal index equals NPOS, even for a caller-proven SIZE_MAX-element view.
Nonempty needle proves candidate count `length - needle.length + 1` representable;
reverse traversal decrements only positive remaining counts. Matched offsets
prove each `offset + index` before indexing. Inputs may overlap read-only; result
metadata must be disjoint from referenced payloads and correctly typed/live.
Malformed input/NULL output preserves outputs. Bounds cannot validate arbitrary
pointers or forged extents/lifetimes.

Pop validates required output and initialized buffer before emptiness, reads the
last byte, then decreases length. Empty is OUT_OF_RANGE; invalid state and all
errors preserve descriptor/payload/output. Success does not erase or release
storage and invalidates prior borrowed views. Existing assignment already covers
export and overlapping copies, avoiding another wrapper. Pop has O(1) work;
comparison is O(minimum length), edges O(pattern length) and search at worst
O(view length times needle length). Automatic state is scalar; no payload scratch.

Optional stores one actual allowed-policy, assignable/copyable element plus uint8
presence and native padding. None uses typed aggregate initialization, including
valid null pointers. Some(NULL) remains present. Checked operations accept only
tags zero/one; NULL required arguments precede invalid tag, then get may return
EMPTY. Has/get/get_or require disjoint live outputs; all errors preserve state
and output. Set-none retains initialized payload; set-some copies before presence.
No ownership/cleanup/retention/erasure is implied. Eager fallback and unspecified
ordinary argument ordering replace repeated-evaluation/lazy macros explicitly.
Checking/flag operations are O(1); fixed-type value copies and returned wrapper
temporaries cost their actual type sizes. Caller budgets large values and stack.

Cstr boolean parse validates bounded NUL termination before skipping a leading
run of the six basic ASCII whitespace characters. The remaining entire suffix must be true/yes/on/1 under
existing basic-Latin case comparison to produce one; all other terminated text
successfully produces zero. No trailing trim, locale/POSIX, unbounded scan or
strict-grammar claim. Required argument/zero capacity and missing NUL fail without
output mutation. The bounded suffix is at most four bytes before keyword compare;
its capacity addition is therefore proven. Work is O(input capacity), scalar/view
automatic storage only. Raw and expanded stringification evaluate no runtime
expression and produce immutable literals.

ISA flags require positive individual predefined-macro evidence and an existing
x86-family identity. They are per translation unit/options, zero on other
families, and do not infer AVX from AVX2 without compiler evidence. No CPUID,
runtime hardware/OS register-state support, instruction permission, runtime
dispatch or cryptographic implementation is provided. Native -m option checks
compile declarations only; no such instructions are executed. Primary reference:
[GCC x86 options](https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html).

All new functionality is caller-owned/serialized, without locks, recursion,
hidden allocation, callbacks, mutable global state or external I/O. Compile
feature metadata is not target qualification. Input extents, lifetime, caller
size/rate/deadline limits and application ownership remain integration duties.

## Policy and MISRA traceability

Threat boundaries include malformed/oversized descriptors, binary NUL/high-bit
bytes, full/empty buffers, absent/corrupt optional tags, invalid C-string extents
and misleading compile-target evidence. Controls are checked arithmetic before
mutation, typed storage, finite caller extents, explicit no-match/empty/error
domains and positive target evidence. No arbitrary-pointer recovery, sensitive
erasure, constant-time comparison or shared-context guarantee is made.

HPS-C-017/018 rationale: one byte range engine serves equality/order/edge/search;
search directions share validation and candidate accounting; pop reuses buffer
validation. Existing reader, typed storage and export composition are reused.
Named optional functions replace repeated anonymous expressions without an
untyped macro language. None reuses language-correct aggregate initialization.
Boolean conversion reuses String/Cstr validation/classification/comparison rather
than introducing another locale or case engine. Per-ISA flag branches retain
separate independent evidence, rather than infer unrelated feature dependencies.

Applicable IDs: HPS-HDR-001 through 006; HPS-C-001 through 019;
HPS-MEM-001/002/005; HPS-EXEC-001/003; HPS-SEC-001/002/003/005/007/009;
HPS-GOV-004/005/006/008/009; HPS-PRO-001/002/003; HPS-VER-001 through 006;
HPS-REL-001/003/004. Existing counted-character/C-string fixtures continue under
the [proposed scoped storage disposition](string-storage-2026-10-13.md), not an
approved exception. This change adds no ordinary char binary array or native
numerical alias and does not approve prior enum/test/allocator findings.

Relevant MISRA rows: Directives 4.1, 4.6 through 4.12, 4.14; Rules 1.1, 1.3,
5.1, 5.4, 5.6, 8.2, 8.3, 8.8, 8.10, 8.13, 9.1, 10.1, 10.3, 10.4,
11.3, 11.5, 11.8, 12.2, 13.1, 13.2, 13.5, 13.6, 17.2, 17.7, 18.1,
18.2, 18.4, 18.6, 20.1, 20.7, 20.10, 20.11, 20.12, 20.14, 21.3,
21.15, 21.16 and 22.1. Matrix statuses/deviations stay unchanged; earlier evidence
links are retained below. Licensed rule/essential-type/target analysis is open.

## Current verification

Host: x86_64 Linux; GCC/G++ 11.4.0, Clang/Clang++ 14.0.0, Python 3.10.12,
CMake 3.22.1 and Meson 0.61.2. Evidence is for the source base above plus this
working tree, with enabled test assertions through `-UNDEBUG`. Three new suites
expose filename-matching `int32_t` entry points registered in all manifests and
the shared runner. No historical migration result is reused as current evidence.

| Executed check | Actual configuration/result |
| --- | --- |
| Strict default Make | `make test BUILD_DIR=build/bytes-features-optional-make CFLAGS='-std=c99 -Wall -Wextra -Wpedantic -Werror'`: 27 shared suites, 361 portability, 157 detection, 15 annotation and 942 JSON oracle/mutation checks PASS. The final added long-leading-whitespace fixture was rebuilt and the shared runner again PASS. |
| CMake Release/hosted | Configure `build/bytes-features-optional-cmake` with `-DCMAKE_BUILD_TYPE=Release -DHPS_BUILD_HOSTED_DEBUG=ON -DCMAKE_C_FLAGS='-Wall -Wextra -Wpedantic -Werror'`, build `--parallel 4`, then `ctest --test-dir build/bytes-features-optional-cmake --output-on-failure`: 6/6 PASS, including 28 suites and hosted subprocess contracts. After the final fixture addition, affected rebuild/shared-runner recheck PASS. |
| Clang ASan+UBSan/Meson | `CC=clang meson setup build/bytes-features-optional-meson -Dwerror=true -Dhosted_debug=true -Db_sanitize=address,undefined -Db_lundef=false`, compile, then final `ASAN_OPTIONS=detect_leaks=0:verbosity=1 meson test -C build/bytes-features-optional-meson --print-errorlogs`: 6/6 PASS. All 28 shared suites are instrumented; Python-generated checks use their explicit ordinary compiler flags. LeakSanitizer remains unavailable under ptrace and is disabled; no leak-check result. Earlier instrumented process timeouts are retained below. |
| Public C++ headers | All 72 public headers independently/repeatedly included with G++/Clang++ C++11/17 and `-Wall -Wextra -Wpedantic -Werror`: 288 checks PASS. |
| New generated C++ values/mixed C | The three new suites plus macro-utility suite compiled as C++11/17 under both compilers, linked to fresh C99 Bytes/Cstr/String archives and ran PASS (four configurations). This exercises scalar, structured and const-pointer optional payloads and every generated helper, plus byte algorithms and boolean conversion. |
| Installed package | Install via `cmake --install build/bytes-features-optional-cmake --prefix "$PWD/build/bytes-features-optional-install"`. Separate C++17 `find_package(HPS CONFIG REQUIRED)` consumer links `HPS::bytes` and `HPS::cstr` and runs PASS. Four G++/Clang++ C++11/17 direct link/runs against installed headers/fresh C archives also PASS, including optional, byte algorithms/pop, truthy parse and stringification. Consumer sources remain under `build/bytes-features-optional-consumer/`. |
| Policy/checker behavior | `python3 tools/check_policy.py --self-test`: 14 scope, 16 lexical, eight header and six invalid-governance fixtures PASS. Repository check remains exit 1 with the same 153 prior findings. Path/rule/token multisets are unchanged after allowing source-line offsets from added includes; no new finding or suppression. |
| Integrity/traceability | All 23 decoded snapshot byte counts/hashes and ZIP members match the inventory; archive SHA-256 unchanged. Non-evidence fields of all 221 MISRA rows are preserved. `git diff --check` PASS; staged checks precede commit. |

The independent byte oracle covers all 945 small binary haystack/needle pairs
over a two-byte alphabet, using a forward exhaustive match accumulator and libc
ordering model independently of production traversal. Additional cases exercise
NUL/high-bit bytes, overlapping read-only inputs, too-long/empty patterns,
metadata-only SIZE_MAX arithmetic without payload access, corrupt buffers,
no-erasure pop and overlapping export composition. Optional tests cover empty/
present-null, scalar/structured/const-pointer copies, tag corruption, NULL-first
priority, retained payload, eager fallback and single argument evaluation.
Truthy tests cover spelling/case, all leading whitespace classes, long runs,
trailing/unknown false results, high-bit input, first-NUL behavior and bounded
failure preservation. Stringification distinguishes expanded/raw tokens and
confirms operand side effects do not run.

Detection adds 38 synthetic C/C++ instruction-macro/family cases and 14 native
declaration-only compiler-option checks on this x86 host (105 to 157 total).
Non-x86 native compilers explicitly skip the x86-only option checks, retaining
synthetic family gating. No synthetic-target binary or enabled ISA instruction
is run; these checks do not qualify foreign ABI/hardware or runtime OS support.

One initial Meson ASan/UBSan runner exceeded its 30-second limit without a
sanitizer diagnostic; standalone invocations also intermittently stalled. The
unchanged instrumented binary subsequently passed in 0.02 seconds, and a temporary
tracing runner using the same instrumented suite objects/C archives completed all
28 entries. A verbose ASan startup run and the final six-check Meson run passed.
The stall's cause is undetermined: it is not dismissed as a demonstrated code
false positive or proven environment fault. Obsolete stalled processes were
stopped. These host-tool observations remain a limitation; no stable sanitizer
runtime or absence-of-defects claim is inferred from successful reruns.

Manual implementation review checked tag initialization/ownership, byte bounds,
shared helper preconditions, reverse-loop termination, truthy suffix extents and
target-only ISA meaning. Source audits were partitioned across agents; named
independent project acceptance is not supplied by those audits or passing tests.
Remaining gates include licensed semantic/MISRA analysis, counted-character
disposition, named review, target/compiler/ABI/charset/libc and stack/resource/
timing evidence, broader fuzzing, application ownership/deadline controls and
license/security release decisions. No MISRA compliance, industrial qualification,
measured performance or WCET is claimed.

## Retained earlier matrix evidence

| Earlier evidence | Rows |
| --- | --- |
| [objects-json-2026-10-19.md](objects-json-2026-10-19.md) | Dir 4.1, Dir 4.6, Dir 4.7, Dir 4.8, Dir 4.9, Dir 4.10, Dir 4.11, Dir 4.12, Dir 4.14, Rule 1.1, Rule 1.3, Rule 5.1, Rule 5.4, Rule 5.6, Rule 8.2, Rule 8.3, Rule 8.8, Rule 8.10, Rule 8.13, Rule 9.1, Rule 10.1, Rule 10.3, Rule 10.4, Rule 11.3, Rule 11.5, Rule 11.8, Rule 12.2, Rule 13.1, Rule 13.2, Rule 13.5, Rule 17.2, Rule 17.7, Rule 18.1, Rule 18.2, Rule 18.4, Rule 18.6, Rule 20.1, Rule 20.7, Rule 20.10, Rule 20.11, Rule 20.12, Rule 20.14, Rule 21.3, Rule 21.15, Rule 21.16, Rule 22.1 |
| [utilities-runtime-2026-10-19.md](utilities-runtime-2026-10-19.md) | Rule 13.6 |
