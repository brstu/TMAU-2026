import csv
from pathlib import Path
import matplotlib.pyplot as plt

csv_path = Path(__file__).with_name("results.csv")
if not csv_path.exists():
    raise SystemExit("Файл results.csv не найден. Сначала запустите программу.")

with csv_path.open(newline="", encoding="utf-8") as f:
    rows = list(csv.DictReader(f))

if not rows:
    raise SystemExit("В results.csv нет данных.")

tau = [int(row["tau"]) for row in rows]
u = [float(row["u"]) for row in rows]
y = [float(row["y"]) for row in rows]
model = rows[0]["model"]
signal = rows[0]["signal"]

fig, ax = plt.subplots(figsize=(10, 5.5))
ax.plot(tau, y, marker="o", linewidth=2, label="Выход y")
ax.plot(tau, u, marker=".", linestyle="--", alpha=0.75, label="Вход u")
ax.set_title(f"{model} — {signal} input")
ax.set_xlabel("Дискретное время τ")
ax.set_ylabel("Значение")
ax.grid(True)
ax.legend()
fig.tight_layout()

image_path = csv_path.with_name("plot_results.png")
fig.savefig(image_path, dpi=150)
print(f"График сохранён: {image_path}")
plt.show()  # Окно графика открывается автоматически и остаётся на экране.
