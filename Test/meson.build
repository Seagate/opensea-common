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

# ---- Interactive secure input test ----
expect_prog = find_program('expect')

test(
  'opensea-common-unit-tests-with-secure-user-input',
  expect_prog,
  args: [join_paths(meson.current_source_dir(), 'unit', 'test_secure_input.exp'), e]
)
