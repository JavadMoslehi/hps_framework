project('hps', 'c', version: '0.1.0',
  default_options: ['c_std=c99', 'warning_level=3'])

hps_base = declare_dependency(include_directories: include_directories('include'))
meson.override_dependency('hps-base', hps_base)
hps_base_test = executable('hps_base_test', 'tests/base/test_base.c',
  dependencies: hps_base)
test('hps_base', hps_base_test)
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
hps_pool_test = executable('hps_pool_test', 'tests/memory/test_pool.c',
  dependencies: hps_memory, c_args: ['-UNDEBUG'])
test('hps_pool', hps_pool_test)

hps_bytes_library = static_library('hps_bytes', 'src/bytes/view.c', 'src/bytes/buffer.c',
  dependencies: hps_base, install: true)
hps_bytes = declare_dependency(link_with: hps_bytes_library,
  dependencies: hps_base)
meson.override_dependency('hps-bytes', hps_bytes)
hps_bytes_test = executable('hps_bytes_test', 'tests/bytes/test_bytes.c',
  dependencies: hps_bytes, c_args: ['-UNDEBUG'])
test('hps_bytes', hps_bytes_test)
