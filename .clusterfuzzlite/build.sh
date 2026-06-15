#!/bin/bash -eu

# Clean up any leftover configuration caches
rm -rf builddir

# Setup Meson to build your pure C fuzzers
meson setup builddir \
  -Db_sanitize=none \
  -Dc_args="$CFLAGS" \
  -Dc_link_flags="${LDFLAGS:-}" \
  --buildtype=debug

# Compile the target
meson compile -C builddir

# Add fuzz targets here
FUZZERS=(
  "bit_manip_fuzzer_generic_input" 
  "bit_manip_fuzzer_64bit_input"
  "bit_manip_fuzzer_float_input"
)

# Automated loop to pack binaries and their respective seeds
for fuzzer in "${FUZZERS[@]}"; do
  # 1. Copy the executable binary to $OUT
  cp "builddir/fuzz/$fuzzer" "$OUT/"
  
  # 2. Safely copy its matching corpus directory if it exists
  if [ -d "fuzz/${fuzzer}_corpus" ]; then
    cp -r "fuzz/${fuzzer}_corpus" "$OUT/"
  fi

  # Copy the matching dictionary file if present
  if [ -f "fuzz/${fuzzer}.dict" ]; then
    cp "fuzz/${fuzzer}.dict" "$OUT/"
  fi
done