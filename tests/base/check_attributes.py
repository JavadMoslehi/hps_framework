#!/usr/bin/env python3
"""Compile and exercise enabled/disabled annotation contracts on the host."""
import argparse
from pathlib import Path
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler', nargs='+', default=['cc'])
args = parser.parse_args()
COMPILER = [part for value in args.compiler for part in shlex.split(value)]
checks = 0


def compile_case(source, dialect='c99', enabled=0, fail=False, run=False):
    global checks
    with tempfile.TemporaryDirectory(prefix='hps-attribute-') as temporary:
        target = Path(temporary) / 'check'
        language = 'c++' if dialect.startswith('c++') else 'c'
        command = COMPILER + ['-x', language, '-std=' + dialect, '-Wall', '-Wextra',
                              '-Werror', '-pedantic-errors', '-O0', '-I',
                              str(ROOT / 'include'),
                              '-DHPS_ENABLE_COMPILER_ATTRIBUTES=' + str(enabled)]
        command += ['-', '-o', str(target)] if run else ['-fsyntax-only', '-']
        result = subprocess.run(command, input=source, text=True, capture_output=True)
        if (result.returncode != 0) != fail:
            raise RuntimeError(' '.join(command) + '\n' + result.stderr)
        if fail and 'nonnull' not in result.stderr:
            raise RuntimeError('Expected nonnull contract diagnostic:\n' + result.stderr)
        if run:
            subprocess.run([str(target)], check=True)
        checks += 1


source = '''#include "hps/base/all.h"
#include <stdint.h>
#if defined(HPS_PACKED) || defined(HPS_ALIGNED)
#error "ABI-changing declaration attributes must not be silently imported"
#endif
#if !HPS_ENABLE_COMPILER_ATTRIBUTES && (HPS_HAS_ALWAYS_INLINE || HPS_HAS_NEVER_INLINE || HPS_HAS_USED || HPS_HAS_PURE || HPS_HAS_CONSTFUNC || HPS_HAS_NONNULL)
#error "Disabled annotations must not claim extension guarantees"
#endif
static HPS_ALWAYS_INLINE int32_t inline_value(int32_t value) { return value; }
HPS_NEVER_INLINE static int32_t separate_value(int32_t value) { return value; }
HPS_USED static const uint32_t retained = UINT32_C(7);
HPS_UNUSED_DECL static const uint32_t declaration = UINT32_C(9);
HPS_MAYBE_UNUSED static const uint32_t maybe = UINT32_C(11);
HPS_PURE static int32_t read_value(const int32_t *value) { return *value; }
HPS_CONSTFUNC static int32_t twice(int32_t value) { return value * INT32_C(2); }
HPS_NONNULL(1) static int32_t required_pointer(const int32_t *value) { return *value; }
int main(void)
{
    const int32_t value = INT32_C(3);
    HPS_UNUSED(retained);
    HPS_UNUSED(declaration);
    HPS_UNUSED(maybe);
    if (inline_value(value) != value || separate_value(value) != value) { return 1; }
    if (read_value(&value) != value || required_pointer(&value) != value) { return 2; }
    return (twice(value) == INT32_C(6)) ? 0 : 3;
}
'''
for dialect in ['c99', 'c11', 'c17', 'c2x']:
    for enabled in [0, 1]:
        compile_case(source, dialect, enabled, run=True)

subset = '''#include "hps/base/attributes.h"
#include "hps/base/macros.h"
#ifdef HPS_NONNULL
#error "Variadic annotation is absent from the legacy dialect subset"
#endif
typedef void (*hps_attribute_translation_unit)(void);
'''
for dialect in ['c89', 'c++98']:
    for enabled in [0, 1]:
        compile_case(subset, dialect, enabled)

nonnull = '''#include "hps/base/attributes.h"
#include <stdint.h>
HPS_NONNULL(1) int32_t hps_required(const int32_t *value);
int main(void) { return hps_required(NULL) == INT32_C(0) ? 0 : 1; }
'''
compile_case(nonnull, enabled=0)
compile_case(nonnull, enabled=1, fail=True)

without_probes = '''#include "hps/base/compiler.h"
#undef HPS_HAS_ATTRIBUTE
#define HPS_HAS_ATTRIBUTE(name) 0
#include "hps/base/attributes.h"
#if HPS_HAS_ALWAYS_INLINE || HPS_HAS_NEVER_INLINE || HPS_HAS_USED || HPS_HAS_PURE || HPS_HAS_CONSTFUNC || HPS_HAS_NONNULL
#error "No extension annotation may be claimed without its probe"
#endif
typedef void (*hps_attribute_translation_unit)(void);
'''
compile_case(without_probes, enabled=1)
print(f'HPS annotations: {checks} checks passed ({" ".join(COMPILER)}).')
