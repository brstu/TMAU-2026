import csv
import sys
from pathlib import Path

import matplotlib.pyplot as plt


models = {
    1: "Модель 1.2 — линейная модель с задержкой управления",
    2: "Модель 2.4 — нелинейная модель (косинус и логарифм)",
    3: "Модель 3.6 — дифференциальная модель (кубический рост)"
}

signals = {
    1: "Ступенчатое воздействие",
    2: "Импульсное воздействие",
    3: "Гармоническое воздействие"
}

model_choice = int(sys.argv[1]) if len(sys.argv) > 1 else 1
signal_choice = int(sys.argv[2]) if len(sys.argv) > 2 else 1

model_name = models.get(model_choice, "Модель")
signal_name = signals.get(signal_choice, "Входное воздействие")

script_directory = Path(__file__).resolve().parent

project_directory = script_directory

for folder in script_directory.parents:
    if (folder / "build.bat").is_file():
        project_directory = folder
        break

csv_candidates = [
    Path.cwd() / "results.csv",
    script_directory / "results.csv",
    project_directory / "results.csv"
]

csv_path = next(
    (path for path in csv_candidates if path.is_file()),
    None
)

if csv_path is None:
    print("Файл results.csv не найден.")
    sys.exit(1)

with open(csv_path, "r", encoding="utf-8-sig", newline="") as file:
    rows = list(csv.DictReader(file))

if not rows:
    print("Нет данных для построения графиков.")
    sys.exit(1)

tau = [int(row["Шаг"]) for row in rows]
u = [float(row["Входное воздействие"]) for row in rows]
y = [float(row["Следующее значение y"]) for row in rows]

plt.rcParams["font.family"] = ["Arial", "DejaVu Sans"]
plt.rcParams["axes.unicode_minus"] = False

fig, axes = plt.subplots(2, 1, figsize=(12, 8), sharex=True)

fig.suptitle(
    f"Моделирование объекта управления\n{model_name}",
    fontsize=14,
    fontweight="bold"
)

axes[0].plot(
    tau,
    u,
    marker="o",
    markersize=3,
    label=signal_name
)
axes[0].set_title(f"Входное воздействие: {signal_name}")
axes[0].set_ylabel("Вход u(τ)")
axes[0].grid(True)
axes[0].legend()

axes[1].plot(
    tau,
    y,
    marker="o",
    markersize=3,
    label="Выходная реакция y(τ+1)"
)
axes[1].set_title(f"Выходная реакция модели: {model_name}")
axes[1].set_xlabel("Шаг моделирования τ")
axes[1].set_ylabel("Выход y(τ+1)")
axes[1].grid(True)
axes[1].legend()

fig.tight_layout(rect=[0, 0, 1, 0.92])

image_path = project_directory / "results.png"

fig.savefig(image_path, dpi=150, bbox_inches="tight")

print(f"\nГрафик сохранён:\n{image_path.resolve()}")

plt.show()