# HPS engineering governance

This directory is the normative engineering policy for HPS. It defines what a
contributor must demonstrate before a change or release can be accepted. The
policy is written in English so that external reviewers and integrators can use
the same requirements. Policy version: 0.3.0; established: 2026-10-11; amended:
2026-10-18. The user-requested removal of the shared status system is recorded in
the [removal review](reviews/status-removal-2026-10-18.md). Readability and
duplication rules remain in force; project-role review and release acceptance
remain pending.

HPS is currently a development framework with host-tested modules and no qualified
deployment target. This policy is an engineering baseline, not a certificate, a safety case,
a PCI validation, or evidence that current code already satisfies every rule.
No deployment profile is qualified yet. An unresolved finding remains visible
and blocks the corresponding release claim.

## Navigation

| Document | Purpose |
| --- | --- |
| [Charter](charter.md) | Scope, authority, decisions and exceptions |
| [Public headers](headers.md) | Guard, declarations, C linkage and dependency contracts |
| [C rules](c-rules.md) | Types, expressions, APIs, pointers and declarations |
| [Memory and execution](memory-and-execution.md) | Static resources, timing and ownership |
| [Security](security.md) | Threat model, trust boundaries, sensitive data and dependencies |
| [Deployment profiles](profiles.md) | Bare-metal, hosted, RTOS, telecom, automotive and payment requirements |
| [Verification and releases](verification.md) | Traceability, review gates, evidence and permitted claims |
| [Standards register](standards.md) | Edition selection, scope and primary references |
| [MISRA plan](misra/README.md) | All-guideline index, review methods and deviation handling |
| [MISRA matrix](misra/matrix.csv) | One disposition record for each of 221 baseline guidelines |
| [Validation record](validation.md) | Reproducible subset checks and their limitations |
| [Bytes review](reviews/bytes-2026-10-11.md) | Borrowed views, fixed-capacity buffers and pending evidence |
| [Endian bytes review](reviews/bytes-endian-2026-10-11.md) | Bounded integer encoding and pending target/test evidence |
| [Integer codec review](reviews/bytes-codec-2026-10-13.md) | Order-selecting conversion APIs, integration and source-only evidence |
| [Enum traits migration](reviews/enum-traits-2026-10-13.md) | CSEP naming migration, original API restoration and utility-layer placement |
| [CSEP text migration](reviews/csep-text-2026-10-13.md) | Cstr, builder/policy, spans/string keys and current compiled evidence |
| [Data-structure generator review](reviews/data-structure-2026-10-18.md) | Typed descriptor/node generators, static storage and draft naming migration |
| [String foundation review](reviews/string-2026-10-13.md) | Bounded text views/buffers, legacy adaptation and current host verification |
| [Character storage proposal](reviews/string-storage-2026-10-13.md) | Counted text and actual char-array fixtures; release disposition pending |
| [Bytes reader review](reviews/bytes-reader-2026-10-11.md) | Checked cursor, ecosystem mapping and pending verification |
| [Bytes API naming review](reviews/bytes-api-names-2026-10-11.md) | Explicit byte operations, migration and source-only checks |
| [Local error results review](reviews/status-removal-2026-10-18.md) | Shared status removal, ordinary return types and API/ABI migration |
| [Historical shared status review](reviews/status-2026-10-11.md) | Superseded namespace/diagnostic proposal and retained source evidence |
| [Test entry-point review](reviews/test-entrypoints-2026-10-11.md) | Shared runner and proposed test-only numerical exception |
| [Pointer-pool review](reviews/pointer-pool-2026-10-11.md) | Current allocator contract, changes and open findings |
| [Earlier source review](reviews/source-review-2026-10-11.md) | Historical inventory and earlier handle-pool snapshot |
| [Current gaps](status.md) | Facts and work still required for adoption |
| [Templates](templates/README.md) | Concrete artifacts for changes, targets and releases |

## Applying this policy

Before changing production code, read the C, memory and security rules and choose
the affected deployment profile. Identify the policy requirement IDs in the change
record. Provide evidence appropriate to that change and update any affected MISRA
rows. Reviewers reject changes with unresolved applicable violations unless the
charter allows a recorded, approved exception. Never infer compliance from a
successful compilation or a zero-warning analyzer run alone.

The lightweight command `python3 tools/check_policy.py` checks selected source
restrictions and governance consistency without compiling or executing C code.
It is not a MISRA analyzer. The [root agent rules](../AGENTS.md) and
[contribution instructions](../CONTRIBUTING.md) make this policy discoverable.
Repository hosting branch protection and required review settings must still be
configured by maintainers; files cannot prevent someone bypassing review.

Current scope is the base headers, static pool core,
borrowed bytes module, bounded string/Cstr utilities, typed spans, checksum
validation and named-value traits. Tests/examples follow the
same numerical type rules with a documented hosted `main` exception. OS adapters,
cryptographic services, protocol stacks and payment applications are future or
external integration responsibilities.
