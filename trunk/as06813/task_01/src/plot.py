import os
import re
import sys

import matplotlib.pyplot as plt

TITLES = {
    "1.3": "Линейная модель 1.3",
    "2.7": "Нелинейная модель 2.7",
    "3.1": "Дифференциальная модель 3.1",
}
SIGNALS = {"step": "ступенчатое", "impulse": "импульсное", "harmonic": "гармоническое"}


def load(path):
    k, t, u, y = [], [], [], []
    with open(path, encoding="utf-8") as f:
        next(f)  # пропускаем заголовок
        for line in f:
            a, b, c, d = line.strip().split(";")
            k.append(int(a))
            t.append(float(b))
            u.append(float(c))
            y.append(float(d))
    return k, t, u, y


def show_table(k, t, u, y):
    print(f"\n{'k':>5}  {'t':>7}  {'u(k)':>9}  {'y(k)':>11}")
    print("-" * 38)
    for row in zip(k, t, u, y):
        yk = 0.0 if abs(row[3]) < 5e-6 else row[3]
        print(f"{row[0]:>5}  {row[1]:>7.2f}  {row[2]:>9.4f}  {yk:>11.5f}")


def results_file_from_args():
    """Проверяет аргумент и возвращает (модель, воздействие) или None.

    Путь из командной строки напрямую не используется: имя файла разбирается
    по шаблону, а модель и воздействие берутся из списка допустимых значений.
    """
    if len(sys.argv) < 2:
        return None
    match = re.fullmatch(r"result_(\d\.\d)_([a-z]+)\.csv", os.path.basename(sys.argv[1]))
    if match is None:
        return None
    model = next((m for m in TITLES if m == match.group(1)), None)
    signal = next((s for s in SIGNALS if s == match.group(2)), None)
    if model is None or signal is None:
        return None
    return model, signal


parsed = results_file_from_args()
if parsed is None:
    print("Запуск: python plot.py result_<модель>_<воздействие>.csv")
    sys.exit(1)

model, signal = parsed
name = f"result_{model}_{signal}"
path = os.path.join(os.getcwd(), name + ".csv")
if not os.path.exists(path):
    print("Нет файла с результатами:", name + ".csv")
    sys.exit(1)

title = f"{TITLES[model]}, {SIGNALS[signal]} воздействие"

k, t, u, y = load(path)
print(title)
show_table(k, t, u, y)

# для модели 3.1 по оси X время, для остальных - номер шага
x = t if model == "3.1" else k
x_label = "t" if model == "3.1" else "k"

# общий график: вход и выход на одних осях
fig = plt.figure(figsize=(9, 5))
plt.step(x, u, where="post", color="tab:orange", linewidth=2, linestyle="--", label="вход u")
plt.plot(x, y, ".-", color="tab:blue", linewidth=2, label="выход y")
plt.title(title)
plt.xlabel(x_label)
plt.grid(True)
plt.legend()
fig.tight_layout()

images = os.path.join("..", "doc", "images")
os.makedirs(images, exist_ok=True)
picture = os.path.join(images, name.replace(".", "_") + ".png")
fig.savefig(picture, dpi=110)
print("\nКартинка:", os.path.abspath(picture))

plt.show()
