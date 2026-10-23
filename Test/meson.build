# Meson build for opensea-common tests

test_sources = [
    'framework/test_framework.c',
    'unit/bit_manip_test.c',
    'unit/math_utils_test.c',
    'unit/string_utils_test.c',
    'unit/io_utils_test.c',
    'unit/memory_safety_test.c',
    'unit/pattern_utils_test.c',
    'unit/sort_and_search_test.c',
    'unit/type_conversion_test.c',
    'unit/unit_conversion_test.c',
    'unit/precision_timer_and_sleep_test.c',
    'unit/secure_file_test.c',
    'main.c',
]

e = executable(
    'opensea-common-tests',
    test_sources,
    dependencies: [opensea_common_dep],
    c_args: global_cpp_args,  # Access the same variable from parent scope
    override_options: ['werror=false'],
)

test('opensea-common-unit-tests', e)

if host_machine.system() == 'windows'
    windows_acl_tests = executable(
        'windows-acl-tests',
        ['windows_acl_test.c', 'framework/test_framework.c'],
        include_directories: include_directories('..\\src'),
        dependencies: [opensea_common_dep, deps, c.find_library('advapi32')],
        c_args: global_cpp_args,
    )
    test('windows-acl-tests', windows_acl_tests)
endif

if host_machine.system() == 'linux'
    path_policy_tests = executable(
        'path-security-tests',
        ['path_security_test.c', 'framework/test_framework.c'],
        dependencies: [opensea_common_dep],
        c_args: global_cpp_args,
    )
    test('path-security-unit-tests', path_policy_tests)
    test('path-security-isolated-tests', path_policy_tests, args: ['--integration'])
endif

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
