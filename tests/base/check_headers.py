#!/usr/bin/env python3
"""Check public header layout, independent inclusion and host C/C++ linkage.

These checks exercise the selected host compiler in C99 and C++11 modes. They
do not qualify another target, a compiler ABI, or general C++ element types.
"""
import argparse
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]

# Expand each generator using only its own public header. An include-only probe
# cannot discover missing dependencies hidden inside a macro replacement list.
GENERATORS = {
    'span': ('''HPS_SPAN_DEFINE(size_t, hps_header_value)
''', '''    size_t values[1] = {0U};
    hps_header_value_t value = HPS_SPAN_STATIC_INIT(values, 1U);
    value = hps_header_value_make(values, 1U);
    value = HPS_SPAN_MAKE(hps_header_value, values, 1U);
    return value.length == 1U ? 0 : 1;
'''),
    'mutable_span': ('''HPS_MUTABLE_SPAN_DEFINE(size_t, hps_header_value)
''', '''    size_t values[1] = {0U};
    hps_header_value_t value = HPS_MUTABLE_SPAN_STATIC_INIT(values, 1U);
    value = hps_header_value_make(values, 1U);
    value = HPS_MUTABLE_SPAN_MAKE(hps_header_value, values, 1U);
    return value.length == 1U ? 0 : 1;
'''),
    'cspan': ('''HPS_CSPAN_DEFINE(size_t, hps_header_value)
''', '''    size_t values[1] = {0U};
    hps_header_value_t value = HPS_CSPAN_STATIC_INIT(values, 1U);
    value = hps_header_value_make(values, 1U);
    value = HPS_CSPAN_MAKE(hps_header_value, values, 1U);
    return value.length == 1U ? 0 : 1;
'''),
    'vector': ('''HPS_VECTOR_DEFINE(size_t, hps_header_value)
HPS_VECTOR_OPERATIONS_DEFINE(size_t, hps_header_value)
''', '''    size_t values[1] = {0U};
    size_t output = 0U;
    uint8_t found = 0U;
    hps_header_value_iterator_t iterator;
    hps_header_value_t value = HPS_VECTOR_STATIC_INIT(values, 0U, 1U);
    value = hps_header_value_make(values, 0U, 1U);
    value = HPS_VECTOR_MAKE(hps_header_value, values, 0U, 1U);
    if (hps_header_value_push(&value, 1U) != HPS_COLLECTION_OK) { return 1; }
    if (hps_header_value_get(&value, 0U, &output) != HPS_COLLECTION_OK) { return 2; }
    if (hps_header_value_iterator_begin(&value, &iterator) != HPS_COLLECTION_OK) {
        return 3;
    }
    return hps_header_value_iterator_next(&iterator, &output, &found) ==
        HPS_COLLECTION_OK ? 0 : 4;
'''),
    'robuffer': ('''HPS_ROBUFFER_DEFINE(size_t, hps_header_value)
''', '''    size_t values[1] = {0U};
    hps_header_value_t value = HPS_ROBUFFER_STATIC_INIT(values, 1U, 1U);
    value = hps_header_value_make(values, 1U, 1U);
    value = HPS_ROBUFFER_MAKE(hps_header_value, values, 1U, 1U);
    return value.length == 1U ? 0 : 1;
'''),
    'svector': ('''HPS_SVECTOR_DEFINE(size_t, hps_header_value, 2U)
''', '''    hps_header_value_t value = HPS_SVECTOR_STATIC_INIT;
    value = hps_header_value_make();
    value = HPS_SVECTOR_MAKE(hps_header_value);
    return value.length == 0U ? 0 : 1;
'''),
    'sring': ('''HPS_SRING_DEFINE(size_t, hps_header_value, 2U)
''', '''    hps_header_value_t value = HPS_SRING_STATIC_INIT;
    value = hps_header_value_make();
    value = HPS_SRING_MAKE(hps_header_value);
    return value.length == 0U ? 0 : 1;
'''),
    'ring': ('''HPS_RING_DEFINE(size_t, hps_header_value)
''', '''    size_t values[1] = {0U};
    hps_header_value_t value = HPS_RING_STATIC_INIT(values, 0U, 0U, 0U, 1U);
    value = hps_header_value_make(values, 0U, 0U, 0U, 1U);
    value = HPS_RING_MAKE(hps_header_value, values, 0U, 0U, 0U, 1U);
    return value.length == 0U ? 0 : 1;
'''),
    'stack': ('''HPS_STACK_DEFINE(size_t, hps_header_value)
''', '''    size_t values[1] = {0U};
    hps_header_value_t value = HPS_STACK_STATIC_INIT(values, 1U, 1U);
    value = hps_header_value_make(values, 1U, 1U);
    value = HPS_STACK_MAKE(hps_header_value, values, 1U, 1U);
    return value.length == 1U ? 0 : 1;
'''),
    'slist': ('''HPS_SLIST_DEFINE(size_t, hps_header_value)
''', '''    hps_header_value_node_t node = HPS_SLIST_NODE_STATIC_INIT(1U, NULL);
    hps_header_value_t value = HPS_SLIST_STATIC_INIT(&node, 1U);
    node = hps_header_value_node_make(1U, NULL);
    value = hps_header_value_make(&node, 1U);
    node = HPS_SLIST_NODE_MAKE(hps_header_value, 1U, NULL);
    value = HPS_SLIST_MAKE(hps_header_value, &node, 1U);
    return value.length == 1U ? 0 : 1;
'''),
    'dlist': ('''HPS_DLIST_DEFINE(size_t, hps_header_value)
HPS_DLIST_OPERATIONS_DEFINE(size_t, hps_header_value)
''', '''    size_t output = 0U;
    uint8_t found = 0U;
    hps_header_value_iterator_t iterator;
    hps_header_value_node_t node = HPS_DLIST_NODE_STATIC_INIT(1U, NULL, NULL);
    hps_header_value_t value = HPS_DLIST_STATIC_INIT(NULL, NULL, 0U);
    node = hps_header_value_node_make(1U, NULL, NULL);
    value = hps_header_value_make(NULL, NULL, 0U);
    node = HPS_DLIST_NODE_MAKE(hps_header_value, 1U, NULL, NULL);
    value = HPS_DLIST_MAKE(hps_header_value, NULL, NULL, 0U);
    if (hps_header_value_push_back(&value, &node) != HPS_COLLECTION_OK) { return 1; }
    if (hps_header_value_iterator_begin(&value, 1U, &iterator) != HPS_COLLECTION_OK) {
        return 2;
    }
    return hps_header_value_iterator_next(&iterator, &output, &found) ==
        HPS_COLLECTION_OK ? 0 : 3;
'''),
    'bst': ('''HPS_BST_DEFINE(size_t, hps_header_value)
''', '''    hps_header_value_node_t node = HPS_BST_NODE_STATIC_INIT(1U, NULL, NULL);
    hps_header_value_t value = HPS_BST_STATIC_INIT(&node, 1U);
    node = hps_header_value_node_make(1U, NULL, NULL);
    value = hps_header_value_make(&node, 1U);
    node = HPS_BST_NODE_MAKE(hps_header_value, 1U, NULL, NULL);
    value = HPS_BST_MAKE(hps_header_value, &node, 1U);
    return value.length == 1U ? 0 : 1;
'''),
    'optional': ('''HPS_OPTIONAL_DEFINE(size_t, hps_header_value)
''', '''    size_t output = 0U;
    uint8_t present = 0U;
    hps_header_value_t value = hps_header_value_none();
    value = hps_header_value_some(1U);
    if (hps_header_value_has(&value, &present) != HPS_OPTIONAL_OK) { return 1; }
    if (hps_header_value_get_or(&value, 0U, &output) != HPS_OPTIONAL_OK) { return 2; }
    if (hps_header_value_set_none(&value) != HPS_OPTIONAL_OK) { return 3; }
    if (hps_header_value_set_some(&value, 1U) != HPS_OPTIONAL_OK) { return 4; }
    return hps_header_value_get(&value, &output) == HPS_OPTIONAL_OK ? 0 : 5;
'''),
}

LINK_CALLER = '''#include "hps/base.h"
#include "hps/memory/memory.h"
#include "hps/bytes/bytes.h"
#include "hps/string/string.h"
#include "hps/cstr/cstr.h"
#include "hps/json/json.h"
#include "hps/validation/validation.h"
int main(void)
{
    const uint8_t bytes[1] = {UINT8_C(1)};
    hps_byte_view view;
    hps_string_view text;
    hps_json_tokenizer tokenizer;
    hps_json_token token;
    size_t length = 0U;
    uint8_t valid = UINT8_C(1);
    if (hps_pool_init() != HPS_POOL_OK) { return 1; }
    if (hps_byte_view_init(&view, bytes, 1U) != HPS_BYTE_OK) { return 2; }
    if (hps_string_view_init(&text, NULL, 0U) != HPS_STRING_OK) { return 3; }
    if (hps_cstr_length("x", 2U, &length) != HPS_STRING_OK) { return 4; }
    if (length != 1U) { return 5; }
    if (hps_json_tokenizer_init(&tokenizer, text) != HPS_JSON_OK) { return 6; }
    if (hps_json_tokenizer_next(&tokenizer, &token) != HPS_JSON_OK) { return 7; }
    if (token.kind != HPS_JSON_EOF) { return 8; }
    if (hps_iran_national_code_validate("0000000000", 11U, &valid) != HPS_STRING_OK) {
        return 9;
    }
    return valid == UINT8_C(0) ? 0 : 10;
}
'''


def checked(command, source=None):
    result = subprocess.run(command, input=source, text=True, capture_output=True)
    if result.returncode != 0:
        detail = '\n' + source if source is not None else ''
        raise RuntimeError(shlex.join(command) + '\n' + result.stderr + detail)


def public_guard(header):
    return re.sub(r'[^A-Za-z0-9]', '_', header.as_posix()).upper()


def header_layout(include):
    public = include / 'hps'
    root_headers = sorted(path.name for path in public.glob('*.h'))
    if root_headers != ['base.h']:
        raise RuntimeError('include/hps must expose only base.h at its root: ' +
                           ', '.join(root_headers))
    headers = sorted(path.relative_to(include) for path in public.rglob('*.h'))
    directories = sorted({path.parent for path in headers if path.parent != Path('hps')})
    umbrellas = {}
    for directory in directories:
        umbrella = (Path('hps/base.h') if directory == Path('hps/base') else
                    directory / (directory.name + '.h'))
        if umbrella not in headers:
            raise RuntimeError('Missing package umbrella: ' + umbrella.as_posix())
        umbrellas[directory] = umbrella
        for header in (path for path in headers if path.parent == directory):
            if header != umbrella and not re.fullmatch(r'hps_[a-z][a-z0-9_]*\.h', header.name):
                raise RuntimeError('Invalid public leaf name: ' + header.as_posix())
    return headers, umbrellas


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', nargs='+', default=[os.environ.get('CC', 'cc')])
    parser.add_argument('--include-dir', type=Path, default=ROOT / 'include',
                        help='Public include root; may name a clean install tree.')
    args = parser.parse_args()
    compiler = [part for value in args.compiler for part in shlex.split(value)]
    include = args.include_dir.resolve()
    headers, umbrellas = header_layout(include)
    common = ['-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-I', str(include)]
    checks = 1

    def compile_source(source, dialect):
        language = 'c++' if dialect == 'c++11' else 'c'
        checked(compiler + common + ['-std=' + dialect, '-x', language,
                                    '-fsyntax-only', '-'], source)

    for dialect in ['c99', 'c++11']:
        for header in headers:
            source = '#include "{0}"\n#include "{0}"\n'.format(header.as_posix())
            source += 'typedef void (*hps_header_translation_unit)(void);\n'
            compile_source(source, dialect)
            checks += 1
        for directory, umbrella in umbrellas.items():
            source = '#include "' + umbrella.as_posix() + '"\n'
            for header in headers:
                if directory not in header.parents:
                    continue
                # Hosted diagnostics are a separate, explicitly selected package.
                if directory == Path('hps/platform') and Path('hps/platform/hosted') in header.parents:
                    source += '#ifdef ' + public_guard(header) + '\n'
                    source += '#error "Platform umbrella imported hosted diagnostics"\n#endif\n'
                else:
                    source += '#ifndef ' + public_guard(header) + '\n'
                    source += '#error "Umbrella omitted ' + header.as_posix() + '"\n#endif\n'
            source += 'typedef void (*hps_header_translation_unit)(void);\n'
            compile_source(source, dialect)
            checks += 1
        for order in [headers, list(reversed(headers))]:
            source = ''.join('#include "' + header.as_posix() + '"\n' for header in order)
            source += 'typedef void (*hps_header_translation_unit)(void);\n'
            compile_source(source, dialect)
            checks += 1
        for name, (declarations, body) in GENERATORS.items():
            source = '#include "hps/data_structure/hps_' + name + '.h"\n'
            source += declarations + 'int main(void)\n{\n' + body + '}\n'
            compile_source(source, dialect)
            checks += 1
        for name, body in [
            ('collection', 'return HPS_COLLECTION_OK == INT32_C(0) ? 0 : 1;'),
            ('static_init', 'const char *value = HPS_DATA_STRUCTURE_EMPTY_INIT;\n'
                            '    return value == 0 ? 0 : 1;'),
        ]:
            source = '#include "hps/data_structure/hps_' + name + '.h"\n'
            source += 'int main(void)\n{\n    ' + body + '\n}\n'
            compile_source(source, dialect)
            checks += 1

    # Build implementations as C, then compile and link a caller as C++. Keeping
    # separate objects detects missing C linkage that include-only tests miss.
    with tempfile.TemporaryDirectory(prefix='hps-public-headers-') as temporary:
        output = Path(temporary)
        objects = []
        for source in sorted((ROOT / 'src').rglob('*.c')):
            relative = source.relative_to(ROOT / 'src')
            if relative == Path('main.c') or relative.parts[0] == 'platform':
                continue
            target = output / ('_'.join(relative.with_suffix('').parts) + '.o')
            checked(compiler + common + ['-std=c99', '-c', str(source), '-o', str(target)])
            objects.append(str(target))
        caller = output / 'caller.o'
        checked(compiler + common + ['-std=c++11', '-x', 'c++', '-c', '-', '-o', str(caller)],
                LINK_CALLER)
        executable = output / 'caller'
        checked(compiler + [str(caller)] + objects + ['-o', str(executable)])
        checked([str(executable)])
        checks += 1
    print(f'HPS public headers: {checks} checks passed; {len(headers)} headers, '
          f'C99/C++11 inclusion and C-to-C++ host link ({shlex.join(compiler)}).')


if __name__ == '__main__':
    main()
