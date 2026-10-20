#!/usr/bin/env python3
"""Check HPS's limited source restrictions and governance records without a C build.

This lexical checker is not a C parser, MISRA analyzer or compliance assessment.
It does not compile, run C programs, access the network or change project files.
Optional self-tests create and remove isolated temporary fixtures.
"""
import argparse
import ast
import csv
import json
from pathlib import Path
import re
import shutil
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
TOKENS_TO_IGNORE = re.compile(
    r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\[\s\S]|[^"\\])*"|\'(?:\\[\s\S]|[^\'\\])*\'')
BASIC_TYPES = re.compile(r'\b(?:short|int|long|signed|unsigned|float|double|_Bool|bool)\b')
NONEXACT_TYPES = re.compile(
    r'\b(?:u?int_(?:least|fast)(?:8|16|32|64)_t|u?intmax_t|u?intptr_t)\b')
CHAR_STORAGE = re.compile(r'\bchar\b(?!\s*(?:const\s+)?\*)')
ALLOCATION = re.compile(
    r'\b(?:malloc|calloc|realloc|free|aligned_alloc|mmap|munmap|sbrk|brk)\s*\(')
UNSAFE_API = re.compile(
    r'\b(?:gets|strcpy|strcat|sprintf|vsprintf|scanf|fscanf|sscanf|vscanf|vfscanf|vsscanf)\s*\(')
HOSTED_MAIN = re.compile(r'\bint\s+main\s*\(\s*void\s*\)')
LINKS = re.compile(r'\[[^\]]*\]\(([^)]+)\)')
REQUIREMENT = re.compile(r'^(HPS-[A-Z]+-\d{3}):', re.MULTILINE)
MATRIX_FIELDS = ['kind', 'guideline_id', 'category', 'scope', 'method_group',
                 'owner_role', 'status', 'evidence', 'deviation']
STATUSES = {'unassessed', 'open', 'satisfied', 'deviation-approved', 'not-applicable'}
# Separately built, default-disabled hosted adapter allowed by HPS-PRO-002.
# This admits its stdio include only; numerical/heap/concurrency rules still run.
HOSTED_STDIO_SOURCES = {'src/platform/hosted/debug.c'}
HOSTED_DEBUG_HEADER = 'include/hps/platform/hosted/debug.h'
# HPS-C-020: standard names are declared only by their compatibility provider.
# This does not exempt other statements, APIs, heap calls or headers in a file.
STANDARD_PROVIDERS = {
    'stdint': 'include/hps/base/hps_stdint.h',
    'stddef': 'include/hps/base/hps_stddef.h',
    'stdbool': 'include/hps/base/hps_bool.h',
}
STANDARD_TYPE_FIXTURES = {
    'tests/base/test_standard_types.c',
    'tests/base/check_standard_types.py',
}
BOOL_SURFACES = {STANDARD_PROVIDERS['stdbool']} | STANDARD_TYPE_FIXTURES
STANDARD_INTEGER_NAMES = (
    r'(?:u?int(?:8|16|32|64)_t|u?int_(?:least|fast)(?:8|16|32|64)_t|'
    r'u?intmax_t|u?intptr_t)')
STANDARD_INTEGER_TYPEDEF = re.compile(
    r'^[ \t]*typedef[ \t]+(?:__[A-Z0-9_]+_TYPE__|'
    r'(?:(?:signed|unsigned)[ \t]+)?(?:char|short|int|long(?:[ \t]+long)?|__int64)|'
    + STANDARD_INTEGER_NAMES + r')[ \t]+' + STANDARD_INTEGER_NAMES + r'[ \t]*;',
    re.MULTILINE)


def blank_lexical(match):
    return ''.join('\n' if char == '\n' else ' ' for char in match.group())


def source_findings(source, path, hosted=False):
    """Return lexical findings; string/character literals and comments are ignored."""
    clean = TOKENS_TO_IGNORE.sub(blank_lexical, source)
    if path == STANDARD_PROVIDERS['stdint']:
        clean = STANDARD_INTEGER_TYPEDEF.sub(blank_lexical, clean)
    if path in BOOL_SURFACES:
        clean = re.sub(r'\bbool\b', lambda match: ' ' * len(match.group()), clean)
    if path in STANDARD_TYPE_FIXTURES:
        clean = NONEXACT_TYPES.sub(blank_lexical, clean)
    hosted_dependency_pattern = None
    if hosted:
        clean = HOSTED_MAIN.sub(blank_lexical, clean)
    patterns = [
        (BASIC_TYPES, 'HPS-C-001: basic numerical type'),
        (NONEXACT_TYPES, 'HPS-C-001/002: nonapproved integer type'),
        (CHAR_STORAGE, 'HPS-C-003: char storage must be a C-string pointer'),
        (ALLOCATION, 'HPS-MEM-001: heap/OS allocation API'),
        (UNSAFE_API, 'HPS-C-003/HPS-SEC-002: unbounded input/string API'),
        (re.compile(r'\b(?:enum|union)\b'), 'HPS-C-004/010: numerical enum or union storage'),
        (re.compile(r'\b(?:setjmp|longjmp)\s*\('), 'HPS-C-014: nonlocal control transfer'),
    ]
    if path.startswith('include/') or (path.startswith('src/') and path != 'src/main.c'):
        forbidden_headers = 'pthread|threads|stdatomic|unistd'
        if path not in HOSTED_STDIO_SOURCES:
            forbidden_headers += '|stdio'
        patterns.extend([
            (re.compile(r'\b(?:pthread_\w+|thrd_\w+|atomic_\w+)\s*\('),
             'HPS-MEM-005: concurrency API in current single-context core'),
            (re.compile(r'#\s*include\s*<\s*(?:' + forbidden_headers + r')\.h\s*>'),
             'HPS-PRO-002/HPS-MEM-005: platform/concurrency/I/O dependency in core'),
        ])
        if (path not in HOSTED_STDIO_SOURCES) and (path != HOSTED_DEBUG_HEADER):
            hosted_dependency_pattern = re.compile(
                r'^[ \t]*#\s*include\s*[<"]hps/platform/hosted/[^>"\n]+[>"]',
                re.MULTILINE)
    findings = []
    for pattern, message in patterns:
        for match in pattern.finditer(clean):
            line = clean.count('\n', 0, match.start()) + 1
            findings.append(f'{path}:{line}: {message}: {match.group().strip()}')
    if hosted_dependency_pattern is not None:
        # Preserve include strings; only comments are blanked for this scan.
        include_code = TOKENS_TO_IGNORE.sub(
            lambda match: blank_lexical(match) if match.group().startswith(('//', '/*'))
            else match.group(), source)
        for match in hosted_dependency_pattern.finditer(include_code):
            line = include_code.count('\n', 0, match.start()) + 1
            findings.append(f'{path}:{line}: HPS-PRO-002: hosted adapter dependency in core: '
                            f'{match.group().strip()}')
    if path.startswith(('include/', 'src/')):
        # Preserve quoted include names while ignoring explanatory comments.
        include_code = TOKENS_TO_IGNORE.sub(
            lambda match: blank_lexical(match) if match.group().startswith(('//', '/*'))
            else match.group(), source)
        for match in re.finditer(
                r'^[ \t]*#\s*include\s*[<"](stdint|stddef|stdbool)\.h[>"]',
                include_code, re.MULTILINE):
            if path != STANDARD_PROVIDERS[match.group(1)]:
                line = include_code.count('\n', 0, match.start()) + 1
                findings.append(f'{path}:{line}: HPS-HDR-003/HPS-C-020: '
                                f'use the Base standard-type provider: {match.group().strip()}')
    return findings


def header_guard(public_path):
    return re.sub(r'[^A-Za-z0-9]', '_', public_path).upper()


def header_findings(source, public_path):
    """Validate the documented header shape; this is not a declaration parser."""
    guard = header_guard(public_path)
    # Keep string literals (including the linkage spelling), blank comments only.
    code = TOKENS_TO_IGNORE.sub(
        lambda match: blank_lexical(match) if match.group().startswith(('//', '/*'))
        else match.group(), source)
    lines = [line.strip() for line in code.splitlines() if line.strip()]
    findings = []
    if lines[:2] != ['#ifndef ' + guard, '#define ' + guard]:
        findings.append(f'{public_path}: HPS-HDR-001: missing/mismatched path guard')
    if not source.rstrip().endswith('#endif /* ' + guard + ' */'):
        findings.append(f'{public_path}: HPS-HDR-001: missing closing guard annotation')
    if not re.search(r'@file\s+' + re.escape(public_path) + r'(?=\s|$)', source) or not re.search(
            r'@brief[ \t]+\S', source):
        findings.append(f'{public_path}: HPS-HDR-002: missing file identity or purpose')
    if re.search(r'#\s*include\s*[<"][^>"\n]+\.c[>"]', code):
        findings.append(f'{public_path}: HPS-HDR-003: implementation inclusion')
    declarations = list(re.finditer(
        r'^[ \t]*(?:extern[ \t]+)?[A-Za-z_]\w*(?:[ \t]+|[ \t]*\*+[ \t]*)'
        r'hps_\w+\s*\([^;{}]*\)\s*;', code, re.MULTILINE))
    declarations += list(re.finditer(r'^\s*extern\s+[^";{}]+;', code, re.MULTILINE))
    declarations += list(re.finditer(
        r'\btypedef\s+[^;{}]+\(\s*\*\s*hps_\w+\s*\)[^;{}]*;', code))
    openings = list(re.finditer(
        r'#ifdef[ \t]+__cplusplus\s*\n[ \t]*extern[ \t]+"C"[ \t]*\{[ \t]*\n'
        r'[ \t]*#endif', code))
    closings = list(re.finditer(
        r'#ifdef[ \t]+__cplusplus\s*\n[ \t]*\}[ \t]*\n[ \t]*#endif', code))
    if declarations or re.search(r'\bextern\s+"C"', code):
        if len(openings) != 1 or len(closings) != 1 or openings[0].end() >= closings[0].start():
            findings.append(f'{public_path}: HPS-HDR-004: missing guarded C linkage block')
        else:
            start, end = openings[0].end(), closings[0].start()
            if any(not (start <= declaration.start() < end) for declaration in declarations):
                findings.append(f'{public_path}: HPS-HDR-004: external declaration outside C linkage')
            if re.search(r'^\s*#\s*include\b', code[start:end], re.MULTILINE):
                findings.append(f'{public_path}: HPS-HDR-003: include inside C linkage')
    return findings


def check_sources(root):
    findings = []
    guards = set()
    for directory in ['include', 'src', 'tests']:
        for path in sorted((root / directory).rglob('*')):
            if path.is_file() and path.suffix in {'.h', '.c'}:
                relative = path.relative_to(root).as_posix()
                hosted = relative == 'src/main.c' or (
                    relative.startswith('tests/') and path.suffix == '.c')
                findings.extend(source_findings(path.read_text(), relative, hosted))
                if directory == 'include' and path.suffix == '.h':
                    public = path.relative_to(root / 'include').as_posix()
                    guard = header_guard(public)
                    if guard in guards:
                        findings.append(f'{relative}: HPS-HDR-001: duplicate header guard')
                    guards.add(guard)
                    findings.extend(header_findings(path.read_text(), public))
    # Literal C snippets generated by Python tests receive the hosted exception.
    # Dynamically constructed snippets still need compiler/analyzer review.
    for path in sorted((root / 'tests').rglob('*.py')):
        tree = ast.parse(path.read_text())
        for node in ast.walk(tree):
            if isinstance(node, ast.Constant) and isinstance(node.value, str):
                value = node.value
                if '#include' in value or re.search(r'\bmain\s*\(', value):
                    findings.extend(source_findings(
                        '\n' * (node.lineno - 1) + value,
                        path.relative_to(root).as_posix(), hosted=True))
    # Check actual C examples, not explanatory prose about forbidden spellings.
    documents = [root / 'README.md'] + sorted((root / 'docs').rglob('*.md'))
    for path in documents:
        for match in re.finditer(r'^```c\s*\n([\s\S]*?)^```', path.read_text(), re.MULTILINE):
            prefix = '\n' * path.read_text()[:match.start(1)].count('\n')
            findings.extend(source_findings(prefix + match.group(1),
                                            path.relative_to(root).as_posix()))
    return findings


def within_root_file(root, value):
    if not value or Path(value).is_absolute():
        return False
    candidate = (root / value).resolve()
    return candidate.is_relative_to(root.resolve()) and candidate.is_file()


def check_governance(root):
    findings = []
    directory = root / 'governance'
    baseline = json.loads((directory / 'misra/baseline.json').read_text())
    expected = {('Dir', identity) for identity in baseline['directives']}
    expected.update(('Rule', identity) for identity in baseline['rules'])
    if (baseline['edition'] != 'MISRA C:2023' or len(baseline['directives']) != 21
            or len(baseline['rules']) != 200 or len(expected) != 221):
        findings.append('governance/misra/baseline.json: invalid pinned baseline inventory')
    plan = (directory / 'misra/README.md').read_text()
    groups = set(re.findall(r'\|\s*(D\d+\.\d+|R\d+)\s*\|', plan))
    with (directory / 'misra/matrix.csv').open(newline='') as stream:
        reader = csv.DictReader(stream)
        if reader.fieldnames != MATRIX_FIELDS:
            findings.append('governance/misra/matrix.csv: incorrect column schema')
        rows = list(reader)
    seen = set()
    for number, row in enumerate(rows, 2):
        location = f'governance/misra/matrix.csv:{number}'
        identity = (row.get('kind'), row.get('guideline_id'))
        if identity in seen or identity not in expected:
            findings.append(f'{location}: duplicate or nonbaseline guideline {identity}')
        seen.add(identity)
        expected_category = 'Required'
        if identity[0] == 'Rule' and identity[1] in baseline['mandatory_rules']:
            expected_category = 'Mandatory'
        elif (identity[0] == 'Rule' and identity[1] in baseline['advisory_rules']) or (
                identity[0] == 'Dir' and identity[1] in baseline['advisory_directives']):
            expected_category = 'Advisory'
        if row.get('category') != expected_category:
            findings.append(f'{location}: category differs from pinned baseline')
        expected_method = ('D' + str(identity[1]) if identity[0] == 'Dir'
                           else 'R' + str(identity[1]).split('.')[0])
        if (row.get('method_group') not in groups or row.get('method_group') != expected_method
                or not row.get('scope') or not row.get('owner_role')):
            findings.append(f'{location}: missing scope, review assignment or owner')
        status = row.get('status')
        if status not in STATUSES:
            findings.append(f'{location}: invalid assessment status')
        if status in {'satisfied', 'not-applicable', 'deviation-approved'}:
            if not within_root_file(root, row.get('evidence', '')):
                findings.append(f'{location}: disposition requires an existing evidence record')
        if status == 'deviation-approved':
            if row.get('category') == 'Mandatory':
                findings.append(f'{location}: mandatory guideline cannot have an approved deviation')
            if not within_root_file(root, row.get('deviation', '')):
                findings.append(f'{location}: missing deviation record')
    if seen != expected:
        findings.append('governance/misra/matrix.csv: incomplete baseline identifier coverage')

    documents = sorted(directory.rglob('*.md')) + [root / name for name in
                                                  ['README.md', 'AGENTS.md', 'CONTRIBUTING.md']]
    documents.extend(sorted((root / 'docs').rglob('*.md')))
    requirement_ids = set()
    for path in documents:
        source = path.read_text()
        for identity in REQUIREMENT.findall(source):
            if identity in requirement_ids:
                findings.append(f'{path.relative_to(root)}: duplicate requirement {identity}')
            requirement_ids.add(identity)
        for match in LINKS.finditer(source):
            target = match.group(1).strip().strip('<>')
            if re.match(r'^[a-zA-Z][a-zA-Z0-9+.-]*:', target) or target.startswith('#'):
                continue
            target = target.split('#', 1)[0]
            if target and not (path.parent / target).exists():
                findings.append(f'{path.relative_to(root)}: missing local link target {target}')
    if not requirement_ids:
        findings.append('governance: no identifiable project requirements')
    return findings, len(rows), len(requirement_ids)


def self_test():
    cases = [
        ('uint32_t n; size_t count; const char *label;', False, False),
        ('char *label;', False, False),
        ('char const *label;', False, False),
        ('unsigned char flag;', False, True),
        ('int32_t n; uint_least32_t generation;', False, True),
        ('char bytes[8];', False, True),
        ('/* int n; malloc(4); */ uint8_t value;', False, False),
        ('const char *label = "unsigned int; malloc(1)";', False, False),
        ('uint8_t value = \'x\';', False, False),
        ('int main(void) { return 0; }', True, False),
        ('int main(void) { int value; return 0; }', True, True),
        ('int main(void) { return 0; }', False, True),
        ('void *p = malloc(16U);', False, True),
        ('strcpy(destination, source);', False, True),
        ('typedef enum { VALUE } status;', False, True),
        ('#include <pthread.h>', False, True),
    ]
    for source, hosted, expected in cases:
        actual = bool(source_findings(source, 'include/test.h', hosted))
        if actual != expected:
            raise RuntimeError(f'Checker self-test failed for {source!r}')
    scope_cases = [
        ('#include <stdio.h>', 'src/bytes/buffer.c', True),
        ('atomic_load(value);', 'src/bytes/view.c', True),
        ('#include <stdio.h>', 'src/main.c', False),
        ('#include <stdio.h>', 'src/platform/hosted/debug.c', False),
        ('#include <stdio.h>', 'src/platform/hosted/other.c', True),
        ('#include <stdio.h>', HOSTED_DEBUG_HEADER, True),
        ('#include <unistd.h>', 'src/platform/hosted/debug.c', True),
        ('#include <stdatomic.h>', 'src/platform/hosted/debug.c', True),
        ('malloc(1U);', 'src/platform/hosted/debug.c', True),
        ('int value;', 'src/platform/hosted/debug.c', True),
        ('pthread_lock(value);', 'src/platform/hosted/debug.c', True),
        ('#include "hps/platform/hosted/debug.h"', 'src/bytes/buffer.c', True),
        ('#include <hps/platform/hosted/debug.h>', 'include/hps/base/all.h', True),
        ('#include "hps/platform/hosted/debug.h"', 'src/platform/hosted/debug.c', False),
    ]
    for source, path, expected in scope_cases:
        if bool(source_findings(source, path)) != expected:
            raise RuntimeError(f'Implementation-scope self-test failed: {path}')
    print(f'Implementation-scope self-tests: {len(scope_cases)} cases passed.')
    provider_cases = [
        ('typedef __INT_FAST16_TYPE__ int_fast16_t;', STANDARD_PROVIDERS['stdint'], False),
        ('typedef unsigned char uint8_t;', STANDARD_PROVIDERS['stdint'], False),
        ('typedef unsigned char private_byte;', STANDARD_PROVIDERS['stdint'], True),
        ('typedef unsigned char uint8_t;', 'include/hps/bytes/view.h', True),
        ('uint_fast16_t count;', STANDARD_PROVIDERS['stdint'], True),
        ('uint_fast16_t count;', 'tests/base/check_standard_types.py', False),
        ('uint_fast16_t count;', 'tests/bytes/test_bytes.c', True),
        ('int count;', STANDARD_PROVIDERS['stdint'], True),
        ('bool value;', STANDARD_PROVIDERS['stdbool'], False),
        ('bool value;', 'include/hps/bytes/view.h', True),
        ('unsigned char value;', STANDARD_PROVIDERS['stdbool'], True),
        ('malloc(1U);', STANDARD_PROVIDERS['stdint'], True),
        ('#include <stdint.h>', STANDARD_PROVIDERS['stdint'], False),
        ('#include <stddef.h>', STANDARD_PROVIDERS['stdint'], True),
        ('#include "stdint.h"', 'src/bytes/view.c', True),
        ('#include "hps/base/hps_stdint.h"', 'src/bytes/view.c', False),
        ('#include <stdint.h>', 'tests/base/check_standard_types.py', False),
        ('/* #include <stdint.h> */', 'include/hps/bytes/view.h', False),
    ]
    for source, path, expected in provider_cases:
        if bool(source_findings(source, path)) != expected:
            raise RuntimeError(f'Standard-provider scope self-test failed: {path}: {source}')
    print(f'Standard-provider self-tests: {len(provider_cases)} cases passed.')
    if within_root_file(ROOT, '/etc/passwd') or within_root_file(ROOT, '../outside'):
        raise RuntimeError('Evidence path containment self-test failed')
    print(f'Policy checker self-tests: {len(cases)} lexical cases passed.')

    prototype = 'uint8_t hps_test_operation(void);'
    opening = '#ifdef __cplusplus\nextern "C" {\n#endif\n'
    closing = '#ifdef __cplusplus\n} /* extern "C" */\n#endif\n'
    prefix = ('#ifndef HPS_TEST_H\n#define HPS_TEST_H\n'
              '/** @file hps/test.h\n * @brief Test header. */\n#include <stdint.h>\n')
    suffix = '#endif /* HPS_TEST_H */\n'
    valid = prefix + opening + prototype + '\n' + closing + suffix
    header_cases = [
        (valid, False),
        (prefix + '#define HPS_TEST_VALUE 1U\n' + suffix, False),
        (valid.replace('#define HPS_TEST_H', '#define HPS_WRONG_H'), True),
        (valid.replace('@file hps/test.h', '@file hps/wrong.h'), True),
        (prefix + prototype + '\n' + suffix, True),
        (prefix + opening + closing + prototype + '\n' + suffix, True),
        (valid.replace(prototype, '#include <stddef.h>\n' + prototype), True),
        (valid.replace(closing, ''), True),
    ]
    for source, expected in header_cases:
        if bool(header_findings(source, 'hps/test.h')) != expected:
            raise RuntimeError('Public-header checker self-test failed')
    print(f'Header self-tests: {len(header_cases)} positive/negative cases passed.')

    with tempfile.TemporaryDirectory(prefix='hps-policy-check-') as temporary:
        fixture = Path(temporary)
        shutil.copytree(ROOT / 'governance', fixture / 'governance')
        shutil.copytree(ROOT / 'docs', fixture / 'docs')
        for name in ['README.md', 'AGENTS.md', 'CONTRIBUTING.md']:
            shutil.copy2(ROOT / name, fixture / name)
        matrix = fixture / 'governance/misra/matrix.csv'
        with matrix.open(newline='') as stream:
            original = list(csv.DictReader(stream))
        alterations = ['missing', 'duplicate', 'category', 'assignment',
                       'missing_evidence', 'mandatory_deviation']
        for alteration in alterations:
            rows = [dict(row) for row in original]
            if alteration == 'missing':
                rows.pop()
            elif alteration == 'duplicate':
                rows.append(dict(rows[0]))
            elif alteration == 'category':
                rows[0]['category'] = ('Advisory' if rows[0]['category'] != 'Advisory'
                                       else 'Required')
            elif alteration == 'assignment':
                rows[0]['method_group'] = 'R1'
            elif alteration == 'missing_evidence':
                rows[0]['status'] = 'satisfied'
                rows[0]['evidence'] = ''
            else:
                mandatory = next(row for row in rows if row['category'] == 'Mandatory')
                mandatory['status'] = 'deviation-approved'
                mandatory['evidence'] = 'README.md'
                mandatory['deviation'] = 'CONTRIBUTING.md'
            with matrix.open('w', newline='') as stream:
                writer = csv.DictWriter(stream, fieldnames=MATRIX_FIELDS)
                writer.writeheader()
                writer.writerows(rows)
            if not check_governance(fixture)[0]:
                raise RuntimeError(f'Governance self-test failed: {alteration}')
    print(f'Governance self-tests: {len(alterations)} invalid record cases rejected.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--self-test', action='store_true', help='also validate checker behavior')
    parser.add_argument('--require-assessed', action='store_true',
                        help='reject unassessed/open MISRA rows; does not certify compliance')
    arguments = parser.parse_args()
    if arguments.self_test:
        self_test()
    try:
        findings = check_sources(ROOT)
        governance_findings, guidelines, requirements = check_governance(ROOT)
        findings.extend(governance_findings)
        if arguments.require_assessed:
            with (ROOT / 'governance/misra/matrix.csv').open(newline='') as stream:
                unresolved = sum(row['status'] in {'unassessed', 'open'}
                                 for row in csv.DictReader(stream))
            if unresolved:
                findings.append(f'Qualified-release record gate: {unresolved} MISRA dispositions remain unresolved')
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(f'Policy check failed: {error}', file=sys.stderr)
        return 1
    if findings:
        for finding in findings:
            print(finding, file=sys.stderr)
        return 1
    print(f'HPS policy subset passed: {guidelines} MISRA identifiers; '
          f'{requirements} project requirement IDs. No C build/tests performed.')
    print('Implementation compliance, evidence approval and target qualification are not assessed.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
