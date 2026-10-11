# Evidence templates

Copy a template into a project-owned evidence location and fill every applicable
field. Do not store secrets or sensitive production payloads. Unfilled placeholders
are proposed work, not approval or verification evidence.

| Template | Used for |
| --- | --- |
| [Public header](public-header.h) | Guarded C declarations with a C++ linkage wrapper |
| [Change](change.md) | Requirement, design, code and verification traceability |
| [Deviation](deviation.md) | Scoped exceptions and external-standard deviations |
| [Target](target.md) | Platform, ABI, resource and deployment-profile qualification |
| [Release](release.md) | Public claims and retained acceptance evidence |

Evidence record names/revisions are stable within a release. A matrix references
actual completed records, not these blank templates. Assign named owners and
reviewers when records are created; a role label alone is not an approval.
