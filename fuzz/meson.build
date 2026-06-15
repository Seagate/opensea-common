fuzzing_engine = run_command('sh', '-c', 'echo $LIB_FUZZING_ENGINE', check: true).stdout().strip()
fuzz_args = fuzzing_engine.split()
fuzz_link_args = fuzz_args + ['-fsanitize=address']

fuzz_targets = [
  'bit_manip/bit_manip_fuzzer_generic_input',
  'bit_manip/bit_manip_fuzzer_64bit_input',
  'bit_manip/bit_manip_fuzzer_float_input'
]

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

# Pass all paths to Python as a single string to loop over them safely
targets_str = ','.join(fuzz_targets)
run_command('python3', '-c', f'''
import shutil, os
for path in "{targets_str}".split(","):
    src_corpus = path + "_corpus"
    binary_name = path.split("/")[-1]
    dest_corpus = os.path.join("build_dir_corpuses", binary_name + "_corpus")
    
    if os.path.exists(src_corpus):
        shutil.copytree(src_corpus, dest_corpus, dirs_exist_ok=True)
''', check: false)