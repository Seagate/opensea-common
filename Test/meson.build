# Meson build for opensea-common tests

test_sources = [
    'framework/test_framework.c',
    'unit/bit_manip_test.c',
    'unit/math_utils_test.c',
    'unit/string_utils_test.c',
    'unit/io_utils_test.c',
    'main.c',
]

e = executable(
    'opensea-common-tests',
    test_sources,
    dependencies: [opensea_common_dep],
)

test('opensea-common-unit-tests', e);

#---------------------for secure input----------------------------

secure_tests = executable(
    'secure-input-tests',
    ['unit/test_secure_inputs.c'],
    dependencies: [opensea_common_dep],
)

expect_prog = find_program('expect', required: true)

test(
  'secure-user-input-tests',
  expect_prog,
  args: [join_paths(meson.current_source_dir(), 'unit', 'test_secure_input.exp'), secure_tests],
  depends: secure_tests
)