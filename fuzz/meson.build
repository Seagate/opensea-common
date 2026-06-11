project('bitmanip-fuzz', 'c', 'cpp',
  default_options: ['cpp_std=c++17']
)

# Include directory for headers
inc = include_directories('include')

# Fuzzing compiler flags
fuzz_flags = [
  '-fsanitize=fuzzer,address,undefined'
]

# All fuzz harnesses live in fuzz/
# This will find all files starting with fuzz_ and ending with .cpp
fuzz_sources = files()
foreach f : meson.get_compiler('cpp').find_sources('fuzz', '*.cpp')
  fuzz_sources += f
endforeach

# Build one executable per fuzz harness
foreach fuzz_file : fuzz_sources
  # Extract base name without extension
  fuzz_name = fuzz_file.stem()

  executable(
    fuzz_name,
    [fuzz_file, 'src/bit_manip.c'],  # Add more C files if needed
    include_directories: inc,
    cpp_args: fuzz_flags,
    c_args: fuzz_flags,
    link_args: fuzz_flags
  )
endforeach
