# Historical handle-pool validation record

Superseded by [current validation](validation.md); hashes below belong to the
earlier handle-pool snapshot.

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
| governance/misra/matrix.csv | `7e27e2adf584c622ff4a18c0708d8a540687cdab6efac82aa0834e98e982441e` |
| include/hps/memory/pool.h | `4a94a157ca562432f49082af9c578e01f84e0596525301565478cfd097fe40a1` |

The current [source review](reviews/source-review-2026-10-11.md) records the
reviewed file hashes, contract changes and unresolved integration obligations.
