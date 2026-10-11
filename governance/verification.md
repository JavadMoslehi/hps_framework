# Verification and release acceptance

## Required evidence

HPS-VER-001: Every change has a requirement/change record. Include requirement IDs,
observable behavior, invariant/contract changes, failure modes, dependencies,
affected profiles and the source revision. Verification targets the behavior and
risks rather than simply mirroring implementation statements.

HPS-VER-002: A change review checks numerical bounds, initialization, ownership,
state transitions, error semantics, aliasing, lifetime, resource budgets and execution
context. Source inspection must distinguish verified properties from assumptions.
For safety/security changes, an appropriate independent specialist review is required
before the corresponding industrial release claim.

HPS-VER-003: Release evidence must name analyzer/compiler versions, C mode, target,
actual flags, included translation units, dependency versions and diagnostics.
Cover every MISRA guideline with its method, outcome and disposition. Analyzer
limitations need manual/alternative evidence. A handwritten checker is only a
small subset; its success is not evidence for the remaining rules.

HPS-VER-004: Plan tests for nominal operation, invalid inputs, boundary arithmetic,
capacity exhaustion, stale ownership, fault/error paths, startup/shutdown and any
promised recovery. Add fuzzing for untrusted parsers and target integration tests
for hardware assumptions. Use address/undefined-behavior/thread tools only where
they are supported and relevant; unsupported tooling remains a recorded gap.

HPS-VER-005: Maintain traceable coverage objectives justified by the chosen profile
and safety plan. Do not assert universal statement/branch/MC/DC thresholds without
that scope decision. Record exclusions, generated code and unreachable-path
reasoning. Coverage percentage does not prove correctness or independence.

HPS-VER-006: Verify resource budgets and timing on the target, not only on a host.
Capture stack use, allocation/capacity behavior, linker map, external symbol
requirements, latency distributions and justified WCET if promised. Dependency
symbol inspection is useful but not a proof of transitive no-heap behavior.

## Change and release gates

| Gate | Minimum acceptance evidence | Unresolved condition |
| --- | --- | --- |
| Documentation proposal | Valid links/IDs, consistency with code, explicit assumptions/status | Return for correction; no qualification claim |
| Development code change | Applicable policy checks, reviewed contracts, appropriate source/tests, visible remaining gaps | Keep unverified code classified development-only |
| Qualified-target release | Production build/tests, all-guideline disposition, approved deviations, target ABI/resource evidence, independent review, security/dependency records | Block qualification for that target |
| Industrial adoption claim | Above plus applicable system/profile assessments, integration assumptions and external validation where needed | Block corresponding industrial/compliance claim |

HPS-REL-001: A development snapshot may be shared only with licensing established
and status plainly stated. It must not be presented as qualified production code.
Changes whose tests were intentionally deferred remain unverified; record why and
what needs to run later. User instructions to skip builds do not become evidence
of a passing build.

HPS-REL-002: A release manifest records revision/tag, governance version, API/ABI,
qualified target profiles, hashes, build inputs, SBOM/license inventory, verification
reports, deviations, vulnerabilities, residual risks and supported upgrade behavior.
Keep artifacts independently reviewable and reproducible. Avoid non-deterministic
build claims until independently reproduced.

HPS-REL-003: Compatibility changes need an explicit migration note. Check public
structure layout, exact integer widths, alignment, API status representation,
configuration ABI and allocation lifetime. The recent type and pool replacements
invalidate historical results; they must not inherit earlier certification labels.

HPS-REL-004: Public statements and résumé entries must match completed work.
Acceptable today: "Designed a C framework with a static byte-block pool and engineering
governance covering MISRA guideline tracking and embedded deployment profiles."
Do not claim MISRA compliance, ISO/PCI certification, production payment processing,
proven WCET or performance figures absent scoped supporting evidence.

HPS-REL-005: Define maintenance support, regression policy, vulnerability reporting
and end-of-support per release. Do not promise global reliability or a service
level before the corresponding operational commitments exist.
