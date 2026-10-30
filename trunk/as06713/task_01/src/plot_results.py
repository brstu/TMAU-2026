import csv
from pathlib import Path
import matplotlib.pyplot as plt

BASE_DIR = Path(__file__).resolve().parent
CSV_FILE = BASE_DIR / "simulation.csv"

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
                row["Model"],
                row["Input"],
                float(row["Time"]),
                float(row["u_tau"]),
                float(row["y_tau"])
            ))
        except (ValueError, TypeError):
            continue

if not rows:
    raise SystemExit("В CSV нет корректных данных для построения графиков.")

model_name = rows[0][0]
signal_name = rows[0][1]

time = [row[2] for row in rows]
input_signal = [row[3] for row in rows]
output_signal = [row[4] for row in rows]

fig, axes = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

fig.suptitle(
    f"{model_name}\nВход: {signal_name}",
    fontsize=14
)

axes[0].plot(
    time,
    input_signal,
    marker="o",
    linewidth=1.5,
    label="u(tau)"
)

axes[0].set_title("Входное воздействие")
axes[0].set_ylabel("u(tau)")
axes[0].grid(True)
axes[0].legend()

axes[1].plot(
    time,
    output_signal,
    marker="o",
    linewidth=1.5,
    label="y(tau)"
)

axes[1].set_title("Выходной сигнал системы")
axes[1].set_xlabel("Время (tau)")
axes[1].set_ylabel("y(tau)")
axes[1].grid(True)
axes[1].legend()

plt.tight_layout()

plt.show()
