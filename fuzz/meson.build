# fuzz/meson.build

fs = import('fs')

fuzz_sources = files(
  'fuzz_bit_manip.cpp',
)

foreach src : fuzz_sources
  exe_name = fs.stem(src)
  fuzz_exe = executable(exe_name, src, dependencies: [opensea_common_dep])
  test(exe_name, fuzz_exe, timeout: 10)
endforeach