add_library(as06721_task02_models STATIC main.cpp StabilityAnalyzer.cpp)
target_compile_definitions(as06721_task02_models PRIVATE TASK02_SIMULATION_LIBRARY)
target_compile_features(as06721_task02_models PUBLIC cxx_std_17)
target_include_directories(as06721_task02_models PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(as06721_task02 main.cpp)
set_target_properties(as06721_task02 PROPERTIES OUTPUT_NAME task02)
target_link_libraries(as06721_task02 PRIVATE as06721_task02_models)
target_compile_definitions(as06721_task02 PRIVATE PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/.." TASK02_PLOT_SCRIPT="${CMAKE_CURRENT_BINARY_DIR}/plot.py")

foreach(target as06721_task02_models as06721_task02)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

if(TASK02_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "TASK02_COVERAGE requires GCC.")
    endif()
    target_compile_options(as06721_task02_models PRIVATE --coverage -O0 -g)
    target_link_options(as06721_task02_models INTERFACE --coverage)
endif()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/plot.py" [=[
import csv
import sys
from pathlib import Path

try:
    import matplotlib.pyplot as plt
except ImportError:
    print("Ошибка: библиотека matplotlib не установлена.")
    print("Установите её командой: py -m pip install matplotlib")
    sys.exit(1)


def find_csv(argument=None):
    if argument:
        path = Path(argument).expanduser()
        if path.is_file():
            return path.resolve()
        print(f"Указанный CSV-файл не найден: {path}")
        return None

    script_dir = Path(__file__).resolve().parent
    project_dir = script_dir.parent
    candidates = [
        Path.cwd() / "results.csv",
        project_dir / "results.csv",
        script_dir / "results.csv",
        project_dir / "build" / "results.csv",
    ]

    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()

    return None


def parse_arguments():
    csv_argument = None
    model_number = None

    if len(sys.argv) >= 2:
        if sys.argv[1] in {"1", "2", "3"}:
            model_number = sys.argv[1]
        else:
            csv_argument = sys.argv[1]

    if len(sys.argv) >= 3 and sys.argv[2] in {"1", "2", "3"}:
        model_number = sys.argv[2]

    return csv_argument, model_number


def main():
    csv_argument, model_number = parse_arguments()
    csv_path = find_csv(csv_argument)

    if csv_path is None:
        print("Не удалось найти results.csv.")
        print('Пример запуска: py src/plot.py "results.csv" 1')
        sys.exit(1)

    if model_number not in {"1", "2", "3"}:
        print("Ошибка: номер модели должен быть 1, 2 или 3.")
        sys.exit(1)

    try:
        with csv_path.open("r", newline="", encoding="utf-8-sig") as file:
            reader = csv.DictReader(file)
            required = {"tau", "u", "y"}

            if not reader.fieldnames or not required.issubset(reader.fieldnames):
                print("Ошибка: в CSV должны быть столбцы tau,u,y.")
                sys.exit(1)

            rows = list(reader)

        if not rows:
            print("Ошибка: CSV-файл пустой.")
            sys.exit(1)

        tau = [int(row["tau"]) for row in rows]
        u = [float(row["u"]) for row in rows]
        y = [float(row["y"]) for row in rows]
    except (OSError, ValueError, csv.Error) as error:
        print(f"Ошибка чтения CSV: {error}")
        sys.exit(1)

    fig, axes = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

    axes[0].plot(tau, u, marker="o", markersize=3, label="u(tau)")
    axes[0].set_title("Входное воздействие")
    axes[0].set_ylabel("u(tau)")
    axes[0].grid(True)
    axes[0].legend()

    axes[1].plot(tau, y, marker="o", markersize=3, label="y(tau)")
    axes[1].set_title("Результат моделирования")
    axes[1].set_xlabel("Шаг симуляции tau")
    axes[1].set_ylabel("y(tau)")
    axes[1].grid(True)
    axes[1].legend()

    fig.tight_layout()

    image_names = {
        "1": "Model1.2.png",
        "2": "Model2.7.png",
        "3": "Model3.5.png",
    }

    project_dir = csv_path.parent
    doc_dir = project_dir / "doc"
    doc_dir.mkdir(parents=True, exist_ok=True)
    image_path = doc_dir / image_names[model_number]

    fig.savefig(image_path, dpi=150, bbox_inches="tight")
    print(f"График сохранён: {image_path}")
    print("Открываю окно графика...")
    plt.show()


if __name__ == "__main__":
    main()
]=])
