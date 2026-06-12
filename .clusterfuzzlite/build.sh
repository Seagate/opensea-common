#!/bin/bash -eu

# Setup Meson to use ClusterFuzzLite's sanitizers and compilers
meson setup builddir \
  -Db_sanitize=none \
  -Dc_args="$CFLAGS" \
  -Dc_link_flags="$LDFLAGS" \
  --buildtype=debug

# Compile the target
meson compile -C builddir

# Copy the final binary to the $OUT folder (required by CFLite)
cp builddir/fuzz/fuzz_header $OUT/