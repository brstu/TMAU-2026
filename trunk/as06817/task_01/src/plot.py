import csv
import os
import matplotlib.pyplot as plt


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
    plt.title("Результат моделирования — " + model_name + ", вариант 17")
    plt.grid()
    plt.tight_layout()

    plt.savefig(image_file, dpi=150)
    plt.close()


base_dir = r"C:\Users\widuxd\Source\Repos\TMAU-2026\out\build\x64-Debug\trunk\as06817\task_01\src"
doc_dir = r"C:\Users\widuxd\Source\Repos\TMAU-2026\trunk\as06817\task_01\doc"

draw_graph(
    os.path.join(base_dir, "model17.csv"),
    os.path.join(doc_dir, "model17.png"),
    "модель 1.7"
)

draw_graph(
    os.path.join(base_dir, "model21.csv"),
    os.path.join(doc_dir, "model21.png"),
    "модель 2.1"
)

draw_graph(
    os.path.join(base_dir, "model35.csv"),
    os.path.join(doc_dir, "model35.png"),
    "модель 3.5"
)

print("Три графика сохранены в папке doc.")