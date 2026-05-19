# Meson build for opensea-common tests

test_sources = [
    'framework/test_framework.c',
    'unit/bit_manip_test.c',
    #'unit/math_utils_test.c',
    #'unit/string_utils_test.c',
    #'unit/io_utils_test.c',
    #'unit/memory_safety_test.c',
    #'unit/pattern_utils_test.c',
    #'unit/sort_and_search_test.c',
    #'unit/type_conversion_test.c',
    #'unit/unit_conversion_test.c',
    #'unit/precision_timer_and_sleep_test.c',
    #'unit/secure_file_test.c',
    'main.c',
]

e = executable(
    'opensea-common-tests',
    test_sources,
    dependencies: [opensea_common_dep],
)

test('opensea-common-unit-tests', e)

#---------------------for secure input----------------------------

#secure_tests = executable(
#    'secure-input-tests',
#    ['framework/test_framework.c', 'unit/test_secure_inputs.c', 'secure_input_main.c'],
#    dependencies: [opensea_common_dep],
#)

#expect_prog = find_program('expect', required: true)

#test(
#  'secure-user-input-tests',
#  expect_prog,
#  args: [
#    join_paths(meson.current_source_dir(), 'unit', 'test_secure_input.exp'),
#    secure_tests.full_path()
#  ],
#  workdir: meson.current_build_dir()
#)