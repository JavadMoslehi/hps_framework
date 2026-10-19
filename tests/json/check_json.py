#!/usr/bin/env python3
"""Deterministic JSON grammar oracle and mutation checks on a hosted harness.

The generated harness alone uses stdio for test transport. Library inputs have
explicit lengths, caller-owned typed storage, 1024 tokens and 64 frames. This is
host regression/fuzz evidence, not exhaustive parser or target qualification.
"""
import argparse
import json
import os
from pathlib import Path
import random
import shlex
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler', nargs='+', default=[os.environ.get('CC', 'cc')])
parser.add_argument('--sanitize', action='store_true')
args = parser.parse_args()
compiler = [part for item in args.compiler for part in shlex.split(item)]
rng = random.Random(8259)

HARNESS = r'''
#include <stdio.h>
#include <stdint.h>
#include "hps/json/all.h"
#include "hps/string/fixed.h"
HPS_STRING_FIXED_DEFINE(test_input, 1025U)
HPS_STRING_FIXED_DEFINE(test_output, 1025U)

static uint8_t read_u32(uint32_t *value)
{
    uint8_t bytes[4U];
    uint32_t decoded = UINT32_C(0);
    if (fread(bytes, 1U, sizeof(bytes), stdin) != sizeof(bytes)) { return 0U; }
    for (size_t index = 0U; index < sizeof(bytes); ++index) {
        decoded |= (uint32_t)bytes[index] << (uint32_t)(index * 8U);
    }
    *value = decoded;
    return 1U;
}

static uint8_t write_u32(const uint32_t value)
{
    uint8_t bytes[4U];
    for (size_t index = 0U; index < sizeof(bytes); ++index) {
        bytes[index] = (uint8_t)((value >> (uint32_t)(index * 8U)) & UINT32_C(255));
    }
    return (uint8_t)(fwrite(bytes, 1U, sizeof(bytes), stdout) == sizeof(bytes));
}

int main(void)
{
    test_input_t input;
    test_output_t output;
    hps_string_buffer destination;
    hps_json_token tokens[1024U];
    hps_json_frame frames[64U];
    hps_json_document document;
    hps_string_view view;
    uint8_t decoded[1024U];
    uint32_t case_count = UINT32_C(0);
    int32_t result;
    /* This fixture uses the generated typed storage; adoption/other binding
     * helpers are outside the transport contract but remain valid declarations. */
    (void)test_input_capacity;
    (void)test_input_bind;
    (void)test_input_adopt;
    (void)test_output_capacity;
    (void)test_output_adopt;
    if (read_u32(&case_count) != 1U || case_count > UINT32_C(4096)) { return 2; }
    for (uint32_t case_index = UINT32_C(0); case_index < case_count; ++case_index) {
        uint32_t input_length = UINT32_C(0);
        size_t written = 0U;
        if (read_u32(&input_length) != 1U || input_length >= UINT32_C(1024)) { return 2; }
        const size_t length = (size_t)input_length;
        if (fread(input.data, 1U, length, stdin) != length) { return 2; }
        input.data[length] = '\0';
        view.data = input.data;
        view.length = length;
        result = hps_json_read(view, tokens, 1024U, frames, 64U, &document);
        if (result == HPS_JSON_SYNTAX) {
            if (write_u32(UINT32_C(1)) != 1U || write_u32(UINT32_C(0)) != 1U) { return 7; }
        } else {
            if (result != HPS_JSON_OK) { return 3; }
            for (size_t index = 0U; index < document.token_count; ++index) {
                if (tokens[index].kind == HPS_JSON_STRING) {
                    result = hps_json_token_decode_string(&tokens[index], decoded,
                                                            sizeof(decoded), &written);
                    if (result != HPS_JSON_OK || written > tokens[index].raw.length) {
                        return 4;
                    }
                }
            }
            if (test_output_bind(&output, &destination) != HPS_STRING_OK) { return 5; }
            result = hps_json_write_compact(&destination, view, frames, 64U);
            if (result != HPS_JSON_OK || destination.length >= 1024U) { return 6; }
            if (write_u32(UINT32_C(0)) != 1U ||
                write_u32((uint32_t)destination.length) != 1U ||
                fwrite(destination.data, 1U, destination.length, stdout)
                    != destination.length) { return 7; }
        }
    }
    if (ferror(stdin) != 0 || fflush(stdout) != 0) { return 7; }
    return 0;
}
'''


def value(depth=0):
    scalars = [None, True, False, rng.randrange(-(2**63), 2**63),
               ''.join(rng.choice('ab \\"\n\t\x00éی😀')
                       for _ in range(rng.randrange(12)))]
    if depth >= 3 or rng.randrange(3) == 0:
        return rng.choice(scalars)
    if rng.randrange(2) == 0:
        return [value(depth + 1) for _ in range(rng.randrange(4))]
    return {str(index) + rng.choice(['', 'é', 'ی']): value(depth + 1)
            for index in range(rng.randrange(4))}


def strict_load(data):
    # CPython also accepts NaN/Infinity and lone surrogates by default; generated
    # cases contain neither, and constants are explicitly rejected by this oracle.
    def reject_constant(_):
        raise ValueError('non-JSON numeric constant')
    result = json.loads(data.decode('utf-8'), parse_constant=reject_constant)
    def check_unicode(item):
        if isinstance(item, str):
            if any(0xD800 <= ord(character) <= 0xDFFF for character in item):
                raise ValueError('unpaired surrogate outside HPS subset')
        elif isinstance(item, list):
            for child in item:
                check_unicode(child)
        elif isinstance(item, dict):
            for key, child in item.items():
                check_unicode(key)
                check_unicode(child)
    check_unicode(result)
    return result


cases = []
for _ in range(180):
    data = json.dumps(value(), ensure_ascii=bool(rng.randrange(2)),
                      separators=rng.choice([(',', ':'), (', ', ': ')])).encode()
    cases.append((data, True))
    # Exercise deletion, insertion, replacement and truncation. The independent
    # oracle decides validity; a mutation can legitimately remain valid JSON.
    for operation in range(4):
        position = rng.randrange(len(data) + 1)
        if operation == 0:
            changed = data[:position] + data[position + 1:]
        elif operation == 1:
            changed = data[:position] + rng.choice([b',', b':', b'"', b'\\', b'0']) + data[position:]
        elif operation == 2:
            changed = data[:position] + b'\x00' + data[position + 1:]
        else:
            changed = data[:position]
        try:
            strict_load(changed)
            expected = True
        except (ValueError, UnicodeError):
            expected = False
        cases.append((changed, expected))

for data in [b'-0', b'1e9999', b'-1.2e-999', b'0.0', b'1E+1',
             b'{"x":1,"x":2}', b'"\\uD83D\\uDE00"', b'"\\u0000"',
             b'\r\n [\ttrue , false, null\r] ']:
    cases.append((data, True))
for data in [b'', b'-', b'00', b'01', b'1.', b'.1', b'1e', b'1e+', b'+1',
             b'NaN', b'Infinity', b'trueX', b'null0', b'[1,]', b'{"x":0,}',
             b'{}{}', b'[1 2]', b'{1:2}', b'/*x*/0', b'//x\n0', b'\x0b0',
             b'\x0c0', b'\xef\xbb\xbf0', b'"\\q"', b'"\\uD800"',
             b'"\\uDC00"', b'"\\uD800\\u0041"', b'"\xc0\x80"',
             b'"\xed\xa0\x80"', b'"\xf4\x90\x80\x80"', b'"\xff"',
             b'"\xe2\x82"', b'"line\nfeed"']:
    cases.append((data, False))

with tempfile.TemporaryDirectory(prefix='hps-json-oracle-') as directory:
    executable = Path(directory) / 'json_oracle'
    flags = ['-std=c99', '-Wall', '-Wextra', '-Wpedantic', '-Werror']
    if args.sanitize:
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
    sources = sorted((ROOT / 'src/json').glob('*.c'))
    sources += sorted((ROOT / 'src/string').glob('*.c'))
    command = compiler + flags + ['-I', str(ROOT / 'include'), '-x', 'c', '-']
    command += [str(source) for source in sources] + ['-o', str(executable)]
    subprocess.run(command, input=HARNESS.encode(), check=True, timeout=60)
    if len(cases) > 4096:
        raise RuntimeError('Fixture exceeds explicit harness case budget')
    request = bytearray(struct.pack('<I', len(cases)))
    for data, _ in cases:
        if len(data) >= 1024:
            raise RuntimeError('Fixture exceeds explicit harness input budget')
        request += struct.pack('<I', len(data))
        request += data
    result = subprocess.run([str(executable)], input=request, capture_output=True,
                            timeout=30)
    if result.returncode != 0:
        raise RuntimeError(f'Batched harness returned {result.returncode}\n'
                           + result.stderr.decode(errors='replace'))
    offset = 0
    for index, (data, expected) in enumerate(cases):
        if len(result.stdout) - offset < 8:
            raise RuntimeError(f'case {index}: truncated response header')
        status, length = struct.unpack_from('<II', result.stdout, offset)
        offset += 8
        if status not in (0, 1) or length >= 1024 or length > len(result.stdout) - offset:
            raise RuntimeError(f'case {index}: invalid response status/length')
        compact = result.stdout[offset:offset + length]
        offset += length
        if status != (0 if expected else 1) or (status == 1 and length != 0):
            raise RuntimeError(f'case {index}: expected valid={expected}, '
                               f'status={status}, input={data!r}\n'
                               + result.stderr.decode(errors='replace'))
        if expected:
            if strict_load(compact) != strict_load(data):
                raise RuntimeError(f'Compact writer changed value: {data!r}')
    if offset != len(result.stdout):
        raise RuntimeError('Unexpected trailing harness response bytes')
print(f'HPS JSON: {len(cases)} deterministic oracle/mutation checks passed '
      f'({" ".join(compiler)}; sanitize={args.sanitize}).')
