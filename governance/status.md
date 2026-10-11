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
| GAP-012 | Bytes view/buffer APIs and overlap/boundary tests are prepared but unbuilt; target memmove is unqualified | Run compiled tests/analyzers and qualify the target copy implementation, ABI, stack and timing | Open |
| GAP-013 | User-requested int module test entry points exceed HPS-C-005; shared runner/build changes are unbuilt | Review the [proposed test-only exception](reviews/test-entrypoints-2026-10-11.md) and verify the updated runner when builds are authorized | Open |
| GAP-014 | Endian integer reads/appends and prepared byte fixtures are unbuilt; 64-bit arithmetic helpers/stack/ABI remain target assumptions | Verify the [endian change](reviews/bytes-endian-2026-10-11.md), essential types and actual target dependencies when builds are authorized | Open |
| GAP-015 | Sequential reader and cursor/failure tests are prepared but unbuilt; target ABI, stack and timing remain unverified | Verify the [reader change](reviews/bytes-reader-2026-10-11.md) with compiled tests/analyzers/fuzzing and target evidence when authorized | Open |
| GAP-016 | Ecosystem directories are placeholders; industrial/medical/defence overlays and concrete target contracts for all eight ecosystems are unset | Select product scope, system safety/security requirements, standards and target profiles before adoption | Open |
| GAP-017 | Five bytes API symbols were renamed; updated consumer compilation/linkage remains unverified | Apply the [name migration](reviews/bytes-api-names-2026-10-11.md) and rebuild suites/consumers when builds are authorized | Open |
| GAP-010 | Lightweight checks cover only syntax-level restrictions and document consistency | Add qualified semantic/whole-program tools and independent review for release | Open |

Historical host tests from the earlier allocator are not replacement-pool evidence.
No percentage-compliant or "production ready" claim may be derived from matrix
completeness or the existence of this governance folder. Profile obligations are
planned release gates; unimplemented future features are not implied by their text.
