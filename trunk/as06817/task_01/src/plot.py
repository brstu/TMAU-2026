import csv
import os
import sys
import matplotlib.pyplot as plt

# Исправление кодировки аргументов командной строки для Windows
if sys.platform == "win32":
    import io
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
    # Перекодируем аргументы из cp1251 (Windows console) в utf-8
    fixed_argv = []
    for arg in sys.argv:
        try:
            fixed_argv.append(arg.encode('cp1251').decode('utf-8'))
        except UnicodeDecodeError:
            fixed_argv.append(arg)
    sys.argv = fixed_argv


def read_results(filename):
    tau = []
    y = []

    with open(filename, "r", encoding="utf-8") as file:
        reader = csv.DictReader(file)

        for row in reader:
            tau.append(float(row["tau"]))
            y.append(float(row["y"]))

    return tau, y


def draw_graph(csv_file, image_file, model_name):
    tau, y = read_results(csv_file)

    plt.figure(figsize=(8, 5))
    plt.plot(tau, y, marker="o", markersize=3)

    plt.xlabel("τ")
    plt.ylabel("y")
    plt.title("Результат моделирования")
    plt.grid()
    plt.tight_layout()

    plt.savefig(image_file, dpi=150)
    plt.close()


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Ошибка: необходимо передать CSV, путь к изображению и модель.")
        sys.exit(1)

    csv_file = sys.argv[1]
    image_file = sys.argv[2]
    model_name = sys.argv[3]

    if not os.path.isfile(csv_file):
        print("CSV-файл не найден:", os.path.abspath(csv_file))
        sys.exit(1)

    try:
        draw_graph(csv_file, image_file, model_name)
        print("График успешно построен.")
    except Exception as error:
        print("Ошибка построения графика:", error)
        sys.exit(1)