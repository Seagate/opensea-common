# Get LIB_FUZZING_ENGINE environment variable (cross-platform compatible)
if target_machine.system() == 'windows'
  fuzzing_engine = run_command('cmd', '/c', 'echo %LIB_FUZZING_ENGINE%', check: true).stdout().strip()
  # On Windows, if the variable is not set, echo returns the literal string %VAR%
  if fuzzing_engine.startswith('%')
    fuzzing_engine = ''
  endif
else
  fuzzing_engine = run_command('sh', '-c', 'echo $LIB_FUZZING_ENGINE', check: true).stdout().strip()
endif

# Set up fuzzing flags (only apply Unix-specific sanitizers on non-Windows platforms)
if fuzzing_engine != ''
  fuzz_args = fuzzing_engine.split()
  if target_machine.system() != 'windows'
    fuzz_link_args = fuzz_args + ['-fsanitize=address']
  else
    fuzz_link_args = fuzz_args
  endif
else
  fuzz_args = []
  fuzz_link_args = []
endif

fuzz_targets = [
  'bit_manip/bit_manip_fuzzer_generic_input',
  'bit_manip/bit_manip_fuzzer_64bit_input',
  'bit_manip/bit_manip_fuzzer_32bit_input',
  'bit_manip/bit_manip_fuzzer_16bit_input',
  'bit_manip/bit_manip_fuzzer_8bit_input',
  'bit_manip/bit_manip_fuzzer_float_input'
]

# Only build fuzz targets when fuzzing is available
# On Unix: always build, On Windows: only if LIB_FUZZING_ENGINE is set
if target_machine.system() != 'windows' or fuzzing_engine != ''
  foreach target_path : fuzz_targets
    path_segments = target_path.split('/')
    binary_name = path_segments[-1]

    executable(binary_name,
      target_path + '.c',
      dependencies: [opensea_common_dep],
      c_args: fuzz_args,
      link_args: fuzz_link_args, 
      install: false
    )
  endforeach
endif

# Find Python interpreter (cross-platform compatible)
python_exe = find_program('python3', 'python', required: false)
if python_exe.found()
  targets_str = ','.join(fuzz_targets)
  python_script = '''
  import shutil, os
  for path in "''' + targets_str + '''".split(","):
      src_corpus = path + "_corpus"
      binary_name = path.split("/")[-1]
      dest_corpus = os.path.join("build_dir_corpuses", binary_name + "_corpus")
      
      if os.path.exists(src_corpus):
          shutil.copytree(src_corpus, dest_corpus, dirs_exist_ok=True)
  '''
  run_command(python_exe, '-c', python_script, check: false)
endif