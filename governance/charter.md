# Policy charter

## Authority and requirement language

HPS-GOV-001: `MUST` is a release requirement, `MUST NOT` is a prohibition, and
`SHOULD` requires a written rationale when not followed. Requirement IDs are stable.
This directory governs production source, public headers, generated production
code, documentation examples, integrations and release claims within their scope.
System requirements may strengthen it. A proposed relaxation needs a policy-change
record; a lower-level module document cannot silently override it.

HPS-GOV-002: Apply these documents in order: the approved target/profile record,
this governance baseline, the module contract, and implementation notes. If two
requirements conflict, stop the affected implementation or acceptance decision,
record the conflict and obtain a maintainer disposition. Keep unrelated work moving.
A conflict cannot be resolved by weakening an external mandatory obligation.

HPS-GOV-003: The selected MISRA baseline is C:2023. This is a pinned project choice,
not a claim that it is the newest edition. A target record must specify the exact
edition/amendments and compiler C mode; other editions require a separate matrix
and reviewed migration. Full authoritative standards must be available to the
reviewer. The public guideline index is not a substitute for their full provisions.

## Roles and independence

HPS-GOV-004: Every accepted change needs an author, a maintainer and a reviewer.
A reviewer evaluates contracts, failures, numerical reasoning and evidence, not
only formatting. The author must not be the sole approver of a deviation or
industrial release claim. Safety/security profiles require appropriate specialist
review; a solo-maintainer project must record missing independent review as a gap.

HPS-GOV-005: A reviewer must be able to trace requirement -> design -> implementation
-> verification -> residual limitation. Evidence includes a source revision,
configuration, tool version, scope, result and reviewer. Empty evidence means
unverified. Prior evidence becomes historical when affected code or settings change.

## Exceptions and deviations

HPS-GOV-006: Distinguish a project-policy exception, an external-standard deviation,
an analyzer false-positive disposition and a demonstrated not-applicable decision.
One is not evidence for another. Record each using the deviation template, including
scope, reason, alternative considered, risk, compensating controls, evidence,
independent reviewer, authorization, expiry and revalidation triggers.

HPS-GOV-007: Mandatory MISRA guidelines cannot be deviated from. Required guideline
exceptions require the applicable formal deviation process; advisory disposition
must be recorded and any recategorization explicitly approved. A not-applicable
claim needs target/scope evidence and review. Broad warning suppression, guessed
rule meanings or a blanket "third-party code" waiver are prohibited.

HPS-GOV-008: Exceptions stay proposed until explicitly approved by named project
roles. This document does not itself approve a deviation for existing base macros,
runtime pool allocation, compiler extensions or hosted/library interfaces. Authors
may prepare reviewable changes, but cannot label unresolved deviations compliant.

## Change control and accountability

HPS-GOV-009: Every policy change records motivation, impacted profiles, migration
requirements and evidence invalidated. Version the policy with the release. Do not
rewrite historical reports to make an earlier release appear compliant.

HPS-GOV-010: Maintainers must configure protected branches, required review and
policy checks in the actual hosting service before asserting enforced governance.
No hosting configuration is established by this documentation. An emergency fix
may use a documented expedited review, with bounded follow-up and preserved records;
it cannot bypass an external mandatory obligation.

HPS-GOV-011: A public license, dependency license inventory and security-contact
route must be decided and published before distributing a supported release.
Do not assume that publicly visible source grants redistribution or assessment rights.
