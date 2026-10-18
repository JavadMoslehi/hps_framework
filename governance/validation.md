# Historical governance validation record

This record preserves the 2026-10-11 snapshot and its hashes. It is not current
verification for later source/policy changes. Current shared-status removal
checks and their limitations are recorded in the
[2026-10-18 review](reviews/status-removal-2026-10-18.md); earlier host text
verification is retained in the [text review](reviews/csep-text-2026-10-13.md).

Date: 2026-10-11. Scope: source-policy subset and governance record consistency in
the current working tree. This is not a production-source MISRA assessment or
independently approved target evidence.

## Reproducible checks

`python3 tools/check_policy.py --self-test` passed:

- 16 lexical positive/negative cases covering numerical types, strings, hosted main,
  ignored comments/literals, allocation/unsafe APIs and concurrency includes.
- 3 implementation-scope cases checking bytes-core I/O/concurrency rejection
  and the hosted-example scope.
- 8 public-header cases covering guards, metadata, C linkage, declaration placement
  and includes outside linkage blocks.
- 6 negative governance cases covering missing/duplicate guidelines, altered
  categories, incorrect review assignment, missing evidence and mandatory deviations.
- 221 baseline guideline identities/categories and their review assignments checked.
- 79 unique project requirement IDs and local Markdown link targets checked.
- Current C source and literal generated-test/example C passed the checked subset.
- All 17 public headers passed the structural checks; mixed C/C++ compile/link
  behavior has not been tested.

`python3 tools/check_policy.py --require-assessed` was checked and correctly returned
failure because 220 dispositions remain unassessed and Directive 4.12 remains open. This prevents that record gate
from being mistaken for completed compliance.

`git diff --check` passed. No C compilation, C tests, MISRA analyzer, target timing
measurement or industrial assessment was performed. External hosting protection,
actual reviewer approval and the truth of referenced evidence are outside this
checker's scope. Changes to these artifacts require rerunning the checks.

## Checked artifact hashes

| Artifact | SHA-256 |
| --- | --- |
| tools/check_policy.py | `77864f541eea7e982d07751c426a4cb4f504c50bcdda96186b0cfd8584e585a6` |
| governance/misra/baseline.json | `848204e16e7045b08fab5f584a86f910a7588b51ddd3daf0b9071a9f0eaa4fb0` |
| governance/misra/matrix.csv | `2bc54858667079b676566679fb6719839ae8dbd3196a228264c7f5a3fd9a396d` |
| include/hps/memory/pool.h | `3220f354011f27de08db09810d17cffd4b855c3e80688b7ecbe9beaf0271b48c` |

The [pointer-pool review](reviews/pointer-pool-2026-10-11.md) records the
reviewed file hashes, contract changes and unresolved integration obligations.

Previous checks are retained in the [historical handle-pool record](validation-handle-pool.md).
No compiled signature, payload-alignment, ABI or runtime result is implied.

The [bytes change review](reviews/bytes-2026-10-11.md) records the new API,
source hashes and unexecuted boundary/overlap test plan. Previous pool-only checks
are retained in the [historical pointer-pool record](validation-pointer-pool.md).
