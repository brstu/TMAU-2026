
import csv
import sys
from pathlib import Path

try:
    import matplotlib.pyplot as plt
except ImportError:
    print("Ошибка: библиотека matplotlib не установлена.")
    print("Установи её командой: python -m pip install matplotlib")
    sys.exit(1)


def find_csv():
    # Если путь передан при запуске, используем его.
    if len(sys.argv) > 1:
        path = Path(sys.argv[1]).expanduser()

        if path.is_file():
            return path.resolve()

        print(f"Указанный CSV-файл не найден: {path}")
        return None

    script_dir = Path(__file__).resolve().parent
    project_dir = script_dir.parent

    # Проверяем наиболее вероятные каталоги.
    candidates = [
        Path.cwd() / "results.csv",
        script_dir / "results.csv",
        project_dir / "results.csv",
        project_dir / "build" / "results.csv",
        project_dir / "out" / "build" / "results.csv",
        project_dir / "out" / "build" / "x64-Debug" / "results.csv",
        project_dir / "out" / "build" / "x64-Release" / "results.csv",
    ]

    checked = set()

    for candidate in candidates:
        candidate = candidate.resolve()

        if candidate not in checked:
            checked.add(candidate)

            if candidate.is_file():
                return candidate

    # Дополнительно ищем файл в подпапках проекта.
    for folder in (project_dir, Path.cwd()):
        try:
            for candidate in folder.glob("**/results.csv"):
                if candidate.is_file():
                    return candidate.resolve()
        except (PermissionError, OSError):
            continue

    return None


def main():
    csv_path = find_csv()

    if csv_path is None:
        print("Не удалось найти results.csv.")
        print("Проверь, что программа C++ уже выполнила расчёт.")
        print("Можно передать полный путь к CSV при запуске:")
        print('python src/plot.py "D:/путь/к/results.csv"')
        sys.exit(1)

    print(f"Найден CSV-файл: {csv_path}")

    try:
        with csv_path.open("r", newline="", encoding="utf-8-sig") as file:
            reader = csv.DictReader(file)

            required = {"tau", "u", "y"}
            if not reader.fieldnames or not required.issubset(reader.fieldnames):
                print("Ошибка: в CSV должны быть столбцы tau,u,y.")
                sys.exit(1)

            rows = list(reader)

        if not rows:
            print("CSV-файл пустой.")
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

    # Номер модели передаётся вторым аргументом:
    # python src/plot.py "путь/results.csv" 1
    # Если CSV не передан, номер модели указывается первым аргументом:
    # python src/plot.py 1
    model_number = None

    if len(sys.argv) > 1 and sys.argv[1] in {"1", "2", "3"}:
        model_number = sys.argv[1]
    elif len(sys.argv) > 2 and sys.argv[2] in {"1", "2", "3"}:
        model_number = sys.argv[2]

    if model_number is None:
     model_number = input(
        "Выбери модель (1 — Model 1.4, 2 — Model 2.9, 3 — Model 3.1): "
    ).strip()

    if model_number not in {"1", "2", "3"}:
        print("Ошибка: нужно ввести 1, 2 или 3.")
        plt.close(fig)
        sys.exit(1)

    image_names = {
        "1": "Model1.4.png",
        "2": "Model2.9.png",
        "3": "Model3.1.png",
    }

    project_dir = Path(__file__).resolve().parent.parent
    doc_dir = project_dir / "doc"
    doc_dir.mkdir(parents=True, exist_ok=True)

    image_path = doc_dir / image_names[model_number]
    fig.savefig(image_path, dpi=150, bbox_inches="tight")

    print(f"График сохранён: {image_path}")
    print("Открываю окно графика...")

    plt.show()


if __name__ == "__main__":
    main()
