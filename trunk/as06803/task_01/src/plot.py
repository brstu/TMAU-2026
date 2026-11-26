import argparse
import csv
import sys
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Построение графика моделирования для варианта 8"
    )
    parser.add_argument("csv_file", nargs="?", default="results.csv")
    parser.add_argument("--output", dest="output_file", default=None)
    args = parser.parse_args()
    csv_path = Path(args.csv_file).resolve()

    if not csv_path.is_file():
        print(f"Файл результатов не найден: {csv_path}")
        return 1

    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("Не установлена библиотека matplotlib.")
        print("Установите её командой: python -m pip install matplotlib")
        return 1

    try:
        with csv_path.open("r", encoding="utf-8-sig", newline="") as file:
            reader = csv.DictReader(file)
            required = {"model", "signal", "tau", "u_tau", "y_tau"}
            if not required.issubset(reader.fieldnames or []):
                print("В CSV отсутствуют необходимые столбцы.")
                return 1
            rows = list(reader)
    except (OSError, csv.Error) as error:
        print(f"Ошибка чтения CSV: {error}")
        return 1

    if not rows:
        print("В CSV-файле нет результатов моделирования.")
        return 1

    try:
        tau_values = [int(row["tau"]) for row in rows]
        y_values = [float(row["y_tau"]) for row in rows]
        model_name = rows[0]["model"]
        signal_name = rows[0]["signal"]
    except (KeyError, ValueError) as error:
        print(f"Некорректные данные в CSV: {error}")
        return 1

    plt.figure(figsize=(10, 6))
    plt.plot(tau_values, y_values, marker="o", markersize=3, linewidth=1.7)
    plt.title(f"{model_name}\n{signal_name}")
    plt.xlabel("Время τ")
    plt.ylabel("Выходная величина y(τ)")
    plt.grid(True)
    plt.tight_layout()

    image_path = Path(args.output_file).resolve() if args.output_file else csv_path.with_name("results_plot.png")
    try:
        plt.savefig(str(image_path), dpi=150, bbox_inches="tight")
        print(f"График сохранён: {image_path}")
    except (OSError, RuntimeError) as error:
        print(f"Не удалось сохранить график: {error}")
        plt.close()
        return 1

    try:
        plt.show()
    except Exception as error:
        print(f"Не удалось отобразить график: {error}")
    finally:
        plt.close()

    return 0


if __name__ == "__main__":
    sys.exit(main())
