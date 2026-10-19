#!/usr/bin/env python3
"""Verify optional hosted diagnostics and abort behavior in isolated processes."""
import argparse
import os
from pathlib import Path
import shlex
import signal
import subprocess
import tempfile

try:
    import resource
except ImportError:
    resource = None

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler', nargs='+', default=['cc'])
args = parser.parse_args()
COMPILER = [part for value in args.compiler for part in shlex.split(value)]


def disable_core_dump():
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))


source = '''#include "hps/platform/hosted/debug.h"
#include <stdlib.h>
#include <string.h>
int main(void)
{
    const char *mode = getenv("HPS_TEST_HOSTED_MODE");
    uint32_t evaluations = UINT32_C(0);
    if (mode == NULL) { return 1; }
    if (strcmp(mode, "success") == 0) {
        hps_result_report(INT32_C(0), NULL, SIZE_MAX, NULL, SIZE_MAX);
        hps_panic_on_result(INT32_C(0), NULL, SIZE_MAX, NULL, SIZE_MAX);
        HPS_DEBUG_ASSERT(++evaluations == UINT32_C(1));
#ifdef NDEBUG
        if (evaluations != UINT32_C(0)) { return 2; }
#else
        if (evaluations != UINT32_C(1)) { return 3; }
#endif
        HPS_ASSERT_OR_PANIC(++evaluations != UINT32_C(0));
#ifdef NDEBUG
        if (evaluations != UINT32_C(1)) { return 9; }
#else
        if (evaluations != UINT32_C(2)) { return 10; }
#endif
        return 0;
    }
    if (strcmp(mode, "debug") == 0) { HPS_DEBUG_ASSERT(0); return 0; }
    if (strcmp(mode, "always") == 0) { HPS_ASSERT_OR_PANIC(0); return 0; }
    if (strcmp(mode, "panic") == 0) { hps_panic("failureTAIL", 7U); return 4; }
    if (strcmp(mode, "null_panic") == 0) { hps_panic(NULL, SIZE_MAX); return 5; }
    if (strcmp(mode, "report") == 0) {
        hps_result_report(INT32_MIN, "ctxTAIL", 3U, "bad %sTAIL", 6U);
        return 0;
    }
    if (strcmp(mode, "null_report") == 0) {
        hps_result_report(INT32_C(1), NULL, SIZE_MAX, NULL, SIZE_MAX);
        return 0;
    }
    if (strcmp(mode, "error_panic") == 0) {
        hps_panic_on_result(-INT32_C(42), "ctx", 3U, "bad", 3U);
        return 6;
    }
    if (strcmp(mode, "long_panic") == 0) {
        hps_panic("LONG_MESSAGE", sizeof("LONG_MESSAGE") - 1U);
        return 7;
    }
    return 8;
}
'''.replace('LONG_MESSAGE', 'x' * 180)
checks = 0
with tempfile.TemporaryDirectory(prefix='hps-hosted-debug-') as temporary:
    directory = Path(temporary)
    caller = directory / 'caller.c'
    caller.write_text(source)
    common = COMPILER + ['-std=c99', '-Wall', '-Wextra', '-Wpedantic', '-Werror',
                         '-I', str(ROOT / 'include')]
    adapter = directory / 'debug.o'
    subprocess.run(common + ['-c', str(ROOT / 'src/platform/hosted/debug.c'),
                             '-o', str(adapter)], check=True)
    for release in [False, True]:
        target = directory / ('release' if release else 'debug')
        subprocess.run(common + (['-DNDEBUG'] if release else ['-UNDEBUG']) +
                       [str(caller), str(adapter), '-o', str(target)], check=True)
        cases = [
            ('success', False, ''),
            ('debug', not release, None if not release else ''),
            ('always', True, 'PANIC: Assertion failed: 0\n'),
            ('panic', True, 'PANIC: failure\n'),
            ('null_panic', True, 'PANIC: ?\n'),
            ('report', False, '[ERROR] (ctx): bad %s [result=-2147483648]\n'),
            ('null_report', False, '[ERROR] (?): ? [result=1]\n'),
            ('error_panic', True, '[ERROR] (ctx): bad [result=-42]\n'),
            ('long_panic', True, 'PANIC: ' + ('x' * 180) + '\n'),
        ]
        for mode, aborts, expected in cases:
            environment = dict(os.environ, HPS_TEST_HOSTED_MODE=mode)
            preexec = disable_core_dump if resource is not None else None
            result = subprocess.run([str(target)], cwd=directory, env=environment,
                                    capture_output=True, text=True, preexec_fn=preexec)
            if aborts:
                if os.name == 'posix':
                    assert result.returncode == -signal.SIGABRT, (mode, result)
                else:
                    assert result.returncode != 0, (mode, result)
            else:
                assert result.returncode == 0, (mode, result)
            assert result.stdout == '', (mode, result)
            if expected is None:
                assert result.stderr.startswith('ASSERTION FAILED: 0\nFile: '), result
                assert '\nLine: ' in result.stderr, result
            else:
                assert result.stderr == expected, (mode, result)
            checks += 1
print(f'HPS hosted debug: {checks} process checks passed ({" ".join(COMPILER)}).')
