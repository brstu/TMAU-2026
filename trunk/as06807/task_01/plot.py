import csv
import sys
import matplotlib.pyplot as plt

filename = sys.argv[1] if len(sys.argv) > 1 else "results.csv"

steps, u_vals, y_vals = [], [], []
model = input_type = ""

with open(filename, "r", newline="", encoding="utf-8") as file:
    for row in csv.DictReader(file):
        model = row["Model"]
        input_type = row["InputType"]
        steps.append(int(row["Step"]))
        u_vals.append(float(row["U"]))
        y_vals.append(float(row["Y"]))

if not steps:
    print("В CSV-файле нет результатов.")
    raise SystemExit

# ---- Таблица в консоли ----
print(f"\nМодель: {model}, воздействие: {input_type}\n")
print(f"{'Шаг':>5} | {'u':>12} | {'y':>12}")
print("-" * 36)
for s, u, y in zip(steps, u_vals, y_vals):
    print(f"{s:>5} | {u:>12.6f} | {y:>12.6f}")

# ---- График ----
fig, (ax_u, ax_y) = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

ax_u.plot(steps, u_vals, marker="o", color="tab:orange")
ax_u.set_title(f"Воздействие: {input_type}")
ax_u.set_ylabel("u")
ax_u.grid(True)

ax_y.plot(steps, y_vals, marker="o", color="tab:blue")
ax_y.set_title(f"Отклик модели {model}")
ax_y.set_xlabel("Шаг времени")
ax_y.set_ylabel("y")
ax_y.grid(True)

plt.tight_layout()
plt.show()