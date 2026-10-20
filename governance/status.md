# Current adoption status

Baseline date: 2026-10-11; standard-type provider review updated 2026-10-20. This is a
gap register, not a progress estimate. Earlier host results below belong to
[the text migration snapshot](reviews/csep-text-2026-10-13.md). The changed error
representation invalidates those results for affected sources; current evidence
and remaining checks belong to the
[removal review](reviews/status-removal-2026-10-18.md). Host checks do not close
target gates.
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
| GAP-011 | Changed text headers and a C++17 consumer linked to fresh C99 archives passed on this host; other consumers and target ABIs remain unqualified | Qualify the selected consumer/toolchain/target ABI across its actual scope | Open |
| GAP-012 | Bytes overlap/boundary suite passed on the current host; target memmove remains unqualified | Qualify target copy implementation, ABI, stack, timing and whole-program analysis | Open |
| GAP-013 | Shared runner passed on GCC/Clang and all three build systems; existing native int suite signatures still exceed numerical policy | Review the [proposed test-only exception](reviews/test-entrypoints-2026-10-11.md) before release acceptance | Open |
| GAP-014 | Endian/codec integer byte fixtures passed on this host; target 64-bit arithmetic helpers/stack/ABI remain assumptions | Assess the [endian change](reviews/bytes-endian-2026-10-11.md) and codec on the actual target | Open |
| GAP-015 | Sequential reader cursor/failure suite passed on this host; target ABI, stack and timing remain unverified | Complete the [reader target evidence](reviews/bytes-reader-2026-10-11.md) and scoped analysis | Open |
| GAP-016 | Ecosystem directories are placeholders; industrial/medical/defence overlays and concrete target contracts for all eight ecosystems are unset | Select product scope, system safety/security requirements, standards and target profiles before adoption | Open |
| GAP-017 | Updated bytes names compile/link in the current shared host suite; external stored consumers and target compatibility remain unqualified | Apply the [name migration](reviews/bytes-api-names-2026-10-11.md) to each supported consumer | Open |
| GAP-018 | Shared status registry is removed; direct int32_t module-local errors change public return ABI and numeric codes under policy 0.3.0 | Review the [removal/migration record](reviews/status-removal-2026-10-18.md), rebuild consumers and assess target resources/ABI before release | Open |
| GAP-019 | Original enum API suite passed on GCC/Clang host; native enum/bool/int/char policy conflicts and strict C89/target linkage assessment remain open | Review the [scoped migration](reviews/enum-traits-2026-10-13.md), policy disposition and target dependencies | Open |
| GAP-020 | String foundation passed host/API/overlap tests; counted text and real char-array fixtures exceed the current char-pointer-only policy | Resolve the [scoped storage proposal](reviews/string-storage-2026-10-13.md) and qualify target library/ABI/resources before release | Open |
| GAP-021 | Supplied native string-key enum generator is bounded and host-tested but exceeds numerical enum policy; borrowed/fixed strings use proposed text storage scope | Resolve the [scoped migration dispositions](reviews/csep-text-2026-10-13.md) and qualify actual ABI/resources/libraries before release | Open |
| GAP-022 | Optional allocator/clone headers introduce callback-driven ownership outside default core paths; HPS-MEM-001 prohibits dependency allocator hooks in production core | Resolve the [proposed scoped exception](reviews/memory-layers-2026-10-19.md) before production-core adoption; qualify bounded typed providers and callback contracts | Open |
| GAP-023 | Raw memory operations use target libc and require unsigned-character uint8_t representation access; volatile clearing is not qualified sensitive erasure | Qualify target representation, libc resources/ABI and any erasure use under the [memory review](reviews/memory-layers-2026-10-19.md) | Open |
| GAP-024 | Additional compiler/CPU/OS/libc/POSIX identities and feature probes have host/synthetic evidence only; library/SDK evidence is captured at first inclusion | Qualify actual vendor toolchains, target macros, SDK/libc/profile configuration and include order under the [detection review](reviews/detection-layers-2026-10-19.md) | Open |
| GAP-025 | New optional allocated arrays/manual owners retain pending allocator-hook disposition; attributes and hosted diagnostics require target-specific contracts | Resolve the [utility/runtime dispositions](reviews/utilities-runtime-2026-10-19.md), explicit ownership cleanup, annotation proofs and hosted libc/resource/security boundaries before production adoption | Open |
| GAP-026 | Bounded JSON, String tokenization/indentation, refcounts and typed collection operations have host evidence; counted-character scope, target charset/ABI/resources and application ownership/schema/rate controls remain unqualified | Resolve the [runtime/text/JSON review](reviews/objects-json-2026-10-19.md) and perform scoped semantic, independent and target verification before production acceptance | Open |
| GAP-027 | Added Bytes ordering/search/pop, typed optional values and compile-target x86 flags have host evidence only; optional copy/lifetime contracts and target/compiler/ABI/resource evidence remain unqualified | Complete the [23-header migration review](reviews/bytes-features-optional-2026-10-19.md), named acceptance and actual target verification; compile-target ISA metadata is not runtime permission | Open |
| GAP-028 | Standard-name providers and narrow HPS-C-020 scope are implemented; replacement/compiler metadata and Boolean ABI need target evidence | Review the [provider record](reviews/standard-types-2026-10-20.md), select one provider per target and complete independent acceptance/target checks | Open |
| GAP-010 | Lightweight checks cover only syntax-level restrictions and document consistency | Add qualified semantic/whole-program tools and independent review for release | Open |

Historical host tests from the earlier allocator are not replacement-pool evidence.
No percentage-compliant or "production ready" claim may be derived from matrix
completeness or the existence of this governance folder. Profile obligations are
planned release gates; unimplemented future features are not implied by their text.
