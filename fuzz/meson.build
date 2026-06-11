# fuzz/meson.build

fuzz_sources = files(
  'fuzz_bit_floor_uc.cpp',
  'fuzz_bit_floor_us.cpp'
)

foreach src : fuzz_sources
  exe_name = src.stem()
  fuzz_exe = executable(exe_name, src, dependencies: [dep_yourlib])
  test(exe_name, fuzz_exe, timeout: 10)  # optional: run in CI for 10s
endforeach