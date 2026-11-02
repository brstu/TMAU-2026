import csv
import os
import re
import sys

import matplotlib.pyplot as plt

MODEL_NAMES = {
    "1": "Линейная модель 1.4",
    "2": "Нелинейная модель 2.8",
    "3": "Дифференциальная модель 3.2",
}
SIGNAL_NAMES = {"1": "ступенчатое", "2": "импульсное", "3": "гармоническое"}

# Папка для картинок отчёта
IMAGES_DIR = os.path.join("..", "doc", "images")


def read_results(path):
    tau, u, y = [], [], []
    with open(path, encoding="utf-8") as f:
        for row in csv.DictReader(f):
            tau.append(int(row["tau"]))
            u.append(float(row["u"]))
            y.append(float(row["y"]))
    return tau, u, y


def print_table(title, tau, u, y):
    print()
    print(title)
    print(f"{'шаг':>6} | {'u(τ)':>10} | {'y(τ)':>12}")
    print("-" * 34)
    for t, ut, yt in zip(tau, u, y):
        print(f"{t:>6} | {ut:>10.4f} | {yt:>12.5f}")


def results_file_from_args():
    """Возвращает (номер модели, номер воздействия) из аргумента или None.

    Путь из командной строки напрямую не открывается: имя файла проверяется
    по шаблону, а номера берутся из списка допустимых значений.
    """
    if len(sys.argv) < 2:
        return None
    match = re.fullmatch(r"model_(\d)_signal_(\d)\.csv", os.path.basename(sys.argv[1]))
    if match is None:
        return None
    m = next((key for key in MODEL_NAMES if key == match.group(1)), None)
    s = next((key for key in SIGNAL_NAMES if key == match.group(2)), None)
    if m is None or s is None:
        return None
    return m, s


def main():
    parsed = results_file_from_args()
    if parsed is None:
        print("Запуск: python plot.py model_X_signal_Y.csv")
        return

    m, s = parsed
    base = f"model_{m}_signal_{s}"
    path = os.path.join(os.getcwd(), base + ".csv")
    if not os.path.exists(path):
        print("Файл не найден:", base + ".csv")
        return

    title = f"{MODEL_NAMES[m]}, {SIGNAL_NAMES[s]} воздействие"

    tau, u, y = read_results(path)
    print_table(title, tau, u, y)

    plt.figure(figsize=(9, 5))
    plt.step(tau, u, where="post", linestyle="--", label="вход u(τ)")
    plt.plot(tau, y, marker="o", markersize=3, label="выход y(τ)")
    plt.title(title)
    plt.xlabel("шаг τ")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    os.makedirs(IMAGES_DIR, exist_ok=True)
    image_path = os.path.join(IMAGES_DIR, base + ".png")
    plt.savefig(image_path, dpi=120)
    print("\nГрафик сохранён:", os.path.abspath(image_path))

    plt.show()


if __name__ == "__main__":
    main()
