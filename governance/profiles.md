# Deployment profiles

HPS-PRO-001: Every supported product names an execution profile and any applicable
industry overlays. All profiles inherit core C, memory, security and verification
rules. Overlays add obligations; they do not imply qualification, certification or
new functionality. A profile is qualified only for the revision/configuration in
its approved target record. No profile is currently qualified.

## Execution profiles

| Profile | Required integration contract | Required target evidence |
| --- | --- | --- |
| Bare metal | Startup ordering, interrupts, ownership, clock/tick, watchdog, stack/linker budget, MPU/ECC assumptions, hardware access boundaries | On-board tests, bounded interrupt behavior, compiler/runtime dependencies, memory map, deadline evidence |
| RTOS | Task/ISR map, priorities, scheduler semantics, allowed APIs per context, critical-section budget, stack per task, startup resources | Target traces, priority-inversion assessment, stack high-water analysis, error paths and scheduling bounds |
| Hosted OS | Supported OS/libc/ABI, thread ownership, cancellation/signals, clocks, resource limits, shutdown, error translation | Native tests, dependency audit, sanitizer results where supported, contention/overload evidence for any future thread-safe components |

HPS-PRO-002: Bare-metal integration must not import pthread, processes, files,
sockets or OS allocation into the core. Platform adapters are separate modules
with explicit dependencies. A freestanding compiler flag is not proof that a
linked executable has no OS, runtime helper or heap dependency.

HPS-PRO-003: Single-owner modules remain single-owner even on an OS/RTOS. A
threaded application may use different pools per owner; shared access requires a
separately qualified synchronization layer. The current pool cannot be used from
a task and ISR concurrently. `volatile` does not provide synchronization.

## Telecom overlay

HPS-TEL-001: The integrating product must select the actual protocol/interface,
3GPP release, ETSI specification or applicable operator requirements. "Telecom"
alone is not a testable compliance target. Network bytes use explicit widths,
endianness, lengths, limits and versioning. Malformed peers cannot mutate state
before validation or cause unbounded unauthenticated work.

HPS-TEL-002: Document traffic model, MTU/message size, burst limits, queues,
backpressure/drop behavior, overload recovery, retransmission/timeouts and
observability. State-machine transitions must have ownership, valid predecessor,
error recovery and bounded resources. A parser or descriptor allocator is not
proof of a conforming network function or protocol stack.

HPS-TEL-003: Zero-copy/DMA claims require reference ownership, cache coherency,
layout and device lifetime evidence. Expose bounded aggregate counters without
subscriber identifiers or packet contents containing secrets. Assess service
availability, upgrade compatibility and tenant isolation at integration level.

For standards selection, see [ETSI's NFV program](https://www.etsi.org/technical-groups/nfv/);
its scope is not a universal C-framework certification scheme.

## Performance overlay

HPS-PERF-001: Set measurable per-operation and per-workload throughput/latency,
memory/stack, startup and energy targets where relevant. Report distributions and
resource-exhaustion paths on pinned hardware/configuration. Compare equivalent
workloads and publish limitations. No "fastest", zero-overhead or hard-real-time
claim without evidence supporting that exact statement.

HPS-PERF-002: Treat cache-line alignment, vector instructions, batching, memory
barriers and busy polling as platform choices. Maintain a portable core; isolate
specialized implementations and verify equivalent observable behavior. Pay for
bounded checks explicitly rather than bypassing them in a release mode.

## Automotive overlay

HPS-AUTO-001: The integrating safety plan must identify the item, hazards, allocated
software requirements, intended ASIL where applicable and component assumptions.
Define independence, diagnostic behavior, freedom-from-interference assumptions,
tool confidence, configuration controls and verification depth as required by
that safety plan. A pool has no standalone ASIL rating under this project policy.

HPS-AUTO-002: Safety-related paths need traceability to requirements, justified
coverage goals, target fault responses and reviewed external components. Identify
safe/degraded states, deadlines, watchdog supervision and consequences of exhaustion,
corruption and stale references (including undetectable aliases after address reuse). Module status values alone are not
an implemented safety mechanism for the whole vehicle.

HPS-AUTO-003: Automotive functional-safety and cybersecurity assessments are distinct.
Select applicable standards and evidence at system level. Do not label the project
ISO 26262 compliant, AUTOSAR compliant or automotive qualified without a scoped
assessment. The [ISO 26262 overview](https://www.iso.org/publication/PUB200262.html)
describes a lifecycle spanning far beyond C source style.

## Payment overlay

HPS-PAY-001: Identify payment data flows, trust boundaries and product/operator
responsibilities. Choose applicable PCI DSS and/or PCI Software Security Framework
assessment scope and editions; document the assessor/integrator decision. This
framework is neither a payment implementation nor a validated payment product.

HPS-PAY-002: Numeric money representations use integer minor units or a reviewed
fixed-point model with currency/exponent, range and checked arithmetic. No binary
floating-point money computations. Authentication, authorization, idempotency,
replay protection, durable transaction state and reconciliation are application
requirements, not supplied by the pool.

HPS-PAY-003: Sensitive payment data must not enter logs, generic examples, unchecked
static buffers or retained pool payloads. Define erasure, retention, cryptographic
provider and key management before introducing such data. Implement transaction
recovery and duplicate-request semantics independently of memory allocation.

HPS-PAY-004: Secure lifecycle evidence and operating-environment controls must be
assessed separately. A numerical-type checker cannot establish PCI compliance.
See [PCI SSC's standards](https://www.pcisecuritystandards.org/standards/),
[PCI DSS](https://www.pcisecuritystandards.org/standards/pci-dss/) and
[Secure SLC](https://www.pcisecuritystandards.org/standards/secure-software-lifecycle-secure-slc/).

## Combining profiles

HPS-PRO-004: Record a concrete composition, for example "bare-metal + automotive +
performance" or "hosted single-owner + telecom + performance". List applicable and
excluded requirements with reviewed rationale. No whole-domain not-applicable
shortcut based on the absence of that domain's modules today.
