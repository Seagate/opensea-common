# fuzz/meson.build

fuzz_sources = files(
  'fuzz_bit_floor_uc.cpp',
  'fuzz_bit_floor_us.cpp'
)

foreach src : fuzz_sources
  exe_name = src.stem()
  fuzz_exe = executable(exe_name, src, dependencies: [opensea_common_dep])
  test(exe_name, fuzz_exe, timeout: 10)
endforeach