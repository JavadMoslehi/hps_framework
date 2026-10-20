#!/usr/bin/env python3
"""Exercise standard type wrappers in supported native C and C++ dialects.

Fallback tests use actual host compiler metadata in isolated translation units.
Synthetic missing-metadata cases are rejected without running generated code.
These checks do not qualify another compiler, target ABI or standard library.
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
COMPILER = [part for value in args.compiler for part in shlex.split(value)]
checks = 0
skipped = []


def language(dialect):
    return 'c++' if dialect.startswith('c++') else 'c'


def check(source, dialect='c99', flags=(), run=False, reject=None):
    global checks
    with tempfile.TemporaryDirectory(prefix='hps-standard-types-') as directory:
        output = Path(directory) / 'test'
        command = COMPILER + ['-std=' + dialect, '-x', language(dialect),
                              '-Wall', '-Wextra', '-Werror', '-pedantic-errors',
                              '-I', str(ROOT / 'include'),
                              '-I', str(ROOT / 'tests/base')] + list(flags)
        if run:
            command += ['-', '-o', str(output)]
        else:
            command += ['-', '-fsyntax-only']
        result = subprocess.run(command, input=source, text=True, capture_output=True)
        if reject:
            if result.returncode == 0 or reject not in result.stderr:
                raise RuntimeError('Expected rejection: ' + reject + '\n' +
                                   ' '.join(command) + '\n' + result.stderr)
        elif result.returncode != 0:
            raise RuntimeError(' '.join(command) + '\n' + result.stderr + '\n' + source)
        if run and not reject:
            subprocess.run([str(output)], check=True)
    checks += 1


def supported(dialect):
    result = subprocess.run(COMPILER + ['-std=' + dialect, '-x', language(dialect),
                                      '-fsyntax-only', '-'],
                            input='int main(void) { return 0; }\n',
                            text=True, capture_output=True)
    if result.returncode:
        skipped.append(dialect)
        return False
    return True


headers = ['hps/base/hps_stdint.h', 'hps/base/hps_stddef.h', 'hps/base/hps_bool.h']
wrapper_includes = ''.join('#include "' + header + '"\n' for header in headers)
native_includes = '#include <stdint.h>\n#include <stddef.h>\n#include <inttypes.h>\n#include <string.h>\n'
suite = (ROOT / 'tests/base/test_standard_types.c').read_text()
suite += '\nint main(void) { return (test_standard_types() == INT32_C(0)) ? 0 : 1; }\n'
configurations = [[], ['-DHPS_STDINT_USE_FALLBACK=1'],
                  ['-DHPS_STDDEF_USE_FALLBACK=1'],
                  ['-DHPS_STDINT_USE_FALLBACK=1', '-DHPS_STDDEF_USE_FALLBACK=1']]
dialects = [dialect for dialect in ['c89', 'c99', 'c11', 'c17', 'c2x', 'c23',
                                  'c++98', 'c++11', 'c++17', 'c++20']
            if supported(dialect)]

integer_contracts = '''
#if INT8_MIN != (-INT8_MAX - 1) || UINT8_MAX != 255
#error "8-bit limits"
#endif
#if INT16_MIN != (-INT16_MAX - 1) || UINT16_MAX != 65535
#error "16-bit limits"
#endif
#if INT32_MIN != (-INT32_MAX - 1) || UINT32_MAX != 4294967295U
#error "32-bit limits"
#endif
#if INT64_MIN != (-INT64_MAX - 1) || INT64_MAX != (UINT64_MAX >> 1)
#error "64-bit limits"
#endif
#if INT8_C(127) != INT8_MAX || UINT8_C(255) != UINT8_MAX
#error "8-bit integer constant expressions"
#endif
#if INT16_C(32767) != INT16_MAX || UINT16_C(65535) != UINT16_MAX
#error "16-bit integer constant expressions"
#endif
#if INT32_C(2147483647) != INT32_MAX || UINT32_C(4294967295) != UINT32_MAX
#error "32-bit integer constant expressions"
#endif
#if INT64_C(9223372036854775807) != INT64_MAX
#error "64-bit signed integer constant expression"
#endif
#if UINT64_C(18446744073709551615) != UINT64_MAX
#error "64-bit unsigned integer constant expression"
#endif
#if INTMAX_C(1) != 1 || UINTMAX_C(1) != 1
#error "maximum-width integer constant expressions"
#endif
int main(void)
{
    int8_t minimum8 = INT8_MIN;
    int16_t minimum16 = INT16_MIN;
    int32_t minimum32 = INT32_MIN;
    int64_t minimum64 = INT64_MIN;
    uint64_t maximum64 = UINT64_MAX;
    intmax_t minimum_max = INTMAX_MIN;
    uintmax_t maximum_max = UINTMAX_MAX;
    if (minimum8 >= INT8_C(0) || minimum16 >= INT16_C(0) ||
        minimum32 >= INT32_C(0) || minimum64 >= INT64_C(0) ||
        minimum_max >= INTMAX_C(0) || maximum64 <= UINT64_C(0) ||
        maximum_max <= UINTMAX_C(0)) { return 1; }
    return 0;
}
'''


def type_contract(expression, typename, dialect):
    if language(dialect) == 'c++':
        return ('static_assert(hps_same_type<decltype(' + expression +
                '), decltype(+(' + typename + ')0)>::value, "constant type");\n')
    return ('_Static_assert(_Generic((' + expression + '), __typeof__(+(' +
            typename + ')0): 1, default: 0), "constant type");\n')


def other_limit_contracts(dialect):
    source = ''
    for name, typename in [('WINT', '__WINT_TYPE__'), ('WCHAR', '__WCHAR_TYPE__'),
                           ('SIG_ATOMIC', '__SIG_ATOMIC_TYPE__'),
                           ('PTRDIFF', '__PTRDIFF_TYPE__')]:
        source += '#if defined(' + typename + ') && defined(' + name + '_MIN)\n'
        source += type_contract(name + '_MIN', typename, dialect)
        source += type_contract(name + '_MAX', typename, dialect)
        source += '#endif\n'
    return source

for dialect in dialects:
    for flags in configurations:
        for header in headers:
            check('#include "' + header + '"\n#include "' + header +
                  '"\ntypedef void (*hps_standard_header_probe)(void);\n', dialect, flags)
        check(suite, dialect, flags, run=True)
        check(wrapper_includes + integer_contracts, dialect, flags, run=True)
    # Native order compatibility belongs to the default provider. Replacement
    # fallback headers deliberately do not promise coexistence with native types.
    for includes in [native_includes + wrapper_includes,
                     wrapper_includes + native_includes]:
        check(includes + integer_contracts, dialect, run=True)
    if language(dialect) == 'c++' or dialect == 'c23':
        check(wrapper_includes + '''
#ifdef bool
#error "native bool must remain a keyword"
#endif
#ifdef true
#error "native true must remain a keyword"
#endif
#ifdef false
#error "native false must remain a keyword"
#endif
int main(void) { return true && !false ? 0 : 1; }
''', dialect, run=True)

# Verify constant macro expression types against the required integer-promotion
# types, rather than assuming a host data model or a particular native base type.
for dialect in dialects:
    if dialect not in ['c11', 'c17', 'c2x', 'c23', 'c++11', 'c++17', 'c++20']:
        continue
    for flags in configurations:
        source = wrapper_includes
        if language(dialect) == 'c++':
            source += '''
template <typename Left, typename Right> struct hps_same_type { static const bool value = false; };
template <typename Type> struct hps_same_type<Type, Type> { static const bool value = true; };
'''
        for signedness in ['INT', 'UINT']:
            for bits in ['8', '16', '32', '64']:
                macro = signedness + bits + '_C(0)'
                typename = ('int' if signedness == 'INT' else 'uint') + '_least' + bits + '_t'
                source += type_contract(macro, typename, dialect)
        source += type_contract('INTMAX_C(0)', 'intmax_t', dialect)
        source += type_contract('UINTMAX_C(0)', 'uintmax_t', dialect)
        source += other_limit_contracts(dialect)
        check(source + '\nint main(void) { return 0; }\n', dialect, flags)

# Synthetic unsigned metadata exercises the zero-minimum derivation even on a
# host whose compiler supplies an explicit minimum macro. No code is executed.
unsigned_metadata = ['-DHPS_STDINT_USE_FALLBACK=1', '-U__WINT_MIN__',
                     '-U__WINT_UNSIGNED__', '-D__WINT_UNSIGNED__=1',
                     '-U__SIG_ATOMIC_MIN__', '-U__SIG_ATOMIC_TYPE__',
                     '-D__SIG_ATOMIC_TYPE__=unsigned int', '-U__SIG_ATOMIC_MAX__',
                     '-D__SIG_ATOMIC_MAX__=4294967295U',
                     '-U__SIG_ATOMIC_UNSIGNED__', '-D__SIG_ATOMIC_UNSIGNED__=1']
check(wrapper_includes + other_limit_contracts('c11') +
      '\nint main(void) { return 0; }\n', 'c11', unsigned_metadata)
check(wrapper_includes + other_limit_contracts('c11') +
      '\nint main(void) { return 0; }\n', 'c11',
      ['-DHPS_STDINT_USE_FALLBACK=1', '-fshort-wchar'])

# A 32-bit frontend with usable freestanding limits can distinguish int from
# equally wide long. Synthetic metadata selects long for the signed 32-bit
# family, exercising constant typing without claiming a foreign library ABI.
probe32 = subprocess.run(COMPILER + ['-m32', '-ffreestanding', '-std=c11',
                                    '-x', 'c', '-Werror', '-fsyntax-only', '-'],
                         input='''#include <limits.h>
_Static_assert(__SIZEOF_INT__ == 4 && __SIZEOF_LONG__ == 4, "32-bit int and long");
''', text=True, capture_output=True)
if probe32.returncode == 0:
    check('''#undef __INT32_TYPE__
#define __INT32_TYPE__ __typeof__(__LONG_MAX__)
#undef __INT32_MAX__
#define __INT32_MAX__ 2147483647L
#undef __INT_LEAST32_TYPE__
#define __INT_LEAST32_TYPE__ __typeof__(__LONG_MAX__)
#undef __INT_LEAST32_MAX__
#define __INT_LEAST32_MAX__ 2147483647L
#undef __INT32_C
#undef __INT32_C_SUFFIX__
#define __INT32_C_SUFFIX__ L
#include "hps/base/hps_stdint.h"
_Static_assert(_Generic(INT32_C(0), __typeof__(__LONG_MAX__): 1, default: 0), "long constant");
#if INT32_C(2147483647) != 2147483647L
#error "32-bit long preprocessor constant"
#endif
''', 'c11', ['-m32', '-ffreestanding', '-DHPS_STDINT_USE_FALLBACK=1'])
else:
    print('HPS standard types: optional 32-bit int/long check skipped; '
          'freestanding headers or backend unavailable.')

# C89 compatibility storage cannot normalize assignments like native _Bool.
# Helpers must still normalize every truthy representation to exactly 1.
check(wrapper_includes + '''
int main(void)
{
    bool raw = UINT8_C(42);
    if (raw != UINT8_C(42) || hps_bool_to_u8(raw) != UINT8_C(1) ||
        hps_bool_to_i32(raw) != INT32_C(1)) { return 1; }
    return 0;
}
''', 'c89', run=True)

for switch, header in [('HPS_STDINT_USE_FALLBACK', 'hps/base/hps_stdint.h'),
                       ('HPS_STDDEF_USE_FALLBACK', 'hps/base/hps_stddef.h')]:
    check('#include "' + header + '"\n', flags=['-D' + switch + '=2'],
          reject=switch + ' must be 0 or 1')
for metadata in ['__SIZE_TYPE__', '__PTRDIFF_TYPE__']:
    check('#include "hps/base/hps_stddef.h"\n',
          flags=['-DHPS_STDDEF_USE_FALLBACK=1', '-U' + metadata],
          reject='HPS stddef fallback requires compiler size and pointer-difference types')
check('#include "hps/base/hps_stddef.h"\n',
      flags=['-DHPS_STDDEF_USE_FALLBACK=1', '-U__WCHAR_TYPE__'],
      reject='HPS stddef fallback requires the compiler wide-character type')
check('#include "hps/base/hps_stddef.h"\n',
      flags=['-DHPS_STDDEF_USE_FALLBACK=1', '-U__GNUC__', '-U__clang__'],
      reject='HPS stddef fallback requires compiler __builtin_offsetof support')

for family in ['', '_LEAST', '_FAST']:
    for bits in ['8', '16', '32', '64']:
        signed = 'INT' + family + bits
        unsigned = 'UINT' + family + bits
        for metadata in ['__' + signed + '_TYPE__', '__' + unsigned + '_TYPE__',
                         '__' + signed + '_MAX__', '__' + unsigned + '_MAX__']:
            check('#include "hps/base/hps_stdint.h"\n',
                  flags=['-DHPS_STDINT_USE_FALLBACK=1', '-U' + metadata],
                  reject='HPS stdint fallback requires compiler ' + signed + '/' +
                  unsigned + ' type and limit metadata')
for metadata in ['__INTMAX_TYPE__', '__UINTMAX_TYPE__', '__INTMAX_MAX__', '__UINTMAX_MAX__']:
    check('#include "hps/base/hps_stdint.h"\n',
          flags=['-DHPS_STDINT_USE_FALLBACK=1', '-U' + metadata],
          reject='HPS stdint fallback requires compiler INTMAX/UINTMAX type and limit metadata')
for macro in ['INT8_C', 'UINT8_C', 'INT16_C', 'UINT16_C', 'INT32_C', 'UINT32_C',
              'INT64_C', 'UINT64_C', 'INTMAX_C', 'UINTMAX_C']:
    check('#include "hps/base/hps_stdint.h"\n',
          flags=['-DHPS_STDINT_USE_FALLBACK=1', '-U__' + macro,
                 '-U__' + macro + '_SUFFIX__'],
          reject='HPS stdint fallback requires compiler ' + macro + ' constant metadata')
for metadata in ['__INTPTR_TYPE__', '__UINTPTR_TYPE__', '__INTPTR_MAX__', '__UINTPTR_MAX__']:
    check('''#include "hps/base/hps_stdint.h"
#if defined(INTPTR_MIN) || defined(INTPTR_MAX) || defined(UINTPTR_MAX)
#error "incomplete optional pointer metadata must not publish a partial pair"
#endif
int main(void) { return 0; }
''', flags=['-DHPS_STDINT_USE_FALLBACK=1', '-U' + metadata])
check('#include <stdint.h>\n#include "hps/base/hps_stdint.h"\n',
      flags=['-DHPS_STDINT_USE_FALLBACK=1'],
      reject='HPS stdint fallback must not be mixed with another stdint provider')
check('''#include <limits.h>
#undef CHAR_BIT
#define CHAR_BIT 16
#include "hps/base/hps_stdint.h"
''', flags=['-DHPS_STDINT_USE_FALLBACK=1'],
      reject='HPS stdint fallback requires CHAR_BIT == 8')

print(f'HPS standard types: {checks} checks passed ({" ".join(COMPILER)}); '
      f'dialects: {", ".join(dialects)}.')
if skipped:
    print('HPS standard types: unsupported dialect options skipped: ' + ', '.join(skipped) + '.')
