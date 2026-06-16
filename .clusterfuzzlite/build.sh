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
  "bit_manip_fuzzer_32bit_input"
  "bit_manip_fuzzer_8bit_input"
  "bit_manip_fuzzer_float_input"
)

# Automated loop to pack binaries and their respective seeds
for fuzzer in "${FUZZERS[@]}"; do
  # 1. Copy the executable binary to $OUT
  cp "builddir/fuzz/$fuzzer" "$OUT/"
  
  # 2. Safely copy its matching corpus directory if it exists
  CORPUS_DIR=$(find fuzz/ -type d -name "${fuzzer}_corpus")
  if [ -n "$CORPUS_DIR" ]; then
    cp -r "$CORPUS_DIR" "$OUT/"
  fi

  # Copy the matching dictionary file if present
  DICT_FILE=$(find fuzz/ -type f -name "${fuzzer}.dict")
  if [ -n "$DICT_FILE" ]; then
    cp "$DICT_FILE" "$OUT/"
  fi
done