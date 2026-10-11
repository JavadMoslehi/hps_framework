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
- The static pool is single-context. Do not introduce pthread, atomics, locks or a
  thread-safe claim unless the task explicitly requests a separate reviewed layer.
- Map changes to policy IDs and relevant MISRA rows. Keep unverified results,
  pending deviations and unsupported targets visible. Do not invent approvals.
- Run python3 tools/check_policy.py for relevant changes. It performs no C build.
  Run compiled tests/builds only when consistent with the user's instructions;
  if deferred, report that and do not reuse historical evidence as current evidence.
- Never claim MISRA compliance, industrial qualification, PCI/ISO certification,
  proven WCET or measured performance without the scoped evidence required in
  governance/verification.md.
- Review any applicable policy exception using governance/charter.md. A proposed
  deviation is not an approved one. Prepare concrete reviewable work within the
  user's authorized scope; do not introduce unrelated approval requests.
