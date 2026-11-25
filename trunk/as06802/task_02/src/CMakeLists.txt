add_library(as06802_var17_lib STATIC main.cpp StabilityAnalyzer.cpp)
target_compile_definitions(as06802_var17_lib PRIVATE VAR17_BUILD_LIBRARY)
target_compile_features(as06802_var17_lib PUBLIC cxx_std_17)
target_include_directories(as06802_var17_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(as06802_var17_app main.cpp)
set_target_properties(as06802_var17_app PROPERTIES OUTPUT_NAME task02)
target_link_libraries(as06802_var17_app PRIVATE as06802_var17_lib)
target_compile_definitions(as06802_var17_app PRIVATE
    VAR17_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/.."
    VAR17_PLOT_SCRIPT="${CMAKE_CURRENT_BINARY_DIR}/render_plot.py"
)

foreach(tgt as06802_var17_lib as06802_var17_app)
    if(MSVC)
        target_compile_options(${tgt} PRIVATE /W4 /utf-8)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${tgt} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

if(ENABLE_GCC_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "ENABLE_GCC_COVERAGE requires GNU GCC compiler.")
    endif()
    target_compile_options(as06802_var17_lib PRIVATE --coverage -O0 -g)
    target_link_options(as06802_var17_lib INTERFACE --coverage)
endif()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/render_plot.py" [=[
import csv
import sys
from pathlib import Path

try:
    import matplotlib.pyplot as plt
except ImportError:
    print("Библиотека matplotlib не найдена. Установите: py -m pip install matplotlib")
    sys.exit(1)


def resolve_csv(arg=None):
    if arg:
        p = Path(arg).expanduser()
        return p.resolve() if p.is_file() else None

    base = Path(__file__).resolve().parent.parent
    for candidate in (Path.cwd() / "results.csv", base / "results.csv"):
        if candidate.is_file():
            return candidate.resolve()
    return None


def main():
    csv_arg = sys.argv[1] if len(sys.argv) >= 2 and sys.argv[1] not in {"1", "2", "3"} else None
    mdl_id = sys.argv[2] if len(sys.argv) >= 3 else (sys.argv[1] if len(sys.argv) >= 2 else None)

    csv_file = resolve_csv(csv_arg)
    if csv_file is None or mdl_id not in {"1", "2", "3"}:
        print("Использование: py render_plot.py <results.csv> <1|2|3>")
        sys.exit(1)

    with csv_file.open("r", newline="", encoding="utf-8-sig") as fh:
        rows = list(csv.DictReader(fh))

    tau = [int(r["tau"]) for r in rows]
    u_vals = [float(r["u"]) for r in rows]
    y_vals = [float(r["y"]) for r in rows]

    fig, (ax_u, ax_y) = plt.subplots(2, 1, figsize=(10, 7), sharex=True)
    ax_u.plot(tau, u_vals, marker="o", markersize=3, color="#1f77b4", label="u(tau)")
    ax_u.set_title("Входное воздействие u(tau)")
    ax_u.set_ylabel("u")
    ax_u.grid(True)
    ax_u.legend()

    ax_y.plot(tau, y_vals, marker="o", markersize=3, color="#d62728", label="y(tau)")
    ax_y.set_title("Выходная траектория y(tau) — Вариант 17")
    ax_y.set_xlabel("Шаг симуляции tau")
    ax_y.set_ylabel("y")
    ax_y.grid(True)
    ax_y.legend()

    fig.tight_layout()
    names = {"1": "Model1.7.png", "2": "Model2.1.png", "3": "Model3.5.png"}
    out_dir = csv_file.parent / "doc"
    out_dir.mkdir(parents=True, exist_ok=True)
    out_img = out_dir / names[mdl_id]
    fig.savefig(out_img, dpi=150, bbox_inches="tight")
    print(f"График сохранён: {out_img}")
    plt.show()


if __name__ == "__main__":
    main()
]=])
