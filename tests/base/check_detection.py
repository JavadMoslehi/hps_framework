#!/usr/bin/env python3
"""Check migrated detection boundaries with synthetic macros, without execution.

Synthetic compiler/OS identities exercise preprocessing only. They do not qualify
those toolchains, operating systems, libraries, execution profiles or ABIs.
"""
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


def check(header, expressions, definitions=(), dialect='c99', fail=False, prefix='',
          include_paths=(), flags=(), synthetic=True):
    global checks
    source = prefix + '#include "' + header + '"\n#include "' + header + '"\n'
    for expression in expressions:
        source += '#if !(' + expression + ')\n#error "detection contract"\n#endif\n'
    source += 'typedef void (*hps_detection_translation_unit)(void);\n'
    language = 'c++' if dialect.startswith('c++') else 'c'
    command = COMPILER + ['-x', language, '-std=' + dialect]
    if synthetic:
        command += ['-undef']
    command += ['-Wall',
                          '-Wextra', '-Werror', '-pedantic-errors', '-I',
                          str(ROOT / 'include'), '-fsyntax-only']
    command += ['-I' + str(path) for path in include_paths]
    command += list(flags) + ['-D' + value for value in definitions] + ['-']
    result = subprocess.run(command, input=source, text=True, capture_output=True)
    if (result.returncode != 0) != fail:
        raise RuntimeError(' '.join(command) + '\n' + result.stderr + '\n' + source)
    if fail and 'HPS_CONFIG_LIBC' not in result.stderr:
        raise RuntimeError('Expected HPS_CONFIG_LIBC rejection:\n' + result.stderr)
    checks += 1


# ISA capabilities are translation-unit targets, independent from CPU identity.
# Synthetic cross-target cases use only the needed limits contract, so host SDK
# include selection cannot turn an ARM/unknown macro fixture into a native build.
instruction_macros = ['__AES__', '__PCLMUL__', '__AVX__', '__AVX2__', '__SHA__']
instruction_names = ['AESNI', 'PCLMUL', 'AVX', 'AVX2', 'SHA']
all_instruction_definitions = [name + '=1' for name in instruction_macros]


def instruction_expectations(enabled):
    return ['HPS_TARGET_HAS_X86_' + name + ' == ' + str(1 if name in enabled else 0)
            for name in instruction_names]


with tempfile.TemporaryDirectory(prefix='hps-detection-instructions-') as temporary:
    Path(temporary, 'limits.h').write_text('#define CHAR_BIT 8\n')
    for dialect in ['c99', 'c++11']:
        for target in ['__i386__=1', '_M_IX86=600', '__x86_64__=1', '_M_X64=100']:
            check('hps/base/features/hps_architecture.h', instruction_expectations([]),
                  [target], dialect=dialect, include_paths=[temporary])
            check('hps/base/features/hps_architecture.h', instruction_expectations(instruction_names),
                  [target] + all_instruction_definitions, dialect=dialect,
                  include_paths=[temporary])
        for macro, name in zip(instruction_macros, instruction_names):
            check('hps/base/features/hps_architecture.h', instruction_expectations([name]),
                  ['__x86_64__=1', macro + '=1'], dialect=dialect,
                  include_paths=[temporary])
        for target in [[], ['__arm__=1'], ['__aarch64__=1'], ['__riscv=1'],
                       ['__mips__=1'], ['__powerpc__=1']]:
            check('hps/base/features/hps_architecture.h', instruction_expectations([]),
                  target + all_instruction_definitions, dialect=dialect,
                  include_paths=[temporary])

# Native option checks ask the frontend to validate declarations only; no object,
# executable or instruction is emitted/run. Non-x86 compiler targets explicitly
# skip x86 options while retaining every synthetic architecture-gating check.
predefined = subprocess.run(COMPILER + ['-dM', '-E', '-x', 'c', '-'], input='',
                           text=True, capture_output=True, check=True).stdout
predefined_names = {line.split()[1] for line in predefined.splitlines()
                    if line.startswith('#define ')}
if predefined_names.intersection({'__i386__', '__x86_64__', '_M_IX86', '_M_X64'}):
    disabled = ['-mno-aes', '-mno-pclmul', '-mno-avx', '-mno-avx2', '-mno-sha']
    native_cases = [([], []), (['-maes'], ['AESNI']), (['-mpclmul'], ['PCLMUL']),
                    (['-mavx'], ['AVX']), (['-mavx2'], ['AVX', 'AVX2']),
                    (['-msha'], ['SHA']),
                    (['-maes', '-mpclmul', '-mavx', '-mavx2', '-msha'], instruction_names)]
    for dialect in ['c99', 'c++11']:
        for options, enabled in native_cases:
            check('hps/base/features/hps_architecture.h', instruction_expectations(enabled),
                  dialect=dialect, flags=disabled + options, synthetic=False)
else:
    print('HPS detection: native x86 option checks skipped for non-x86 compiler target.')


clang = ['__clang__=1', '__clang_major__=14', '__clang_minor__=0',
         '__clang_patchlevel__=2']
gcc = ['__GNUC__=11', '__GNUC_MINOR__=4', '__GNUC_PATCHLEVEL__=1']
identities = ['GCC', 'CLANG', 'MSVC', 'ARMCLANG', 'INTEL_CLASSIC',
              'INTEL_LLVM', 'IAR', 'ARMCC', 'TINYCC', 'UNKNOWN']
for definitions, identity, version in [
    ([], 'UNKNOWN', (0, 0, 0)),
    (gcc, 'GCC', (11, 4, 1)),
    (clang + gcc + ['_MSC_VER=1930'], 'CLANG', (14, 0, 2)),
    (clang + gcc + ['__ARMCOMPILER_VERSION=6100000'], 'ARMCLANG', (14, 0, 2)),
    (clang + gcc + ['__INTEL_LLVM_COMPILER=20240000'], 'INTEL_LLVM', (14, 0, 2)),
    (['__INTEL_LLVM_COMPILER=20240000'], 'INTEL_LLVM', (0, 0, 0)),
    (gcc + ['__INTEL_COMPILER=1901'], 'INTEL_CLASSIC', (19, 1, 0)),
    (['_MSC_VER=1930', '__INTEL_COMPILER=1901'], 'INTEL_CLASSIC', (19, 1, 0)),
    (['__ICC=1'], 'INTEL_CLASSIC', (0, 0, 0)),
    (['__ICL=1'], 'INTEL_CLASSIC', (0, 0, 0)),
    (['__IAR_SYSTEMS_ICC__=1', '__VER__=9050001'], 'IAR', (0, 0, 0)),
    (['__ICCARM__=1', '__VER__=9050001'], 'IAR', (0, 0, 0)),
    (['__ICCRX__=1', '__VER__=4050001'], 'IAR', (0, 0, 0)),
    (['__ICAVR__=1'], 'IAR', (0, 0, 0)),
    (['__ICCAVR__=1'], 'IAR', (0, 0, 0)),
    (gcc + ['__CC_ARM=1', '__ARMCC_VERSION=5060750'], 'ARMCC', (5, 6, 750)),
    (['__ARMCOMPILER_VERSION=6100000'], 'ARMCLANG', (0, 0, 0)),
    (gcc + ['__TINYC__=927'], 'TINYCC', (0, 0, 0)),
    (['_MSC_VER=1930', '_MSC_FULL_VER=193012345'], 'MSVC', (19, 30, 0)),
    (gcc + ['__NVCOMPILER=1'], 'UNKNOWN', (0, 0, 0)),
]:
    check('hps/base/hps_compiler.h', [
        'HPS_COMPILER == HPS_COMPILER_' + identity,
        ' + '.join('HPS_COMPILER_IS_' + name for name in identities) + ' == 1',
        'HPS_COMPILER_VERSION_MAJOR == ' + str(version[0]),
        'HPS_COMPILER_VERSION_MINOR == ' + str(version[1]),
        'HPS_COMPILER_VERSION_PATCH == ' + str(version[2]),
    ], definitions)

for definitions, family in [
    ([], 'UNKNOWN'), (['__mips__=1'], 'MIPS'),
    (['__MIPS__=1'], 'MIPS'), (['__powerpc__=1'], 'PPC'),
    (['__powerpc64__=1'], 'PPC'), (['_M_PPC=1'], 'PPC'),
    (['__riscv=1'], 'RISCV'), (['__riscv=1', '__riscv_xlen=128'], 'RISCV'),
]:
    check('hps/base/architecture/hps_id.h',
          ['HPS_ARCHITECTURE == HPS_ARCH_' + family], definitions)

# Bi-endian CPU identity is insufficient evidence for byte order.
for definitions in [[], ['__arm__=1'], ['__aarch64__=1'],
                    ['__riscv=1'], ['__mips__=1'], ['__powerpc__=1']]:
    check('hps/base/architecture/hps_endian.h',
          ['HPS_ENDIANNESS == HPS_ENDIAN_UNKNOWN'], definitions)

for target in ['__i386__=1', '__x86_64__=1', '_WIN32=1']:
    check('hps/base/architecture/hps_endian.h',
          ['HPS_ENDIANNESS == HPS_ENDIAN_LITTLE'], [target])
    check('hps/base/architecture/hps_endian.h',
          ['HPS_ENDIANNESS == HPS_ENDIAN_UNKNOWN'],
          [target, '__BYTE_ORDER__=3412', '__ORDER_LITTLE_ENDIAN__=1234',
           '__ORDER_BIG_ENDIAN__=4321'])
    check('hps/base/architecture/hps_endian.h',
          ['HPS_ENDIANNESS == HPS_ENDIAN_BIG'],
          [target, '__BYTE_ORDER__=4321', '__ORDER_LITTLE_ENDIAN__=1234',
           '__ORDER_BIG_ENDIAN__=4321'])

# Standard syntax and compiler extensions remain separate, including in C++.
for dialect in ['c89', 'c99', 'c11', 'c17', 'c++11', 'c++17']:
    c99 = dialect in ['c99', 'c11', 'c17']
    check('hps/base/features/hps_language.h', [
        'HPS_HAS_COMPOUND_LITERALS == ' + str(1 if c99 else 0),
        'HPS_HAS_DESIGNATED_INITIALIZERS == ' + str(1 if c99 else 0),
        '!HPS_HAS_TYPEOF',
    ], dialect=dialect)
    for strict in [False, True]:
        definitions = gcc + (['__STRICT_ANSI__=1'] if strict else [])
        check('hps/base/features/hps_compiler.h', [
            'HPS_HAS_EXTENSION_STATEMENT_EXPRESSIONS == ' + str(0 if strict else 1),
            'HPS_HAS_EXTENSION_TYPEOF == ' + str(0 if strict else 1),
            'HPS_HAS_BUILTIN_EXPECT',
            'HPS_HAS_BUILTIN_TYPES_COMPATIBLE_P == ' + str(0 if dialect.startswith('c++') else 1),
            'HPS_HAS_BUILTIN_CHOOSE_EXPR == ' + str(0 if dialect.startswith('c++') else 1),
        ], definitions, dialect=dialect)
check('hps/base/features/hps_compiler.h',
      ['!HPS_HAS_EXTENSION_STATEMENT_EXPRESSIONS',
       '!HPS_HAS_EXTENSION_TYPEOF', 'HPS_HAS_EXTENSION_THREAD_LOCAL'],
      ['_MSC_VER=1930'])
check('hps/base/features/hps_language.h', ['HPS_HAS_TYPEOF'], prefix=
      '#include "hps/base/hps_language.h"\n#undef HPS_C_STANDARD_VERSION\n'
      '#define HPS_C_STANDARD_VERSION 202311L\n')

for definitions, expected in [
    ([], ['HPS_PLATFORM_IS_UNKNOWN', '!HPS_PLATFORM_IS_POSIX_FAMILY']),
    (['_WIN64=1'], ['HPS_PLATFORM_IS_WINDOWS', '!HPS_PLATFORM_IS_POSIX_FAMILY']),
    (['__linux__=1'], ['HPS_PLATFORM_IS_LINUX', 'HPS_PLATFORM_IS_POSIX_FAMILY']),
    (['__linux__=1', '__ANDROID__=1'], ['HPS_PLATFORM_IS_ANDROID', 'HPS_PLATFORM_IS_LINUX']),
    (['__FreeBSD__=1'], ['HPS_PLATFORM_IS_BSD', 'HPS_PLATFORM_IS_POSIX_FAMILY']),
    (['__unix__=1'], ['HPS_PLATFORM_IS_UNIX', 'HPS_PLATFORM_IS_POSIX_FAMILY']),
    (['__APPLE__=1', '__MACH__=1'], ['HPS_PLATFORM_IS_APPLE', '!HPS_PLATFORM_IS_IOS', '!HPS_PLATFORM_IS_MACOS']),
    (['__APPLE__=1', '__MACH__=1', 'TARGET_OS_IOS=1'], ['HPS_PLATFORM_IS_IOS', '!HPS_PLATFORM_IS_MACOS']),
    (['__APPLE__=1', '__MACH__=1', 'TARGET_OS_OSX=1'], ['HPS_PLATFORM_IS_MACOS', '!HPS_PLATFORM_IS_IOS']),
    (['__APPLE__=1', '__MACH__=1', 'TARGET_OS_IPHONE=1', 'TARGET_OS_TV=1'], ['!HPS_PLATFORM_IS_IOS', '!HPS_PLATFORM_IS_MACOS']),
]:
    check('hps/platform/hps_os.h', expected, definitions)

for definitions, identity in [
    ([], 'UNKNOWN'), (['__linux__=1'], 'UNKNOWN'),
    (['_MSC_VER=1930'], 'UNKNOWN'), (['__APPLE__=1', '__MACH__=1'], 'UNKNOWN'),
    (['__GLIBC__=2'], 'GLIBC'),
    (['__UCLIBC__=1', '__GLIBC__=2'], 'UCLIBC'),
    (['__NEWLIB__=4'], 'NEWLIB'), (['__BIONIC__=1'], 'BIONIC'),
    (['__MSVCRT__=1'], 'MSVCRT'), (['_UCRT=1'], 'UCRT'),
    (['__DARWIN_C_LEVEL=200809L'], 'APPLE'),
    (['HPS_CONFIG_LIBC=HPS_LIBC_MUSL'], 'MUSL'),
    (['__GLIBC__=2', 'HPS_CONFIG_LIBC=HPS_LIBC_GLIBC'], 'GLIBC'),
]:
    check('hps/platform/hps_libc.h', ['HPS_LIBC_IS_' + identity], definitions)
for definitions in [['HPS_CONFIG_LIBC=99'], ['HPS_CONFIG_LIBC=-1'],
                    ['HPS_CONFIG_LIBC=0'],
                    ['__GLIBC__=2', 'HPS_CONFIG_LIBC=HPS_LIBC_MUSL']]:
    check('hps/platform/hps_libc.h', [], definitions, fail=True)

for definitions, expressions in [
    ([], ['!HPS_POSIX_AVAILABLE', 'HPS_POSIX_VERSION == 0L', 'HPS_XOPEN_VERSION == 0L']),
    (['__linux__=1'], ['!HPS_POSIX_AVAILABLE', 'HPS_POSIX_VERSION == 0L']),
    (['_POSIX_VERSION=200809L', '_XOPEN_VERSION=700'],
     ['HPS_POSIX_AVAILABLE', 'HPS_POSIX_VERSION == 200809L', 'HPS_XOPEN_VERSION == 700']),
    (['_POSIX_C_SOURCE=200809L', '_XOPEN_SOURCE=700'],
     ['!HPS_POSIX_AVAILABLE', 'HPS_POSIX_VERSION == 0L', 'HPS_XOPEN_VERSION == 0L']),
]:
    check('hps/platform/hps_posix.h', expressions, definitions)

# Neither detection family injects bool/true/false into a consumer's namespace.
for header in ['hps/base/features/hps_language.h', 'hps/platform/platform.h']:
    for dialect in ['c89', 'c99', 'c11', 'c++11', 'c++17']:
        check(header, ['!defined(bool)', '!defined(true)', '!defined(false)'],
              dialect=dialect)

# Pointer width comes from its own evidence even for a 64-bit CPU with x32 ABI.
with tempfile.TemporaryDirectory(prefix='hps-detection-width-') as temporary:
    Path(temporary, 'limits.h').write_text('#define CHAR_BIT 8\n')
    source = '''#include "hps/base/architecture/architecture.h"
#if HPS_ARCHITECTURE != HPS_ARCH_X86_64 || HPS_POINTER_BITS != 32
#error "CPU identity must not imply pointer width"
#endif
typedef void (*hps_detection_translation_unit)(void);
'''
    subprocess.run(COMPILER + ['-std=c99', '-undef', '-Wall', '-Wextra', '-Werror',
                              '-pedantic-errors', '-I' + temporary,
                              '-I' + str(ROOT / 'include'), '-D__x86_64__=1',
                              '-D__SIZEOF_POINTER__=4', '-x', 'c', '-',
                              '-fsyntax-only'], input=source, text=True, check=True)
    checks += 1

print(f'HPS detection: {checks} checks passed ({" ".join(COMPILER)}).')
