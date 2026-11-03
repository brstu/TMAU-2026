import os
import subprocess
import sys

REQUIRED = ("matplotlib", "pandas")

try:
    import matplotlib.pyplot as plt
    import pandas as pd
except ImportError:
    print("Installing missing packages...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", *REQUIRED])
    import matplotlib.pyplot as plt
    import pandas as pd

# Каталог с результатами: "../doc" при запуске из src/, иначе "doc"
results_dir = "../doc" if os.path.basename(os.getcwd()) == "src" else "doc"

# Отбираем все CSV-файлы в каталоге результатов
available_csv = [name for name in os.listdir(results_dir) if name.endswith(".csv")]

if not available_csv:
    print("[ERROR] No CSV files found in doc/ directory. Run C++ simulation first.")
    sys.exit(1)

# Берём самый свежий файл — его только что записала C++-программа
newest_csv = max(
    available_csv,
    key=lambda name: os.path.getmtime(os.path.join(results_dir, name)),
)

csv_path = os.path.join(results_dir, newest_csv)
print(f"\nGenerating graph automatically for selected mode: {newest_csv}")

frame = pd.read_csv(csv_path)

# --- Построение графика ---
fig, ax = plt.subplots(figsize=(8, 5))

ax.plot(
    frame["Step"], frame["y"],
    label="Output y(t)",
    color="blue", marker="o", linewidth=2,
)
ax.plot(
    frame["Step"], frame["u"],
    label="Input u(t)",
    color="orange", linestyle="--", linewidth=1.5,
)

ax.set_title(
    f"Transition Process: {newest_csv.replace('.csv', '')}",
    fontsize=12, fontweight="bold",
)
ax.set_xlabel("Step (t)", fontsize=10)
ax.set_ylabel("Value", fontsize=10)
ax.grid(True, linestyle=":", alpha=0.6)
ax.legend(loc="best")

# 1. Автоматически сохраняем картинку рядом с CSV — для отчёта
png_filename = newest_csv.replace(".csv", ".png")
png_path = os.path.join(results_dir, png_filename)
fig.savefig(png_path, dpi=150, bbox_inches="tight")
print(f"[SUCCESS] Graph saved automatically as: {png_path}")

# 2. Принудительно показываем окно с графиком
print("Opening graph window...")
plt.show()