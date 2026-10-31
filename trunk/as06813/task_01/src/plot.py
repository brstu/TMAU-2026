import os
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


path = sys.argv[1] if len(sys.argv) > 1 else ""
if not os.path.exists(path):
    print("Нет файла с результатами:", path)
    sys.exit(1)

name = os.path.splitext(os.path.basename(path))[0]   # result_1.3_step
_, model, signal = name.split("_")
title = f"{TITLES.get(model, model)}, {SIGNALS.get(signal, signal)} воздействие"

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
