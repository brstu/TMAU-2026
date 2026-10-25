#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
echo "=== Configure CMake ==="
cmake -S . -B build
echo "=== Build project ==="
cmake --build build --config Release
echo "=== Run simulation ==="
./build/task01
echo "=== Plot results ==="
python3 src/plot.py results.csv
