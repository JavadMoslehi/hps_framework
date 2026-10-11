# Security engineering

HPS-SEC-001: Each module/profile needs a threat model identifying assets, entry
points, trust boundaries, attackers, operational assumptions and misuse cases.
Distinguish memory safety, confidentiality, integrity, availability and functional
safety. A control for one property must not be advertised as proof of the others.

HPS-SEC-002: Treat network packets, device input, persisted state, external lengths,
configuration and shared-memory messages as untrusted. Validate version, type,
length, range and resource budget before indexing/copying or committing state.
Parsing must be incremental or bounded, reject truncation/overflow, and have explicit
behavior for unknown or malformed fields. Encode/decode wire formats explicitly;
never cast a packet to a native C structure or assume endianness/alignment.

HPS-SEC-003: Define exhaustion, rate-limit and overload behavior. An invalid-input
path may not consume unbounded CPU, memory, logs or retries. Bound unauthenticated
work before allocating reservations. Separate tenants/security domains at the
application boundary; a numerical handle is not authorization or a secret capability.

HPS-SEC-004: Do not implement new cryptographic primitives in the core. Use a reviewed
provider with documented version, platform support, entropy source, key lifecycle,
no-heap behavior if required and constant-time guarantees for the relevant secrets.
`rand`, timestamps, counters and predictable pool handles are not secure randomness.
Constant-time claims require source/generated-code review and target evidence;
ordinary O(1) allocation is not a side-channel guarantee.

HPS-SEC-005: Classify sensitive assets and minimize collection, copying, retention
and access. Logs, metrics, crash dumps, test fixtures and traces must not disclose
secrets or payment authentication material. Sensitive buffers need an approved
erasure primitive that the target compiler cannot remove, with explicit lifetime
and ownership. The pool currently does not wipe payloads and is not approved as
secret storage solely because it uses static memory.

HPS-SEC-006: Security-critical failures fail closed according to the approved system
contract. Propagate checked errors without leaking sensitive internal state.
Authentication/authorization decisions belong to the application boundary;
transport encryption alone does not authorize a message. Audit events must be
bounded, integrity-protected where required and use a reviewed data schema.

HPS-SEC-007: Maintain dependency/version/license inventory, provenance and a release
SBOM. Assess toolchain, libc, OS/RTOS, crypto and binary dependencies within the
product's threat model. Pin release inputs and review upgrades; externally developed
code needs evidence rather than an automatic trust exemption.

HPS-SEC-008: Before a supported release, publish a functioning vulnerability contact
and disclosure process. Do not put live exploit details or secrets in ordinary
change records. Maintain severity/impact assessment, affected versions, mitigation,
fix verification, advisories and coordinated release records. The project's contact
and response commitments are currently unset; do not invent an SLA.

HPS-SEC-009: Verification should include adversarial boundaries, parser fuzzing,
repeated exhaustion/recovery, ownership misuse, sanitizer checks where supported
and target fault injection where required. Compiler hardening, stack protections
and memory protections are target decisions with measured impact, not unconditional
portable guarantees. A sanitizer pass does not prove absence of exploitable defects.

These are HPS requirements informed by lifecycle guidance such as
[NIST SSDF](https://csrc.nist.gov/pubs/sp/800/218/final); they are not a statement
that HPS has completed that framework's practices.
