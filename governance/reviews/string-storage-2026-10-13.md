# Counted text and test storage proposal — 2026-10-13

Status: proposed project-policy exception; NOT approved for a qualified release.
Author/owner: Codex. Named independent reviewer and approving project roles are
not assigned. Policy baseline: 0.2.0. Source base: 2dafeb9 plus the string working
tree. The user authorized completing the chosen string layer and running its
tests; no external-standard deviation or certification approval is inferred.

## Exact scope and rationale

HPS-C-003 currently allows plain char only for C-string pointer interfaces.
The selected string layer borrows counted text through const char pointers,
including data without a trailing NUL and embedded NUL. Its test fixtures use
actual bounded char arrays for writable string buffers, unterminated input bounds,
and overlap/failure checks. The lexical checker reports these arrays. The initial scope covered the foundation headers/sources and its string suite.
The subsequent user-authorized migration extends the scope to include/hps/string/,
include/hps/cstr/, src/string/, src/cstr/, tests/string/, tests/cstr/ and the bounded
character fixtures in tests/enum/test_cstr_enum.c and
tests/validation/test_national_code.c. This includes real temporary/fixed text
arrays and explicitly range-proved ASCII-to-char casts. It does not permit scalar
char numerical declarations, character arithmetic, unrelated object casts,
unrelated test directories or binary payloads disguised as text storage.

Actual typed character arrays model the API's backing objects and give meaningful
bounds/sanitizer evidence. Casting uint8_t arrays to character pointers would
bypass the policy and obscure storage; allocation or a vendor fixture would add
unnecessary ownership/dependencies. String literals cannot supply mutable storage.
No checker allowlist or warning suppression is introduced to hide these findings.

Counted views access only their checked declared extents; bounded C-string import
requires NUL within the supplied capacity. Fixed buffers reserve NUL inside their
capacity and reject embedded NUL before writes. Copies prove capacity and use
memmove for overlap. All failure paths preserve outputs; callers still prove actual
extent, lifetime, initialization and metadata/payload disjointness. Standard-library
dependencies, essential types, target encoding/ABI, stack and resource budgets
remain target-review work. Text is not treated as Unicode or a secret comparison.

## Decision and revalidation

This record permits preparing concrete task-authorized development work; it is
not project-role acceptance or a MISRA deviation. Release acceptance remains
pending disposition under HPS-GOV-006/007/008. Assess relevant Directives 4.6/4.14
and Rules 10.1/10.3/18.1 in the complete licensed standard and target scope; no
mandatory guideline deviation is requested. Preserve existing unassessed matrix
dispositions.

Revalidate on text representation, storage/capacity contract, scope, compiler,
target, library or policy changes. Resolve this proposal before any qualified
release containing the text layer. Remove it only after a scoped policy amendment
or approved applicable disposition, not a passing test. Initial checks remain a historical foundation snapshot. Current technical checks
and final snapshots are in [the CSEP text review](csep-text-2026-10-13.md).
