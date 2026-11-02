#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

# разделяем аргументы: до "--" -> C++, после -> Python
CPP_ARGS=()
PY_ARGS=()
SEP="cpp"
for arg in "$@"; do
    if [[ "$arg" = "--" ]]; then SEP="py"; continue; fi
    if [[ "$SEP" = "cpp" ]]; then CPP_ARGS+=("$arg"); else PY_ARGS+=("$arg"); fi
done

# --- сборка ---
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. >/dev/null
cmake --build . -j"$(nproc)"
cd ..

# --- расчёт ---
echo ">>> C++ args: ${CPP_ARGS[*]:-<none>}"
./build/aaaaa "${CPP_ARGS[@]}"

# --- какой CSV читать ---
CSV="output.csv"
prev=""
for arg in "${CPP_ARGS[@]}"; do
    if [[ "$prev" = "--out" ]]; then CSV="$arg"; fi
    prev="$arg"
done

# --- график ---
echo ">>> plot.py args: ${PY_ARGS[*]:-<none>}"
python src/plot.py --csv "$CSV" "${PY_ARGS[@]}"
