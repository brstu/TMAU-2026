import csv
import os
import sys


def main():
    filename = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results.csv")
    if not os.path.exists(filename):
        print("Файл results.csv не найден. Сначала запустите TiMAY.exe и выполните моделирование.")
        return 1

    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("Для построения графика установите Matplotlib командой: py -m pip install matplotlib")
        return 1

    tau_values = []
    input_values = []
    output_values = []

    try:
        with open(filename, "r", newline="", encoding="utf-8-sig") as file:
            reader = csv.DictReader(file)
            for row in reader:
                tau_values.append(int(row["tau"]))
                input_values.append(float(row["u_tau"]))
                output_values.append(float(row["y_tau"]))
    except (OSError, ValueError, KeyError) as error:
        print(f"Не удалось прочитать результаты: {error}")
        return 1

    if not tau_values:
        print("В results.csv нет данных для построения графика.")
        return 1

    figure, axis = plt.subplots(figsize=(10, 6))
    axis.plot(tau_values, output_values, label="y(tau)", linewidth=2)
    axis.plot(tau_values, input_values, label="u(tau)", linestyle="--", alpha=0.8)
    axis.set_title("Результаты моделирования — вариант 12")
    axis.set_xlabel("Шаг tau")
    axis.set_ylabel("Значение")
    axis.grid(True, alpha=0.3)
    axis.legend()
    figure.tight_layout()

    image_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results_plot.png")
    figure.savefig(image_file, dpi=150)
    print(f"График сохранен в файл: {image_file}")
    plt.show()
    return 0


if __name__ == "__main__":
    sys.exit(main())
