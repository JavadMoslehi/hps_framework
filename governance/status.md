# Current adoption status

Baseline date: 2026-10-11; host evidence updated 2026-10-13. This is a gap
register, not a progress estimate. Current host verification is recorded in
[the text migration review](reviews/csep-text-2026-10-13.md); it does not close target gates.
No qualified production target, approved external-standard compliance summary,
industrial certification or release evidence bundle is currently established.

| Gap ID | Observed condition | Acceptance action | Status |
| --- | --- | --- | --- |
| GAP-001 | Current replacement pointer-pool suite passed on GCC/Clang host; target resources and dependency analysis remain open | Analyze and qualify the exact revision on the selected target | Open |
| GAP-002 | MISRA matrix has complete baseline identifiers but no implementation dispositions | Assess all 221 entries with authoritative rule interpretation and evidence | Open |
| GAP-003 | Runtime pool reservation may need Directive 4.12 deviation | Approve scoped disposition or select static-object lifetime allocation | Open |
| GAP-004 | Compiler annotations and `restrict` now require explicit opt-in; assertion fallback and dialect-dependent features remain unassessed | Analyze actual enabled configurations and approved exceptions | Open |
| GAP-005 | No independently reviewed target ABI/timing/stack/DMA record | Qualify chosen platform/profile; record dependencies and measurements | Open |
| GAP-006 | Pointer pool is single-context; no erase, stale-alias-after-reuse detection or arbitrary RAM-fault containment | Restrict use and assess application ownership, secrets and fault containment | Open |
| GAP-007 | Hosting review/branch-protection/CI settings are not established by repository files | Configure actual hosting enforcement and retain its record | Open |
| GAP-008 | Public release license/security contact/support policy not selected in this task | Select and publish them before a supported distribution | Open |
| GAP-009 | Payment, telecom and automotive product standards/scope unselected | Integration owner establishes profile-specific assessments and responsibilities | Open |
| GAP-011 | String/status repeated headers and mixed C/C++ include/link passed on this host; other modules and target ABIs remain unqualified | Qualify the chosen consumer and C library toolchain/ABI across its actual scope | Open |
| GAP-012 | Bytes overlap/boundary suite passed on the current host; target memmove remains unqualified | Qualify target copy implementation, ABI, stack, timing and whole-program analysis | Open |
| GAP-013 | Shared runner passed on GCC/Clang and all three build systems; existing native int suite signatures still exceed numerical policy | Review the [proposed test-only exception](reviews/test-entrypoints-2026-10-11.md) before release acceptance | Open |
| GAP-014 | Endian/codec integer byte fixtures passed on this host; target 64-bit arithmetic helpers/stack/ABI remain assumptions | Assess the [endian change](reviews/bytes-endian-2026-10-11.md) and codec on the actual target | Open |
| GAP-015 | Sequential reader cursor/failure suite passed on this host; target ABI, stack and timing remain unverified | Complete the [reader target evidence](reviews/bytes-reader-2026-10-11.md) and scoped analysis | Open |
| GAP-016 | Ecosystem directories are placeholders; industrial/medical/defence overlays and concrete target contracts for all eight ecosystems are unset | Select product scope, system safety/security requirements, standards and target profiles before adoption | Open |
| GAP-017 | Updated bytes names compile/link in the current shared host suite; external stored consumers and target compatibility remain unqualified | Apply the [name migration](reviews/bytes-api-names-2026-10-11.md) to each supported consumer | Open |
| GAP-018 | Shared int64_t status registry and integration suite passed on this host, now including module 3; policy 0.2.0 project-role review is pending | Assess target resources/ABI and review the [status amendment](reviews/status-2026-10-11.md) before release | Open |
| GAP-019 | Original enum API suite passed on GCC/Clang host; native enum/bool/int/char policy conflicts and strict C89/target linkage assessment remain open | Review the [scoped migration](reviews/enum-traits-2026-10-13.md), policy disposition and target dependencies | Open |
| GAP-020 | String foundation passed host/API/overlap tests; counted text and real char-array fixtures exceed the current char-pointer-only policy | Resolve the [scoped storage proposal](reviews/string-storage-2026-10-13.md) and qualify target library/ABI/resources before release | Open |
| GAP-021 | Supplied native string-key enum generator is bounded and host-tested but exceeds numerical enum policy; borrowed/fixed strings use proposed text storage scope | Resolve the [scoped migration dispositions](reviews/csep-text-2026-10-13.md) and qualify actual ABI/resources/libraries before release | Open |
| GAP-010 | Lightweight checks cover only syntax-level restrictions and document consistency | Add qualified semantic/whole-program tools and independent review for release | Open |

Historical host tests from the earlier allocator are not replacement-pool evidence.
No percentage-compliant or "production ready" claim may be derived from matrix
completeness or the existence of this governance folder. Profile obligations are
planned release gates; unimplemented future features are not implied by their text.
