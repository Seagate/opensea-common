fuzzing_engine = run_command('sh', '-c', 'echo $LIB_FUZZING_ENGINE', check: true).stdout().strip()
fuzz_args = fuzzing_engine.split()
fuzz_link_args = fuzz_args + ['-fsanitize=address']

fuzz_targets = [
  'bit_manip_fuzzer_generic_input',
  'bit_manip_fuzzer_64bit_input',
  'bit_manip_fuzzer_float_input'
]

foreach target : fuzz_targets
  executable(target,
    target + '.c',
    dependencies: [opensea_common_dep],
    c_args: fuzz_args,
    link_args: fuzz_link_args, 
    install: false
  )
endforeach

run_command('python3', '-c', 'import shutil, sys; shutil.copytree("bit_manip_fuzzer_generic_input_corpus", "build_corpus", dirs_exist_ok=True)', check: false)