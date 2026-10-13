# Preserve the imported enum API during HPS migration — 2026-10-13

Status: development-only; compiled behavior, C89 compatibility and C++ linkage
unverified. Author: Codex. Named maintainer/reviewer acceptance remains pending;
no external-standard deviation or qualification is approved here.
Source base: a216dba1aa5543166cbda92c353a22c377e8ad7b plus the user's working tree
and supplied CSEP attachment. Authorization: rename and place the original file;
the user's subsequent instruction explicitly requires restoring its enum type
definitions and behavior instead of the preceding int32_t/bounded rewrite.
Profile: development with caller-owned storage. The recorded no-build constraint
remains active. User authorization governs this task scope; it does not establish
MISRA acceptance or change the repository's general numerical policy.

## Final migration and dependencies

include/hps/enum/traits.h belongs beside bytes and memory because its functions
perform runtime lookup, iteration and formatting. It stays outside hps/base.h.
CSEP_ENUM_* becomes HPS_ENUM_* and csep_enum_* becomes hps_enum_*; all 34 generated
methods, native enum_type usage, bool/int/char signatures, original tables and
declaration/implementation/inline generation remain unchanged.

The original csep_features.h, csep_cstr.h and csep_cstl.h are absent from HPS.
Only csep_static_inline is referenced from those dependencies. The string/container
headers supply no directly used identifier in this file. HPS_ENUM_STATIC_INLINE
uses the fallback already present, commented out, in the supplied source: C++/C99
inline, MSVC/GCC legacy inline extensions, otherwise static. Callers can provide
the macro before inclusion. This is a local compatibility fallback, not evidence
that the complete old features package has been ported; that file is still needed
to compare any additional old target-specific behavior.

The explicit stddef/stdint/stdbool/string includes are retained. C89-style bodies
are preserved, but C89 targets must supply compatible exact-width/Boolean headers
and any selected compiler extension. No strict-C89 or cross-language qualification
is inferred from syntax. External generated declarations/callbacks need the
caller's guarded C linkage at their expansion sites, as in the original design.
Separate IMPLEMENT use requires prior DECLARE. See the
[original API contract and example](../../docs/architecture/enum-traits.md).

## Policy/MISRA scope and retained findings

- HPS-HDR-001/002/003, HPS-C-013/016, HPS-VER-001/003 and HPS-REL-003: path-derived
  guard, metadata, explicit real dependencies and namespace migration. Header
  structural checks pass; actual compiler/ABI behavior remains unverified.
- HPS-C-001/003/004/005/008, HPS-HDR-004/005/006: native enum, bool, int and scalar
  char APIs are deliberately retained under the user's specific scope instruction.
  They conflict with the general type policy. No registry/status ABI change or
  blanket policy exception is introduced; checker findings remain visible.
- HPS-C-006/007/009/011/012/014, HPS-SEC-001/002/003/009 and HPS-EXEC-001 through 003:
  original unbounded NUL scans, unchecked length sums/conversions/index distances,
  zero pointer constants, partial formatting and copy/append failure mutations are
  preserved. Callers provide valid terminated storage, extents and nonoverlap.
  The original preconditions and unresolved bounds/essential-type assessments are
  not replaced with claims of checked behavior. No measured timing/resource claim.
- HPS-C-015/017/018, HPS-MEM-001/002 and HPS-GOV-004/005/008: repeated function-local
  tables and token-pasting/stringification remain to preserve the supplied design,
  as explicitly requested. Names arrays retain mutable pointers to immutable
  strings; no table writes are performed by these methods. ROM/code duplication,
  extension selection and callback work require scoped target review. No heap or
  OS call is introduced. Preservation is not an approved standards deviation.

Relevant MISRA rows remain unassessed: Directives 4.1, 4.6, 4.9, 4.10, 4.11, 4.14
and Rules 1.3, 5.1, 5.8, 8.2, 8.3, 8.4, 8.5, 8.6, 8.13, 10.1, 10.3, 10.4, 10.8,
11.9, 14.2, 14.4, 15.5, 17.1, 17.7, 18.1, 18.4, 20.1, 20.10, 20.11 and 20.12.
Matrix evidence links remain; there is no satisfied/approved disposition.

## Current source-only verification

Python 3.10.12 and the unchanged tools/check_policy.py were used. The checker
returned 1 with 76 lexical findings: 63 in the restored header, five in its native
enum/bool tests, one in its native enum documentation example and seven existing
findings elsewhere (the endian enum and six int suite signatures). There is no
passing whole-repository policy result. These retained findings match the user's
instruction; no numerical API is silently rewritten to make the checker pass.

Compared the complete helper and generator source region with the supplied
attachment after only namespace substitution and ignoring whitespace: exact match.
Thus return/parameter types, function bodies, table declarations and macro modes
retain the original implementation. Direct whitespace checks and git diff --check
pass. All 34 generated method suffixes are retained; the shared int32_t test_enum
entry point and Make/CMake/Meson registration remain in place. Adapted prepared
tests use native enum/bool, expect index_of failure to write NPOS and omit malformed
unterminated strings that the original scanner cannot safely accept.

No C configure/build, executable test, compiler probe, sanitizer, semantic analyzer
or C/C++ link test is run. A second agent reviewed the restored API documentation
and prepared tests; this is author assistance rather than named project-role
approval. Writable string-buffer/legacy arithmetic behavior, target budgets and
C89 compatibility remain unverified.

## Checked source snapshot

| File | SHA-256 |
| --- | --- |
| Original CSEP attachment | `d4155b4e830adf611383f5c2dbb0d2f619bcc1d8973ac446848f88badcd46870` |
| include/hps/enum/traits.h | `ae4599ab775997aaafe6d06dbc41c1db11969961f228d8bd7280e02d5d7ae639` |
| tests/enum/test_enum.c | `753f361896984522234e643039c324caff10e7c296ee430dce5f2016da28ad20` |
| docs/architecture/enum-traits.md | `bf014ba6036abb647a5cdf3b5997b1220eb9260e2761cf282770364cce84a5ac` |
