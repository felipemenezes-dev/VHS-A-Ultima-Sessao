#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
RAYLIB_PREFIX="${RAYLIB_PREFIX:-$HOME/.local}"
mkdir -p build
gcc -std=c99 -Wall -Wextra -Wpedantic -O2 tests/test_base.c -o build/test_base \
    -I"$RAYLIB_PREFIX/include" -L"$RAYLIB_PREFIX/lib" \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
timeout 40s ./build/test_base
