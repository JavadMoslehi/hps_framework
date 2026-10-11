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

For a qualified-target release, additionally run
`python3 tools/check_policy.py --require-assessed`. This record gate currently
fails because all baseline guidelines are unassessed; passing it later does not
replace the target/evidence review.

Public headers must follow the [header contract](governance/headers.md), including
explicit dependencies and guarded C linkage for C++ consumers of external C APIs.
