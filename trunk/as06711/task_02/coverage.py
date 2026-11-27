#!/usr/bin/env python3
"""Сбор отчёта о покрытии из данных gcov (без внешних Python-пакетов)."""

import argparse
import pathlib
import re
import subprocess
import sys


def collect(root: pathlib.Path, build: pathlib.Path, gcov: str) -> dict:
    # Защита от Command Injection (S2076) и Path Traversal (S2083)
    cwd = pathlib.Path.cwd().resolve()
    resolved_build = (cwd / build).resolve()
    
    if not str(resolved_build).startswith(str(cwd)):
        raise ValueError("Build path must be within the current directory")

    cov_dir = resolved_build / "coverage"
    cov_dir.mkdir(parents=True, exist_ok=True)
    for old in cov_dir.glob("*.gcov"):
        old.unlink()

    gcda_files = sorted((resolved_build / "test" / "CMakeFiles" / "task02_tests.dir").glob("*.gcda"))
    if not gcda_files:
        raise RuntimeError("Не найдены .gcda файлы – тесты не запускались с ENABLE_COVERAGE=ON")

    for gcda in gcda_files:
        # subprocess с shell=False и списком аргументов безопасен
        subprocess.run([gcov, "-b", "-c", "-f", str(gcda)],
                       cwd=cov_dir, capture_output=True, text=True, check=True)

    stats = {}
    func_re = re.compile(r"^function .* called (\d+)")

    for gcov_file in cov_dir.glob("*.gcov"):
        text = gcov_file.read_text(encoding="utf-8", errors="replace")
        source = None
        lines_total = lines_covered = 0
        funcs_total = funcs_covered = 0

        for line in text.splitlines():
            parts = line.split(":", 2)
            if len(parts) == 3 and parts[1].strip() == "0" \
                    and parts[2].startswith("Source:"):
                source = pathlib.Path(parts[2][len("Source:"):].strip())
                continue
            match = func_re.match(line)
            if match:
                funcs_total += 1
                if int(match.group(1)) > 0:
                    funcs_covered += 1
                continue
            parts = line.split(":", 2)
            if len(parts) < 3:
                continue
            count = parts[0].strip()
            if count.startswith("-"):
                continue
            if count in ("#####", "====="):
                if count == "#####":
                    lines_total += 1
                continue
            if count.replace("*", "").isdigit():
                lines_total += 1
                if int(count.replace("*", "")) > 0:
                    lines_covered += 1

        if source is None:
            continue
        try:
            source.relative_to(root / "src")
        except ValueError:
            continue  # системные заголовки и GoogleTest не учитываем

        name = source.name
        item = stats.setdefault(name, [0, 0, 0, 0])
        item[0] += lines_total
        item[1] += lines_covered
        item[2] += funcs_total
        item[3] += funcs_covered

    return stats


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--build", default="build")
    parser.add_argument("--gcov", default="gcov")
    args = parser.parse_args()

    root = pathlib.Path(__file__).resolve().parent
    build = pathlib.Path(args.build)
    if not build.is_absolute():
        build = root / build

    stats = collect(root, build, args.gcov)
    if not stats:
        raise RuntimeError("Не удалось собрать статистику покрытия исходников")

    def pct(covered, total):
        return 100.0 * covered / total if total else 0.0

    report = []
    report.append("ОТЧЁТ О ПОКРЫТИИ КОДОМ (gcov, ENABLE_COVERAGE=ON)")
    report.append(f"Инструмент: {args.gcov}")
    report.append("")
    report.append("| Модуль | Строки | Покрытие строк | Функции | Покрытие функций |")
    report.append("| :--- | ---: | ---: | ---: | ---: |")

    total_lines = total_lines_cov = total_funcs = total_funcs_cov = 0
    for name in sorted(stats):
        lt, lc, ft, fc = stats[name]
        total_lines += lt
        total_lines_cov += lc
        total_funcs += ft
        total_funcs_cov += fc
        report.append(f"| `{name}` | {lc}/{lt} | {pct(lc, lt):.1f} % | "
                      f"{fc}/{ft} | {pct(fc, ft):.1f} % |")

    report.append(f"| **ИТОГО** | **{total_lines_cov}/{total_lines}** | "
                  f"**{pct(total_lines_cov, total_lines):.1f} %** | "
                  f"**{total_funcs_cov}/{total_funcs}** | "
                  f"**{pct(total_funcs_cov, total_funcs):.1f} %** |")
    report.append("")
    report.append("Примечание: ветки, помеченные gcov как `=====` (обработка исключений), "
                  "в строковое покрытие не включаются.")

    text = "\n".join(report) + "\n"
    out = root / "doc" / "coverage_report.txt"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(text, encoding="utf-8")
    print(text)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as exc:
        print(f"[coverage] Ошибка: {exc}", file=sys.stderr)
        sys.exit(1)