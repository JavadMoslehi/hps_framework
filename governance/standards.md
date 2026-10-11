# Standards and reference register

Reference review date: 2026-10-11. Editions are deliberately pinned; the register
is not a statement that every entry is the newest publication. Each integrating
product must confirm the applicable edition, amendments and contractual scope.
A reference motivates HPS requirements; only an approved target assessment can
establish a scoped external-standard result.

| Reference | Project use | Current evidence/status |
| --- | --- | --- |
| MISRA C:2023 | Coding baseline; all-guideline identifier index | 221 identifier/category records checked against the public index; all implementation outcomes unassessed |
| MISRA Compliance:2020 | Enforcement, deviations and compliance-summary process | Governance plan written; implementation assessment incomplete |
| ISO C99 | Initial core language mode | Source contracts only for the replacement pool; target qualification pending |
| NIST SP 800-218 SSDF v1.1 | Secure lifecycle reference | HPS security requirements proposed; SSDF implementation not assessed |
| ISO 26262 family | Automotive integration assessment selected by safety plan | No item definition, allocated ASIL, safety case or assessment |
| PCI DSS / PCI Secure Software / Secure SLC | Payment-specific selection by assessor/integrator | No payment product or PCI validation; editions/scope unselected |
| Applicable 3GPP / ETSI specifications | Actual telecom protocol/network function selection | No protocol/release/interface selected or assessed |

## Primary sources

- [MISRA's C:2023 public guideline index](https://gitlab.com/MISRA/MISRA-C/MISRA-C-2012/tools/-/raw/main/misra_c_2023__headlines_for_cppcheck.txt): identifier/category facts used for matrix completeness. No guideline descriptions, examples or licensed full text are reproduced here.
- [MISRA Compliance:2020](https://www.misra.org.uk/app/uploads/2021/06/MISRA-Compliance-2020.pdf): source for the compliance-process distinction.
- [WG14 C draft N843, hosted startup section](https://open-std.org/jtc1/sc22/wg14/www/docs/n843.htm): background for the `int main(void)` entry-point exception; production qualification must use the selected authoritative C edition.
- [C++ working draft, language linkage](https://eel.is/c++draft/dcl.link): reference for guarded C API declarations; C++ ABI support remains unqualified.
- [NIST SSDF v1.1](https://csrc.nist.gov/pubs/sp/800/218/final): secure-development lifecycle reference.
- [ISO 26262 overview](https://www.iso.org/publication/PUB200262.html): functional-safety lifecycle scope.
- [PCI standards](https://www.pcisecuritystandards.org/standards/), [PCI DSS](https://www.pcisecuritystandards.org/standards/pci-dss/) and [Secure SLC](https://www.pcisecuritystandards.org/standards/secure-software-lifecycle-secure-slc/): assessment-specific source material.
- [ETSI NFV program](https://www.etsi.org/technical-groups/nfv/): an example of a telecom standards family requiring specific scope selection.

Use appropriately licensed authoritative standards during reviews. Public descriptions
and metadata establish the index/scope, not the complete normative interpretation.
Track edition migrations separately: compare every identifier, amendment, category,
tool capability and previously approved deviation. Do not update a version label
without revalidating its evidence.
