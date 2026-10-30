
import sys
import csv
from pathlib import Path

import matplotlib.pyplot as plt


def main():
    if len(sys.argv) != 3:
        print("Ошибка: не указаны модель и входной сигнал.")
        return 1

    model_name = sys.argv[1]
    signal_name = sys.argv[2]

    data_path = Path("plot_data.txt")

    if not data_path.exists():
        print("Ошибка: файл plot_data.txt не найден.")
        return 1

    t_values = []
    y_values = []

    with data_path.open("r", encoding="utf-8", newline="") as file:
        reader = csv.DictReader(file)

        for row in reader:
            t_values.append(int(row["t"]))
            y_values.append(float(row["y"]))

    if not t_values:
        print("Ошибка: нет данных для построения графика.")
        return 1

    model_titles = {
        "model_1_10": "Linear Model 1.10",
        "model_2_2": "Nonlinear Model 2.2",
        "model_3_4": "Differential Model 3.4",
    }

    signal_titles = {
        "step": "Step Input",
        "impulse": "Impulse Input",
        "sinusoidal": "Sinusoidal Input",
    }

    model_title = model_titles.get(model_name, model_name)
    signal_title = signal_titles.get(signal_name, signal_name)

    plt.figure(figsize=(10, 6))
    plt.plot(t_values, y_values, marker="o", markersize=3)
    plt.title(f"{model_title} - {signal_title}")
    plt.xlabel("Time step t")
    plt.ylabel("Output y(t)")
    plt.grid(True)
    plt.tight_layout()

    output_path = Path(f"{model_name}_{signal_name}.png")
    plt.savefig(output_path, dpi=150)
    plt.show()
    plt.close()

    data_path.unlink(missing_ok=True)

    print(f"График сохранён: {output_path.resolve()}")

    return 0


if __name__ == "__main__":
    sys.exit(main())