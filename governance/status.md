# Current adoption status

Baseline date: 2026-10-11. This is a gap register, not a progress estimate.
No qualified production target, approved external-standard compliance summary,
industrial certification or release evidence bundle is currently established.

| Gap ID | Observed condition | Acceptance action | Status |
| --- | --- | --- | --- |
| GAP-001 | Static byte-block pointer pool replaced the handle/index API; replacement tests have not run | Build/test/analyze the exact revision when requested, then retain scoped results | Open |
| GAP-002 | MISRA matrix has complete baseline identifiers but no implementation dispositions | Assess all 221 entries with authoritative rule interpretation and evidence | Open |
| GAP-003 | Runtime pool reservation may need Directive 4.12 deviation | Approve scoped disposition or select static-object lifetime allocation | Open |
| GAP-004 | Compiler annotations and `restrict` now require explicit opt-in; assertion fallback and dialect-dependent features remain unassessed | Analyze actual enabled configurations and approved exceptions | Open |
| GAP-005 | No independently reviewed target ABI/timing/stack/DMA record | Qualify chosen platform/profile; record dependencies and measurements | Open |
| GAP-006 | Pointer pool is single-context; no erase, stale-alias-after-reuse detection or arbitrary RAM-fault containment | Restrict use and assess application ownership, secrets and fault containment | Open |
| GAP-007 | Hosting review/branch-protection/CI settings are not established by repository files | Configure actual hosting enforcement and retain its record | Open |
| GAP-008 | Public release license/security contact/support policy not selected in this task | Select and publish them before a supported distribution | Open |
| GAP-009 | Payment, telecom and automotive product standards/scope unselected | Integration owner establishes profile-specific assessments and responsibilities | Open |
| GAP-011 | Public headers have guarded C linkage; mixed C/C++ include/link behavior is untested | Qualify the chosen C++ consumer and C library toolchain/ABI | Open |
| GAP-010 | Lightweight checks cover only syntax-level restrictions and document consistency | Add qualified semantic/whole-program tools and independent review for release | Open |

Historical host tests from the earlier allocator are not replacement-pool evidence.
No percentage-compliant or "production ready" claim may be derived from matrix
completeness or the existence of this governance folder. Profile obligations are
planned release gates; unimplemented future features are not implied by their text.
