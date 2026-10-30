"""Строит графики u(tau) и y(tau) по CSV."""
import csv
import sys
from pathlib import Path

import matplotlib.pyplot as plt

CSV_FILE = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("simulation.csv")
if not CSV_FILE.is_file():
    raise SystemExit(
        f"Файл результатов не найден: {CSV_FILE}\n"
        "Сначала запустите программу task_01."
    )

rows = []
with CSV_FILE.open("r", encoding="utf-8-sig", newline="") as file:
    reader = csv.DictReader(file, delimiter=";")
    required = {"Model", "Input", "Time", "u_tau", "y_tau"}
    if not reader.fieldnames or not required.issubset(set(reader.fieldnames)):
        raise SystemExit("В CSV отсутствуют необходимые столбцы.")
    for row in reader:
        try:
            rows.append((
                row["Model"], row["Input"], float(row["Time"]),
                float(row["u_tau"]), float(row["y_tau"])
            ))
        except (TypeError, ValueError):
            continue

if not rows:
    raise SystemExit("В CSV нет числовых результатов для построения графика.")

model_name = rows[0][0]
input_name = rows[0][1]
times = [row[2] for row in rows]
input_values = [row[3] for row in rows]
output_values = [row[4] for row in rows]

plt.rcParams["font.family"] = "DejaVu Sans"
fig, axes = plt.subplots(2, 1, figsize=(10, 7), sharex=True)
fig.suptitle(f"{model_name}\nВходное воздействие: {input_name}", fontsize=13)

axes[0].plot(times, output_values, marker="o", markersize=3, linewidth=1.7,
             label="Выход y(t)")
axes[0].set_ylabel("y(t)")
axes[0].grid(True, linestyle="--", alpha=0.45)
axes[0].legend()

axes[1].plot(times, input_values, linewidth=1.5, label="Вход u(t)")
axes[1].set_xlabel("Время")
axes[1].set_ylabel("u(t)")
axes[1].grid(True, linestyle="--", alpha=0.45)
axes[1].legend()

fig.tight_layout(rect=(0, 0, 1, 0.93))
output_file = CSV_FILE.resolve().parent / "results_plot.png"
fig.savefig(output_file, dpi=160, bbox_inches="tight")
print(f"График сохранен: {output_file}")
if plt.get_backend().lower() != "agg":
    plt.show()
