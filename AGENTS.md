# HPS contributor instructions for coding agents

Read [governance/README.md](governance/README.md) and the relevant governance
requirements before editing production code. This policy applies throughout the
repository. User task constraints on execution and scope remain authoritative.

- Numerical declarations use exact-width stdint types or size_t/ptrdiff_t; no basic
  numerical types or least/fast/max variants. Plain char pointers are for C strings.
  The documented hosted int main(void) entry point is the only current basic-type
  declaration exception.
- Public headers follow governance/headers.md: path-derived guards, @file/@brief,
  explicit dependencies and guarded extern "C" for external C APIs/callback types.
  Keep includes outside linkage blocks; macro-only headers need no empty block.
- Preserve static resource budgets, checked arithmetic, typed storage and explicit
  ownership/error contracts. Core code has no heap or OS dependency.
- Keep names, validation, mutation order and purpose comments easy to review.
  Factor avoidable duplicate logic only when its contracts and resource bounds
  match; record justified specialization/repetition under HPS-C-017/HPS-C-018.
- APIs use ordinary types suited to their result. Error-returning module APIs use
  int32_t directly from stdint.h, with zero success and module-local negative typed
  constants; flags remain uint8_t. No shared status typedef, module-ID registry or
  diagnostic lookup dependency. Follow HPS-C-004/HPS-C-019 and document API/ABI
  and numeric-code changes.
- The static pool is single-context. Do not introduce pthread, atomics, locks or a
  thread-safe claim unless the task explicitly requests a separate reviewed layer.
- Map changes to policy IDs and relevant MISRA rows. Keep unverified results,
  pending deviations and unsupported targets visible. Do not invent approvals.
- Run python3 tools/check_policy.py for relevant changes. It performs no C build.
  Run compiled tests/builds only when consistent with the user's instructions;
  if deferred, report that and do not reuse historical evidence as current evidence.
- Module test files expose a callable entry point matching their filename stem,
  such as `test_bytes(void)` in `test_bytes.c`; only `tests/main.c` owns
  the shared hosted runner's `main`. Register new suites in `tests/test_suites.h`
  and all build manifests. The user-requested `int` suite signatures are a proposed
  test-only exception recorded in `governance/reviews/test-entrypoints-2026-10-11.md`,
  not an approved extension of the numerical type policy.
- Never claim MISRA compliance, industrial qualification, PCI/ISO certification,
  proven WCET or measured performance without the scoped evidence required in
  governance/verification.md.
- Review any applicable policy exception using governance/charter.md. A proposed
  deviation is not an approved one. Prepare concrete reviewable work within the
  user's authorized scope; do not introduce unrelated approval requests.

## Legacy framework migration

Read [the legacy source inventory](docs/migration/legacy-sources.md) when reusing
the user's previous frameworks. Both reference trees are strictly read-only;
all edits and generated artifacts belong in HPS. Preserve imported API behavior
and document proposed type/behavior changes explicitly instead of silently
rewriting them during naming or placement work. Update the migration record so
another session or machine can continue from project files.
