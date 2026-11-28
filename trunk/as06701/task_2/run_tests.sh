#!/usr/bin/env bash
# run_tests.sh — сборка + Google Test + gcovr (Linux / Arch).
# Использование: ./run_tests.sh [-h]
set -euo pipefail

cd "$(dirname "$0")"

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
    cat <<'EOF'
Usage: ./run_tests.sh
  Собирает проект с покрытием, запускает ctest, генерирует coverage.html.
Переменные окружения:
  JOBS=N   — число параллельных сборок (по умолчанию: nproc)
EOF
    exit 0
fi

JOBS="${JOBS:-$(nproc)}"
BUILD_DIR="build"

echo " Configure + Build (jobs=$JOBS)"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_COVERAGE=ON
cmake --build . -j"$JOBS"

echo
echo " Running Google Test"
ctest --output-on-failure

echo
echo " Coverage (gcovr)"
if command -v gcovr >/dev/null 2>&1; then
    # Текстовый отчёт
    gcovr -r .. --filter '../src/' --exclude '.*main\.cpp' --txt

    # HTML-отчёт
    gcovr -r .. --filter '../src/' --exclude '.*main\.cpp' --html-details coverage.html
    echo
    echo "HTML report: $(pwd)/coverage.html"
else

    echo "[warn] gcovr not found. Install: sudo pacman -S gcovr"
    echo "       Skipping coverage step."
fi

cd ..
echo
echo "ALL TESTS PASSED."
