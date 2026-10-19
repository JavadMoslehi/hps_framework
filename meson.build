project('hps', 'c', version: '0.1.0',
  default_options: ['c_std=c99', 'warning_level=3'])

hps_base = declare_dependency(include_directories: include_directories('include'))
meson.override_dependency('hps-base', hps_base)
python = find_program('python3', required: false)
cc = meson.get_compiler('c')
if python.found() and cc.get_id() in ['gcc', 'clang']
  test('hps_base_portability', python,
    args: [files('tests/base/check_portability.py'), '--compiler'] + cc.cmd_array(),
    timeout: 120)
  test('hps_environment_detection', python,
    args: [files('tests/base/check_detection.py'), '--compiler'] + cc.cmd_array(),
    timeout: 120)
endif
install_subdir('include/hps', install_dir: get_option('includedir'))

hps_memory_library = static_library('hps_memory', 'src/memory/pool.c',
  'src/memory/operations.c',
  dependencies: hps_base, install: true)
hps_memory = declare_dependency(link_with: hps_memory_library,
  dependencies: hps_base)
meson.override_dependency('hps-memory', hps_memory)

hps_bytes_library = static_library('hps_bytes', 'src/bytes/view.c', 'src/bytes/buffer.c', 'src/bytes/codec.c', 'src/bytes/endian.c', 'src/bytes/reader.c',
  dependencies: hps_base, install: true)
hps_bytes = declare_dependency(link_with: hps_bytes_library,
  dependencies: hps_base)
meson.override_dependency('hps-bytes', hps_bytes)

hps_string_library = static_library('hps_string', 'src/string/view.c', 'src/string/buffer.c',
  'src/string/char.c', 'src/string/builder.c', 'src/string/policy.c',
  dependencies: hps_base, install: true)
hps_string = declare_dependency(link_with: hps_string_library,
  dependencies: hps_base)
meson.override_dependency('hps-string', hps_string)

hps_cstr_library = static_library('hps_cstr', 'src/cstr/core.c',
  'src/cstr/convert.c', 'src/cstr/algorithm.c', 'src/cstr/format.c',
  dependencies: hps_string, install: true)
hps_cstr = declare_dependency(link_with: hps_cstr_library,
  dependencies: hps_string)
meson.override_dependency('hps-cstr', hps_cstr)

hps_validation_library = static_library('hps_validation', 'src/validation/national_code.c',
  dependencies: hps_string, install: true)
hps_validation = declare_dependency(link_with: hps_validation_library,
  dependencies: hps_string)
meson.override_dependency('hps-validation', hps_validation)

test_assertion_args = cc.get_id() in ['msvc', 'clang-cl'] ? ['/UNDEBUG'] : ['-UNDEBUG']
hps_tests = executable('hps_tests', 'tests/main.c', 'tests/base/test_base.c',
  'tests/memory/test_pool.c', 'tests/memory/test_memory_operations.c',
  'tests/memory/test_memory_clone.c', 'tests/bytes/test_bytes.c', 'tests/bytes/test_reader.c',
  'tests/enum/test_enum.c', 'tests/string/test_string.c',
  'tests/string/test_string_extended.c', 'tests/string/test_string_builder.c',
  'tests/span/test_span.c', 'tests/data_structure/test_data_structure.c',
  'tests/cstr/test_cstr.c', 'tests/cstr/test_cstr_convert.c',
  'tests/cstr/test_cstr_format.c', 'tests/validation/test_national_code.c',
  'tests/enum/test_cstr_enum.c',
  dependencies: [hps_base, hps_memory, hps_bytes, hps_string,
    hps_cstr, hps_validation], c_args: test_assertion_args)
test('hps_tests', hps_tests)
