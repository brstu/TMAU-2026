import csv
import os
import sys


def main():
    filename = "results.csv"
    if not os.path.exists(filename):
        print(f"Файл {filename} не найден. Сначала запустите программу моделирования.")
        return 1

    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("Библиотека matplotlib не установлена для выбранного Python.")
        print("Установите её командой: python -m pip install matplotlib")
        return 1

    steps = []
    inputs = []
    outputs = []
    model_name = "Неизвестная модель"
    signal_name = "Неизвестное воздействие"

    try:
        with open(filename, "r", newline="", encoding="utf-8-sig") as file:
            reader = csv.DictReader(file)
            required_columns = {"step", "input_u", "output_y"}
            if not reader.fieldnames or not required_columns.issubset(reader.fieldnames):
                print("Файл results.csv имеет неверный формат. Перезапустите моделирование.")
                return 1

            for row in reader:
                if not steps:
                    model_name = row.get("model_name", model_name) or model_name
                    signal_name = row.get("signal_name", signal_name) or signal_name
                steps.append(int(row["step"]))
                inputs.append(float(row["input_u"]))
                outputs.append(float(row["output_y"]))
    except (OSError, ValueError, csv.Error) as error:
        print(f"Не удалось прочитать results.csv: {error}")
        return 1

    if not steps:
        print("В results.csv нет данных для построения графика.")
        return 1

    plt.figure(figsize=(10, 6))
    plt.plot(steps, inputs, marker="o", markersize=3, linewidth=1.5, label="Входное воздействие u(t)")
    plt.plot(steps, outputs, marker=".", markersize=4, linewidth=1.8, label="Выход модели y(t)")
    plt.title(f"Модель: {model_name}\nТип входного воздействия: {signal_name}")
    plt.xlabel("Номер шага")
    plt.ylabel("Значение")
    plt.grid(True, alpha=0.35)
    plt.legend()
    plt.tight_layout()

    image_name = "results_plot.png"
    try:
        plt.savefig(image_name, dpi=150)
    except OSError as error:
        print(f"Не удалось сохранить график: {error}")
        return 1

    print(f"График сохранён в файл: {image_name}")
    try:
        plt.show()
    except Exception as error:
        print(f"Не удалось открыть окно графика; изображение сохранено: {error}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
