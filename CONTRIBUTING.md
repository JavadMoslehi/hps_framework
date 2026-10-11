# Contributing to HPS

The [engineering governance](governance/README.md) is the normative contribution
policy. Read its C, memory, security and applicable profile requirements before
changing code. HPS is currently development-only; no production target is qualified.

For each change, identify requirement IDs, preserve public contracts, explain
numerical/resource/ownership implications and retain relevant verification evidence.
Use the [change template](governance/templates/change.md) for substantial or
safety/security-relevant work. Update affected MISRA dispositions only when their
actual evidence exists; leave unassessed findings visible.

Run `python3 tools/check_policy.py`. This checks a limited set of source and
governance restrictions without a C build. A qualified release also needs configured
semantic analysis, compiled tests and target evidence. If a task explicitly defers
builds, record that constraint and classify the affected code as unverified.

Changes require maintainer review and independent review where the selected profile
requires it. Exceptions follow the [charter](governance/charter.md); never silence
warnings globally or treat a blank template as approval. Hosting branch protections
must be configured separately; they are not implemented by these files.

Do not add heap, OS or concurrency dependencies to the core. Keep exact-width
numerical types, C-string-only char pointers, typed static objects and explicit
failure/lifetime contracts. Consult [current gaps](governance/status.md) before
making compliance, deployment or performance claims.

Use descriptive names and purpose comments, and keep validation, bounds and
mutation order locally reviewable (HPS-C-017). Share avoidable duplicate logic
only where the contracts and resource bounds match; record justified repetition
or specialization (HPS-C-018). Do not weaken checks or couple an independent test
oracle to production code to reduce repetition.

Framework/module status APIs use `hps_status_t` (`int64_t`) from `hps/status.h` or
a module alias; flags remain `uint8_t`. Allocate module IDs and negative reasons
centrally, register bounded immutable diagnostic messages and document ABI/code
migration under HPS-C-004/HPS-C-019. See
[the status contract](docs/architecture/status.md) and
[the user-requested policy change](governance/reviews/status-2026-10-11.md).
Maintainer/reviewer disposition remains pending; this policy edit does not approve
a MISRA deviation.

For a qualified-target release, additionally run
`python3 tools/check_policy.py --require-assessed`. This record gate currently
fails because all baseline guidelines are unassessed; passing it later does not
replace the target/evidence review.

Public headers must follow the [header contract](governance/headers.md), including
explicit dependencies and guarded C linkage for C++ consumers of external C APIs.

Module tests expose a function matching the source filename: `test_bytes.c`
defines `test_bytes(void)`, for example. Declare suites in `tests/test_suites.h`
and call them once from the shared `tests/main.c` runner; module files do not define
`main`. Add each suite source to Make, CMake and Meson. Keep assertions enabled
and propagate failures. The user-requested `int` suite signatures have a
[proposed test-only policy exception](governance/reviews/test-entrypoints-2026-10-11.md);
the existing numerical policy/checker still reports them pending review.
