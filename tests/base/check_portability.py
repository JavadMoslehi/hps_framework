#!/usr/bin/env python3
"""Compile independent headers and exercise detection/failure contracts.

Synthetic macro tests exercise branches only; they do not validate foreign ABIs.
No binaries compiled with synthetic target macros are run.
"""
import argparse
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler', nargs='+', default=[os.environ.get('CC', 'cc')])
args = parser.parse_args()
CC = [part for item in args.compiler for part in shlex.split(item)]
checks = 0


def compile_source(source, standard='c99', flags=(), fail=False, diagnostic=None,
                   run=False, headers=True, codegen=False):
    global checks
    with tempfile.TemporaryDirectory(prefix='hps-base-test-') as directory:
        output = Path(directory) / 'test'
        command = CC + ['-std=' + standard, '-Wall', '-Wextra', '-Werror',
                        '-pedantic-errors', '-I', str(ROOT / 'include'),
                        '-I', str(ROOT / 'tests/base')]
        if not headers:
            command += ['-undef']
        command += list(flags)
        if run:
            command += ['-x', 'c', '-', '-o', str(output)]
        elif codegen:
            command += ['-x', 'c', '-', '-c', '-o', str(output)]
        else:
            command += ['-x', 'c', '-', '-fsyntax-only']
        # Keep macro-only translation units nonempty without host-library headers
        # that could interpret synthetic language/target macros as real ABI data.
        result = subprocess.run(command, input=source + '\ntypedef void (*hps_test_translation_unit)(void);\n',
                                text=True, capture_output=True)
        if (result.returncode != 0) != fail:
            raise RuntimeError(' '.join(command) + '\n' + result.stderr + '\n' + source)
        if diagnostic and diagnostic not in result.stderr:
            raise RuntimeError('Expected diagnostic: ' + diagnostic + '\n' + result.stderr)
        if run:
            subprocess.run([str(output)], check=True)
    checks += 1


def expect(header, assertions, flags=(), standard='c99', headers=True):
    source = '#include "' + header + '"\n'
    for expression in assertions:
        source += '#if !(' + expression + ')\n#error "incorrect detection"\n#endif\n'
    source += 'int main(void) { return 0; }\n'
    compile_source(source, standard, flags, headers=headers)


for dialect in ['c99', 'c11', 'c17', 'c2x']:
    for header in sorted((ROOT / 'include').rglob('*.h')):
        name = header.relative_to(ROOT / 'include').as_posix()
        compile_source('#include "' + name + '"\n#include "' + name + '"\n', dialect)
    base_source = (ROOT / 'tests/base/test_base.c').read_text()
    base_source += '\nint main(void) { return test_base(); }\n'
    compile_source(base_source, dialect, run=True)
    expect('hps/base/standard.h', ['HPS_C_IS_C99',
           'HPS_C_IS_C11 == (__STDC_VERSION__ >= 201112L)',
           'HPS_C_IS_C17 == (__STDC_VERSION__ >= 201710L)',
           'HPS_C_IS_C23 == (__STDC_VERSION__ >= 202311L)'], standard=dialect)

# Policy switches are explicit, validated configuration; probes are informational.
expect('hps/base/attributes.h', ['HPS_ENABLE_COMPILER_ATTRIBUTES == 0',
       'HPS_ENABLE_RESTRICT == 0', '!HPS_HAS_NODISCARD'], standard='c99')
for switch in ['HPS_ENABLE_COMPILER_ATTRIBUTES', 'HPS_ENABLE_RESTRICT']:
    compile_source('#include "hps/base/attributes.h"\n',
                   flags=['-D' + switch + '=2'], fail=True,
                   diagnostic=switch + ' must be 0 or 1')

# Detection/attributes remain usable in C89.
for header in ['language.h', 'standard.h', 'compiler.h', 'architecture.h',
               'features.h', 'attributes.h', 'version.h']:
    compile_source('#include "hps/base/' + header + '"\n', 'c89')
expect('hps/base/standard.h', ['HPS_C_LEGACY', '!HPS_C_IS_C99'], standard='c89')
compile_source('#define HPS_REQUIRE_C99\n#include "hps/base/standard.h"\n',
               'c89', fail=True, diagnostic='HPS requires C99')
compile_source('#include "hps/base/attributes.h"\nHPS_STATIC_ASSERT(0, "must fail");\n',
               'c99', fail=True)
compile_source('#include "hps/base/attributes.h"\nHPS_STATIC_ASSERT(0, "must fail");\n',
               'c11', fail=True, diagnostic='must fail')
compile_source('#include "hps/base/attributes.h"\nHPS_STATIC_ASSERT(1, "C89 assertion");\n',
               'c89')
compile_source('#include "hps/base/attributes.h"\nHPS_STATIC_ASSERT(0, "must fail");\n',
               'c89', fail=True)
compile_source('''#include "hps/base/attributes.h"
#include <stdint.h>
HPS_NODISCARD int32_t required_result(void);
int main(void) { required_result(); return 0; }
''', flags=['-DHPS_ENABLE_COMPILER_ATTRIBUTES=1'], fail=True, diagnostic='unused', codegen=True)
compile_source('''#include "hps/base/attributes.h"
#include <stdint.h>
HPS_DEPRECATED("use replacement") int32_t old_function(void);
int main(void) { return (old_function() == 0) ? 0 : 1; }
''', flags=['-DHPS_ENABLE_COMPILER_ATTRIBUTES=1'], fail=True, diagnostic='deprecated')
compile_source('''#include "hps/base/attributes.h"
HPS_NORETURN void never_returns(void) { for (;;) {} }
''')

for optional in ['VLA', 'COMPLEX', 'ATOMICS']:
    expect('hps/base/features/language.h', ['!HPS_HAS_' + optional],
           ['-D__STDC_NO_' + optional + '__=1'], 'c11')
expect('hps/base/language.h', ['HPS_HOSTED_IS_KNOWN', 'HPS_IS_FREESTANDING',
       '!HPS_IS_HOSTED'], ['-ffreestanding'])
compile_source('#include "hps/base/all.h"\n', flags=['-ffreestanding'])

# Pointer detection counts C bytes using CHAR_BIT.
with tempfile.TemporaryDirectory(prefix='hps-limits-test-') as directory:
    Path(directory, 'limits.h').write_text('#define CHAR_BIT 16\n')
    expect('hps/base/architecture/pointer.h',
           ['HPS_POINTER_BITS == 64', 'HPS_POINTER_WIDTH == HPS_POINTER_WIDTH_64'],
           ['-I', directory, '-U__SIZEOF_POINTER__', '-D__SIZEOF_POINTER__=4'])

# C++ identification must not advertise C standard features.
expect('hps/base/standard.h', ['HPS_LANGUAGE_IS_CPP', '!HPS_LANGUAGE_IS_C',
       '!HPS_C_LEGACY', '!HPS_C_IS_C99'], ['-D__cplusplus=201703L'], headers=False)

# Compiler precedence, including Clang's GCC/MSVC compatibility macros.
clang_version = ['-D__clang__=1', '-D__clang_major__=14',
                 '-D__clang_minor__=0', '-D__clang_patchlevel__=1']
for definitions, expected in [
    ([], 'UNKNOWN'),
    (['-D__GNUC__=11', '-D__GNUC_MINOR__=4'], 'GCC'),
    (['-D_MSC_VER=1930'], 'MSVC'),
    (clang_version + ['-D__GNUC__=4', '-D_MSC_VER=1930'], 'CLANG'),
    (clang_version + ['-D__ARMCOMPILER_VERSION=6100000'], 'ARMCLANG'),
    (['-D__GNUC__=4', '-D__GNUC_MINOR__=2', '-D__INTEL_COMPILER=1900'], 'INTEL_CLASSIC'),
]:
    expect('hps/base/compiler.h', ['HPS_COMPILER == HPS_COMPILER_' + expected,
           '(HPS_COMPILER_IS_GCC + HPS_COMPILER_IS_CLANG + HPS_COMPILER_IS_MSVC + '
           'HPS_COMPILER_IS_ARMCLANG + HPS_COMPILER_IS_INTEL_CLASSIC + '
           'HPS_COMPILER_IS_INTEL_LLVM + HPS_COMPILER_IS_IAR + '
           'HPS_COMPILER_IS_ARMCC + HPS_COMPILER_IS_TINYCC) == ' +
           ('0' if expected == 'UNKNOWN' else '1')],
           definitions, headers=False)

for definitions, expected in [
    ([], 'UNKNOWN'), (['-D__x86_64__=1'], 'X86_64'),
    (['-D__i386__=1'], 'X86'), (['-D_M_X64=1'], 'X86_64'),
    (['-D_M_IX86=600'], 'X86'), (['-D__arm__=1'], 'ARM32'),
    (['-D_M_ARM=7'], 'ARM32'),
    (['-D__aarch64__=1'], 'AARCH64'), (['-D_M_ARM64=1'], 'AARCH64'),
    (['-D__riscv=1', '-D__riscv_xlen=32'], 'RISCV32'),
    (['-D__riscv=1', '-D__riscv_xlen=64'], 'RISCV64'),
    (['-D__riscv=1'], 'RISCV'),
    (['-D__riscv=1', '-D__riscv_xlen=128'], 'RISCV'),
]:
    expect('hps/base/architecture/id.h',
           ['HPS_ARCHITECTURE == HPS_ARCH_' + expected], definitions, headers=False)

for definitions, expected in [
    ([], 'UNKNOWN'), (['-D_WIN32=1'], 'LITTLE'),
    (['-D__BYTE_ORDER__=1234', '-D__ORDER_LITTLE_ENDIAN__=1234'], 'LITTLE'),
    (['-D__BYTE_ORDER__=4321', '-D__ORDER_BIG_ENDIAN__=4321'], 'BIG'),
    (['-D__BYTE_ORDER__=3412', '-D__ORDER_BIG_ENDIAN__=4321',
      '-D__ORDER_LITTLE_ENDIAN__=1234'], 'UNKNOWN'),
]:
    expect('hps/base/architecture/endian.h',
           ['HPS_ENDIANNESS == HPS_ENDIAN_' + expected], definitions, headers=False)

# Keep host ABI macros for system limits.h, and vary only pointer detection.
for definitions, bits, width in [
    (['-U__SIZEOF_POINTER__'], 0, 'UNKNOWN'),
    (['-U__SIZEOF_POINTER__', '-D_WIN32=1'], 32, '32'),
    (['-U__SIZEOF_POINTER__', '-D_WIN64=1'], 64, '64'),
    (['-U__SIZEOF_POINTER__', '-D__SIZEOF_POINTER__=4'], 32, '32'),
    (['-U__SIZEOF_POINTER__', '-D__SIZEOF_POINTER__=8'], 64, '64'),
    (['-U__SIZEOF_POINTER__', '-D__SIZEOF_POINTER__=16'], 128, 'UNKNOWN'),
]:
    expect('hps/base/architecture/pointer.h', ['HPS_POINTER_BITS == ' + str(bits),
           'HPS_POINTER_WIDTH == HPS_POINTER_WIDTH_' + width], definitions)

# A compiler without extension probes must still compile the portable API.
compile_source('''#include "hps/base/compiler.h"
#undef HPS_HAS_ATTRIBUTE
#define HPS_HAS_ATTRIBUTE(name) 0
#include "hps/base/attributes.h"
#include <stdint.h>
HPS_STATIC_ASSERT(sizeof(size_t) >= 1U, "portable assertion");
HPS_NODISCARD static HPS_INLINE int32_t identity(int32_t value) { return value; }
int main(void) { return (identity(0) == 0) ? 0 : 1; }
''', run=True)
# Borrowed typed spans keep declarations/make syntax usable before compound literals.
span_source = '''#include "hps/data_structure/define.h"
HPS_SPAN_DEFINE(size_t, test_sizes)
static const size_t data[] = {1U, 2U};
static const test_sizes_t initial = HPS_SPAN_STATIC_INIT(data, 2U);
int main(void)
{
    test_sizes_t span;
    span = test_sizes_make(data, 2U);
    if (span.data != data || span.length != 2U || initial.length != 2U) { return 1; }
    span = HPS_SPAN_MAKE(test_sizes, data, 2U);
    return (span.data == data && span.length == 2U) ? 0 : 2;
}
'''
for dialect in ['c89', 'c99', 'c11', 'c17', 'c2x']:
    compile_source(span_source, dialect, run=True)
# Without a builtin probe/GCC identity, both hint fallbacks still normalize/evaluate once.
compile_source('''#include "hps/base/compiler.h"
#undef HPS_HAS_BUILTIN
#define HPS_HAS_BUILTIN(name) 0
#undef HPS_COMPILER_IS_GCC
#define HPS_COMPILER_IS_GCC 0
#include "hps/base/expression.h"
#include <stdint.h>
int main(void)
{
    uint8_t calls = 0U;
    if (HPS_LIKELY(++calls) != 1 || calls != 1U) { return 1; }
    if (HPS_UNLIKELY(42) != 1 || HPS_LIKELY(0) != 0) { return 2; }
    return 0;
}
''', run=True)
# The supplied string-switch generator also keeps its definition body C89-style.
enum_source = '''#include "hps/enum/cstr.h"
#define KEYS(E,P) E(P,PORT,"port") E(P,MODE,"mode")
HPS_CSTR_ENUM_DEFINE_STATIC(key_t, KEY, KEYS)
int main(void)
{
    if (key_t_from_cstr("port", 5U) != KEY_PORT) { return 1; }
    if (key_t_from_cstr("port", 4U) != KEY_UNKNOWN) { return 2; }
    return (strcmp(key_t_to_cstr(KEY_MODE), "mode") == 0) ? 0 : 3;
}
'''
compile_source(enum_source, 'c89', run=True)
compile_source(enum_source, 'c99', run=True)
print(f'HPS base: {checks} portability checks passed ({" ".join(CC)}).')
