import csv
import matplotlib.pyplot as plt

tau = []
u = []
y = []

with open("results.csv", "r", encoding="utf-8") as file:
    reader = csv.DictReader(file)

    for row in reader:
        tau.append(int(row["tau"]))
        u.append(float(row["u"]))
        y.append(float(row["y"]))

plt.figure()
plt.plot(tau, y, label="y(t)")
plt.xlabel("τ")
plt.ylabel("y")
plt.title("TMAU-2026 — результат моделирования, вариант 1")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("result.png", dpi=150)
plt.show()

