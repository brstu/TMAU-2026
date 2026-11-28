add_library(as06809_var16_lib STATIC main.cpp StabilityAnalyzer.cpp)
target_compile_definitions(as06809_var16_lib PRIVATE VAR16_STATIC_LIB)
target_compile_features(as06809_var16_lib PUBLIC cxx_std_17)
target_include_directories(as06809_var16_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(as06809_var16_app main.cpp)
set_target_properties(as06809_var16_app PROPERTIES OUTPUT_NAME task02)
target_link_libraries(as06809_var16_app PRIVATE as06809_var16_lib)
target_compile_definitions(as06809_var16_app PRIVATE
    VAR16_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/.."
    VAR16_CHART_SCRIPT="${CMAKE_CURRENT_BINARY_DIR}/build_charts.py"
)

foreach(item as06809_var16_lib as06809_var16_app)
    if(MSVC)
        target_compile_options(${item} PRIVATE /W4 /utf-8)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${item} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

if(VAR16_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "VAR16_COVERAGE requires GNU GCC.")
    endif()
    target_compile_options(as06809_var16_lib PRIVATE --coverage -O0 -g)
    target_link_options(as06809_var16_lib INTERFACE --coverage)
endif()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/build_charts.py" [=[
import csv
import sys
from pathlib import Path

try:
    import matplotlib.pyplot as plt
except ImportError:
    print("Модуль matplotlib не установлен. Выполните: py -m pip install matplotlib")
    sys.exit(1)


def locate_csv(raw_arg=None):
    if raw_arg:
        candidate = Path(raw_arg).expanduser()
        return candidate.resolve() if candidate.is_file() else None

    root = Path(__file__).resolve().parent.parent
    for path in (Path.cwd() / "results.csv", root / "results.csv"):
        if path.is_file():
            return path.resolve()
    return None


def main():
    csv_path_arg = sys.argv[1] if len(sys.argv) >= 2 and sys.argv[1] not in {"1", "2", "3"} else None
    model_idx = sys.argv[2] if len(sys.argv) >= 3 else (sys.argv[1] if len(sys.argv) >= 2 else None)

    csv_file = locate_csv(csv_path_arg)
    if csv_file is None or model_idx not in {"1", "2", "3"}:
        print("Запуск: py build_charts.py <results.csv> <1|2|3>")
        sys.exit(1)

    with csv_file.open("r", newline="", encoding="utf-8-sig") as handle:
        records = list(csv.DictReader(handle))

    steps = [int(r["tau"]) for r in records]
    controls = [float(r["u"]) for r in records]
    states = [float(r["y"]) for r in records]

    fig, (top_ax, bot_ax) = plt.subplots(2, 1, figsize=(10, 7), sharex=True)
    top_ax.plot(steps, controls, marker="o", markersize=3, color="#2ca02c", label="u(tau)")
    top_ax.set_title("Входной сигнал u(tau)")
    top_ax.set_ylabel("u")
    top_ax.grid(True)
    top_ax.legend()

    bot_ax.plot(steps, states, marker="o", markersize=3, color="#9467bd", label="y(tau)")
    bot_ax.set_title("Отклик системы y(tau) — Вариант 16")
    bot_ax.set_xlabel("Дискретный такт tau")
    bot_ax.set_ylabel("y")
    bot_ax.grid(True)
    bot_ax.legend()

    fig.tight_layout()
    filenames = {"1": "Model1.6.png", "2": "Model2.10.png", "3": "Model3.4.png"}
    doc_folder = csv_file.parent / "doc"
    doc_folder.mkdir(parents=True, exist_ok=True)
    target_png = doc_folder / filenames[model_idx]
    fig.savefig(target_png, dpi=150, bbox_inches="tight")
    print(f"График сохранён: {target_png}")
    plt.show()


if __name__ == "__main__":
    main()
]=])
