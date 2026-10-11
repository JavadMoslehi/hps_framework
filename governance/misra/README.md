# MISRA guideline enforcement plan

Pinned baseline: MISRA C:2023 public identifier index, reviewed 2026-10-11.
The matrix contains 21 directives and 200 rules: 221 unique guideline records.
`baseline.json` retains the identifier manifest; `matrix.csv` records dispositions.
This is coverage of the assessment index, not demonstrated implementation compliance.

Identifiers and published category metadata come from the
[MISRA Consortium's public index](https://gitlab.com/MISRA/MISRA-C/MISRA-C-2012/tools/-/raw/main/misra_c_2023__headlines_for_cppcheck.txt).
Full guideline text, explanations and examples are intentionally not reproduced.
A reviewer needs the licensed authoritative edition and
[MISRA Compliance:2020](https://www.misra.org.uk/app/uploads/2021/06/MISRA-Compliance-2020.pdf).
The plans below are HPS-authored verification assignments, not rule summaries.

## Matrix interpretation

| Field | Meaning |
| --- | --- |
| kind / guideline_id | Exact baseline identity; no omitted or duplicate guideline |
| category | Published baseline classification; do not silently recategorize |
| scope | Initial production plus target-dependency scope; refine with the target record |
| method_group | Review assignment below, not a claim the method is already implemented |
| owner_role | Role responsible for obtaining a disposition; assign a named reviewer in evidence |
| status | unassessed, open, satisfied, deviation-approved or not-applicable |
| evidence | Relative path to a reviewed record identifying revision/target/method/results |
| deviation | Relative path to the approved deviation, when that status is used |

All rows begin `unassessed`. A `satisfied` result needs evidence covering its entire
specified scope/configuration. `not-applicable` needs an approved reason; absence
of a source keyword is insufficient for process, dependency or whole-system rules.
`deviation-approved` needs evidence and the deviation record, never for a mandatory
guideline. The checker verifies pinned categories, row assignments and record existence,
not the truth of an approval. `python3 tools/check_policy.py --require-assessed`
rejects unresolved matrix dispositions; passing that gate still requires independent
review and all target/profile release evidence.
`open` is a tracked finding or pending decision; it does not pass a qualified release.

Do not mark a whole family satisfied because its analyzer emits no messages.
Record undecidable obligations, analyzer limitations, alternative methods and
manual-review conclusions for each applicable row. Classify adopted libraries,
headers, generated code and binaries within the actual product scope.

## Directive review assignments

Each directive has its own method group (`D` followed by its identifier).

| Group | HPS verification assignment | Required record |
| --- | --- | --- |
| D1.1 | Target ABI/compiler/library assumptions and configuration review | Target specification and assumption register |
| D2.1 | Actual production translation units and warning/error results | Build configuration/report |
| D3.1 | Requirements-to-change/test link review | Traceability record |
| D4.1 | Failure-mode, bounds and recovery review | Error-path/fault evidence |
| D4.2 | Assembly inventory and justified target-specific decisions | Adapter/assembly scope record |
| D4.3 | Platform assembly boundary and interface review | Adapter contract and implementation review |
| D4.4 | Source hygiene and maintenance review | Source-review result |
| D4.5 | Naming/collision review across configuration and public APIs | Whole-project identifier review |
| D4.6 | Exact numerical types, strings and hosted exceptions | Policy-check result plus essential-type review |
| D4.7 | Error handling and caller obligations review | Call-site/dataflow review |
| D4.8 | Public implementation exposure and API coupling review | Public API design record |
| D4.9 | Macro-versus-function rationale and evaluation review | Macro inventory and source findings |
| D4.10 | Independent/repeated-header inclusion review | Header checks and preprocessor report |
| D4.11 | Standard-library argument/extent validation review | Library-call inventory and bounds evidence |
| D4.12 | Storage and runtime reservation policy review | Whole-product allocator audit and scoped deviation if needed |
| D4.13 | Object/resource state and lifetime sequence review | Ownership/state-machine record |
| D4.14 | External-input boundaries and adversarial testing | Threat/parser validation report |
| D4.15 | Floating-point applicability and numerical-failure review | Not-applicable evidence or approved numerical-module analysis |
| D5.1 | Execution-context map and shared-state review | Ownership/ordering evidence and applicable race-tool results |
| D5.2 | Lock graph and cancellation/failure behavior review | Not-applicable evidence or synchronization analysis |
| D5.3 | Task/thread startup and resource-creation review | Target execution/resource inventory |

## Rule review assignments

`R<n>` assigns initial methods to every row in rule family `<n>`. Each individual
rule still needs authoritative interpretation and a separate disposition; the family
assignment never replaces that work.

| Group | Initial verification methods | Record focus |
| --- | --- | --- |
| R1 | Production compiler, semantic analyzer and target review | Language mode, extensions, unsupported behavior and undefined behavior |
| R2 | Whole-program source/dataflow and symbol analysis | Source reachability, use and diagnostics |
| R3 | Lexical analysis plus manual maintenance review | Source token/line representation |
| R4 | Literal/encoding scanner plus target review | Character encoding and literal interpretation |
| R5 | Whole-program name/link/configuration review | Public/internal identifiers and uniqueness |
| R6 | Declaration scanner plus ABI/layout review | Specialized field declarations and representation |
| R7 | Literal and essential-type analyzer plus bounds review | Constants, conversions and string-literal usage |
| R8 | Compiler/declaration analyzer plus API review | Linkage, definitions, qualifiers and declarations |
| R9 | Initialization/dataflow analyzer and boundary tests | Initial state and aggregate/object initialization |
| R10 | Configured essential-type analysis and manual numerical proof | Promotions, arithmetic, mixing and conversions |
| R11 | Semantic pointer/qualifier analyzer and target/lifetime review | Pointer provenance, conversions and null handling |
| R12 | Expression/shift analyzer and boundary tests | Evaluation structure and numerical bounds |
| R13 | Side-effect/dataflow analysis plus API call review | Evaluation order and observable effects |
| R14 | Control-flow analyzer and documented loop bounds | Predicates, counters and iteration contracts |
| R15 | Control-flow/resource-lifetime review | Branches, exits and cleanup behavior |
| R16 | Switch analyzer and state-machine tests | Case coverage and fallthrough/default behavior |
| R17 | Call-graph, function-contract and arithmetic review | Calls, recursion/variadics and argument behavior |
| R18 | Extent/lifetime/pointer analysis and boundary testing | Arrays, indexing and object limits |
| R19 | Storage/alias/representation review | Overlap and alternate storage interpretations |
| R20 | Full-configuration preprocessor review | Include/macro/conditional configuration behavior |
| R21 | Standard-library/dependency inventory and semantic review | Approved headers, APIs, arguments and failure behavior |
| R22 | Whole-system resource-state/dataflow review | Standard resources and release/error paths |
| R23 | Selected-dialect generic-selection analysis | C mode and configuration-dependent generic expressions |

These methods must be refined per guideline in the evidence record. Tool coverage
must identify analyzer version, edition support, enabled rule IDs and limitations.
No claim of full coverage is made for `tools/check_policy.py` or earlier Cppcheck
runs. Base headers supporting multiple dialects may need multiple assessments.

## Known initial obligations

- Runtime pool acquire/release has an unresolved D4.12 interpretation/deviation need.
- Single-context ownership requires integration evidence for D5.1, not a concurrency claim.
- Base compiler probes, token concatenation, `restrict` and newer-dialect macros need actual configuration review.
- Exact-width type names do not prove compliance of essential-type arithmetic.
- Tests rewritten for the replacement pool have not run; earlier allocator results are obsolete for it.

An edition migration creates a new baseline and a reviewed comparison of identifiers,
categories, interpretations and deviations. Keep the previous matrix with its
release record; do not relabel it as another edition.
