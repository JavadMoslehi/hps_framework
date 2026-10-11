# Development source review — 2026-10-11

Historical record: the pool/API portions were superseded by the
[pointer-pool change](pointer-pool-2026-10-11.md). The hashes below describe the
earlier source snapshot and do not validate the current allocator.

Scope: all 15 public headers, both production C sources, both C test sources,
Python policy/portability checks and CMake/Make/Meson source manifests in the
working tree. This is an author review against repository governance, not an
independent MISRA assessment. No deployment profile is qualified. Build files
were inspected but no build, C execution or analyzer was performed, following
the user's instruction. Files below identify the reviewed source snapshot.

## Findings and changes

| Area | Review result | Policy mapping |
| --- | --- | --- |
| All public headers | Path-derived unique guards, file metadata, explicit dependencies; pool C prototypes have guarded C linkage. Macro-only headers do not need linkage blocks. C++ include/link qualification remains open. | HPS-HDR-001 through HPS-HDR-006 |
| Base identity/language/architecture/features | Detection retains unknown values rather than inventing target support. Features are capabilities, not permission to use VLA, atomics or complex types. Compiler macros, library availability and ABI still need target analysis. | HPS-C-002, HPS-C-015, HPS-C-016 |
| Base attributes/version | Compiler annotations and restrict default off; only validated 0/1 opt-ins permit enabling them. Static-assert token pasting and dialect syntax remain unassessed. Version macro requires constant arguments without side effects. | HPS-C-007, HPS-C-013, HPS-C-015 |
| Pool API | Expanded extent, initialization, aliasing, failure preservation, lifetime, status, context and complexity contracts. Exposed bookkeeping permits caller-owned static storage; consumers must not mutate it. | HPS-C-009 through HPS-C-013, HPS-MEM-002, HPS-MEM-004 through HPS-MEM-006, HPS-EXEC-001 |
| Pool implementation | Bounds precede indexed access; short-circuit count checks bound subtraction. Successful acquisition has available >= 1; release has allocated >= 1, bounding count changes by capacity. Initialization bounds metadata bytes, bounding index+1. Generations retire at UINT32_MAX. Added detection of a non-sentinel link in an allocated slot. Arbitrary pointers and free-list-wide corruption are outside these local checks. Essential-type results still require analysis. | HPS-C-006 through HPS-C-011, HPS-MEM-001, HPS-MEM-004, HPS-MEM-006 |
| Hosted example | Replaced stdio hello with static typed storage and checked initialization/acquire/resolve/release; fixed capacity, no retries or I/O. main retains the documented hosted signature exception. | HPS-C-005, HPS-C-009, HPS-MEM-001, HPS-MEM-002 |
| C tests | Bounded signed helper; deterministic model arithmetic widened and explicitly reduced instead of implicit wrapping. Added allocated-link corruption/failure-preservation case. Boundary injection is test-only. Assertions perform operations and require enabled assertions, as specified in pool build targets. Tests are prepared, not executed. | HPS-C-006, HPS-C-008, HPS-SEC-009 |
| Tooling/manifests | Current targets refer to static pool and base; no pthread allocator target remains. Portability diagnostic tests opt into compiler annotations and cover invalid configuration. Lexical checker cannot establish semantics, complete dependencies or linked no-heap behavior. | HPS-C-015, HPS-MEM-001, HPS-HDR-006 |

## Pool threat assumptions

Assets are reservation integrity, bounded availability and caller payload lifetime.
Entry points are the five public functions. Valid typed pointers and actual extents,
exclusive context ownership and protected metadata are trusted caller obligations.
Stale or foreign handles and finite exhaustion are checked; handles are predictable
identifiers, not authorization. An attacker able to write pool metadata is outside
this software boundary. Payload confidentiality/erasure, admission control, tenant
separation and fault recovery belong to integration. Repeated failure does no
retry, allocation or logging in the core. Target stack, cache, timing, DMA and
secret-erasure evidence are absent. This records HPS-SEC-001/003/005/006 assumptions,
not a product threat model or security approval.

## Remaining gates

All 221 MISRA guidelines still need authoritative target-scoped disposition.
Directive 4.12 is explicitly open for runtime reservation; no deviation is approved.
No configured production target, independent approval, whole-program analysis,
C/C++ ABI test, runtime result, SBOM, license or industrial qualification is supplied
by this review. See [adoption status](../status.md). The source-policy self-tests and
whitespace check pass; see [validation](../validation.md) for command evidence.

## Source snapshot

| File | SHA-256 |
| --- | --- |
| CMakeLists.txt | `b38839a8325c8f7f57d6f401be51d3b3cf38c49a42c40aa58d6a1c3ee7fd77fc` |
| Makefile | `922bb152305930c963a4472333cfd913830839fbc11dd04709de8efcaafbcbe5` |
| include/hps/base/architecture/endian.h | `6de5a97d5a569cfa43bf3e16a9518c40bf31142a52a574eb72901640cd4491c3` |
| include/hps/base/architecture/id.h | `e690850b300b1a85afc9331d1f5c280294d17a5b43c3f8a49bba5513bc3dfcb5` |
| include/hps/base/architecture/pointer.h | `ced06d3d93c96d161038d231d8ed01ab1800436d9894e3bba423bad84d6928b3` |
| include/hps/base/architecture.h | `612da9c221c17b7cf58627d1f93c054ccd4c951e4ce573016a7a2e297c5ddfe6` |
| include/hps/base/attributes.h | `5437b759eae965f26cb1d5df74e78d3e937e3349907ba90559e2264775656a28` |
| include/hps/base/compiler.h | `8964fdd93bcb3d30cd7ff4c4075d747c3fcc44ec60b05105d5bf863e8e8a3c15` |
| include/hps/base/features/architecture.h | `404272bc555e5199da6d456f2c4d2ee1b94ffaee5f0a955960498ea7fdd3d89e` |
| include/hps/base/features/compiler.h | `1acf8df7fdfb4d5a3b20da1125c5621fb9951cc1711109565faa9af9fe3fbd1c` |
| include/hps/base/features/language.h | `d16dea205203602538e2a2601a70e2ba3730ea252f716073da9d938413946378` |
| include/hps/base/features.h | `00c8e8cc39336b653cbe244d6756c92111dd81324ad659118333158decb55f31` |
| include/hps/base/language.h | `20820af9fa58680d9c832efac36d87e9380c10479faebd106545cd9298144e74` |
| include/hps/base/standard.h | `d2110b4c395bb7798ebaeb744f4796f4c7b3f232d9f0244686e276dac27f2638` |
| include/hps/base/version.h | `d5f78188bec1ae15623f651c454523358a61b2bce0486d4de0d4cb4375e9a5a1` |
| include/hps/base.h | `6eae7cf0612dc76041696da955ee0da6485cc62859ac7b3d0e1812fab7de0738` |
| include/hps/memory/pool.h | `4a94a157ca562432f49082af9c578e01f84e0596525301565478cfd097fe40a1` |
| meson.build | `570259f1845c90d010e8598cbe64e8e551cf1bd333de427317bf44fa4b1d5013` |
| src/main.c | `aea9c4d6bfe3256cb05c66c92b7367253e8f1ff3356e1e858194482be263e551` |
| src/memory/pool.c | `614550192e356d85338096951f2ef342e10f7a5877ab68778ef462b9c7af28de` |
| tests/base/check_portability.py | `a4888beb48a29afb942a6cb9b8140d6907f095fcb4baaf1d76769835d143d841` |
| tests/base/test_base.c | `1bd41cb3259b6de9672554c9f35ed864cdb20a760f233fcdfd178dd3695c4e61` |
| tests/memory/test_pool.c | `e0cd3b62a7e90d3f1eaedd3ec0962211887470a9417ccf73fa49bf0f1251d6f0` |
| tools/check_policy.py | `3779598356e740da2ce85e59e823ea8040d1e4fd5d7c64d1c4ea644bf03ce23a` |
