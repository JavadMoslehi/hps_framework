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
endif
install_subdir('include/hps', install_dir: get_option('includedir'))

hps_memory_library = static_library('hps_memory', 'src/memory/pool.c',
  dependencies: hps_base, install: true)
hps_memory = declare_dependency(link_with: hps_memory_library,
  dependencies: hps_base)
meson.override_dependency('hps-memory', hps_memory)

hps_bytes_library = static_library('hps_bytes', 'src/bytes/view.c', 'src/bytes/buffer.c', 'src/bytes/endian.c',
  dependencies: hps_base, install: true)
hps_bytes = declare_dependency(link_with: hps_bytes_library,
  dependencies: hps_base)
meson.override_dependency('hps-bytes', hps_bytes)

test_assertion_args = cc.get_id() in ['msvc', 'clang-cl'] ? ['/UNDEBUG'] : ['-UNDEBUG']
hps_tests = executable('hps_tests', 'tests/main.c', 'tests/base/test_base.c',
  'tests/memory/test_pool.c', 'tests/bytes/test_bytes.c',
  dependencies: [hps_base, hps_memory, hps_bytes], c_args: test_assertion_args)
test('hps_tests', hps_tests)
