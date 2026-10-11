# Historical pointer-pool validation record

Historical snapshot before the bytes module. See [current validation](validation.md).

Date: 2026-10-11. Scope: source-policy subset and governance record consistency in
the current working tree. This is not a production-source MISRA assessment or
independently approved target evidence.

## Reproducible checks

`python3 tools/check_policy.py --self-test` passed:

- 16 lexical positive/negative cases covering numerical types, strings, hosted main,
  ignored comments/literals, allocation/unsafe APIs and concurrency includes.
- 8 public-header cases covering guards, metadata, C linkage, declaration placement
  and includes outside linkage blocks.
- 6 negative governance cases covering missing/duplicate guidelines, altered
  categories, incorrect review assignment, missing evidence and mandatory deviations.
- 221 baseline guideline identities/categories and their review assignments checked.
- 79 unique project requirement IDs and local Markdown link targets checked.
- Current C source and literal generated-test/example C passed the checked subset.
- All 15 public headers passed the structural checks; mixed C/C++ compile/link
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
| tools/check_policy.py | `3779598356e740da2ce85e59e823ea8040d1e4fd5d7c64d1c4ea644bf03ce23a` |
| governance/misra/baseline.json | `848204e16e7045b08fab5f584a86f910a7588b51ddd3daf0b9071a9f0eaa4fb0` |
| governance/misra/matrix.csv | `363a4e1d56a4c2ab95ed20fc33aa58ad0b7a930a90d86479e441fda20ec4084d` |
| include/hps/memory/pool.h | `3220f354011f27de08db09810d17cffd4b855c3e80688b7ecbe9beaf0271b48c` |

The current [pointer-pool review](reviews/pointer-pool-2026-10-11.md) records the
reviewed file hashes, contract changes and unresolved integration obligations.

Previous checks are retained in the [historical handle-pool record](validation-handle-pool.md).
No compiled signature, payload-alignment, ABI or runtime result is implied.
