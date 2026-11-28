#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

# Разделяем аргументы: до первого "--" -> C++, после -> Python.
# Второй и последующие "--" передаются в Python как есть.
CPP_ARGS=()
PY_ARGS=()
SEP="cpp"
for arg in "$@"; do
    if [ "$SEP" = "cpp" ] && [ "$arg" = "--" ]; then
        SEP="py"
        continue
    fi
    if [ "$SEP" = "cpp" ]; then
        CPP_ARGS+=("$arg")
    else
        PY_ARGS+=("$arg")
    fi
done

# Есть ли --help/-h? Тогда C++ только напечатает справку,
# и запускать plot.py на старом output.csv смысла нет.

for arg in ${CPP_ARGS[@]+"${CPP_ARGS[@]}"}; do
    if [ "$arg" = "--help" ] || [ "$arg" = "-h" ]; then
        echo ">>> --help/-h detected in C++ args; building and showing help only."
        mkdir -p build
        cd build
        cmake -DCMAKE_BUILD_TYPE=Release .. >/dev/null
        cmake --build . -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)"
        cd ..
        ./build/aaaaa "${CPP_ARGS[@]}"
        exit 0
    fi
done

# Выбираем интерпретатор Python. Я это добавил, так как не по-умолчанию на линуксе есть питон, в большинстве случаев только питон3
if   command -v python3 >/dev/null 2>&1; then PYTHON=python3
elif command -v python  >/dev/null 2>&1; then PYTHON=python
else
    echo "ERROR: neither python3 nor python found in PATH" >&2
    exit 1
fi


# Сборка.
JOBS="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. >/dev/null
cmake --build . -j"$JOBS"
cd ..


# Расчёт.
echo ">>> C++ args: ${CPP_ARGS[*]:-<none>}"
./build/aaaaa ${CPP_ARGS[@]+"${CPP_ARGS[@]}"}

# Какой CSV читать plot.py: последний --out в CPP_ARGS, иначе output.csv.
CSV="output.csv"
prev=""
for arg in ${CPP_ARGS[@]+"${CPP_ARGS[@]}"}; do
    if [ "$prev" = "--out" ]; then
        CSV="$arg"
    fi
    prev="$arg"
done


# График.
echo ">>> plot.py args: ${PY_ARGS[*]:-<none>}"
"$PYTHON" src/plot.py --csv "$CSV" ${PY_ARGS[@]+"${PY_ARGS[@]}"}
