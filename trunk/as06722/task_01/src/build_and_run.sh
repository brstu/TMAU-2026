#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
echo "=== Configure CMake ==="
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
echo "=== Build project ==="
cmake --build build --config Release
echo "=== Run simulation ==="
if [ -x "./build/task01" ]; then
    ./build/task01
else
    ./build/Release/task01
fi
