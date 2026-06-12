fuzzing_engine = run_command('sh', '-c', 'echo $LIB_FUZZING_ENGINE', check: true).stdout().strip()
fuzz_args = fuzzing_engine.split()

executable('bit_manip_fuzzer',
  'bit_manip_fuzzer.c',
  dependencies: [opensea_common_dep],
  c_args: fuzz_args,
  link_args: fuzz_args, 
  install: false
)