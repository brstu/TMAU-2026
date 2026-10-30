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
show_window = "--no-show" not in sys.argv

# Файлы последнего запуска (их перечисляет C++-программа в last_run.txt)
list_path = os.path.join(results_dir, "last_run.txt")
if os.path.exists(list_path):
    with open(list_path, encoding="utf-8") as f:
        csv_files = [os.path.basename(line.strip()) for line in f if line.strip()]
else:
    csv_files = sorted(n for n in os.listdir(results_dir) if n.endswith(".csv"))

if not csv_files:
    print("[ERROR] No CSV files found in doc/. Run the C++ simulation first.")
    sys.exit(1)

MODEL_TITLES = {
    "Linear_1_7": "Linear model 1.7",
    "Nonlinear_2_9": "Nonlinear model 2.9",
    "DiffEquation_3_1": "Differential equation 3.1",
}
MODEL_ORDER = list(MODEL_TITLES)
SIGNAL_ORDER = ["Step", "Impulse", "Harmonic"]


def split_name(csv_name):
    stem = csv_name[:-4]
    model, signal = stem.rsplit("_", 1)
    return model, signal


def draw(ax, frame, title):
    ax.plot(frame["Step"], frame["y"], label="Output y(t)",
            color="tab:blue", marker="o", markersize=3, linewidth=2)
    ax.plot(frame["Step"], frame["u"], label="Input u(t)",
            color="tab:orange", linestyle="--", linewidth=1.5)
    ax.set_title(title, fontsize=10, fontweight="bold")
    ax.set_xlabel("Step (t)")
    ax.set_ylabel("Value")
    ax.grid(True, linestyle=":", alpha=0.6)
    ax.legend(loc="best", fontsize=8)


data = {}
for name in csv_files:
    path = os.path.join(results_dir, name)
    if not os.path.exists(path):
        continue
    data[name] = pd.read_csv(path)

    # Отдельный график для каждого CSV — для отчёта
    model, signal = split_name(name)
    fig, ax = plt.subplots(figsize=(8, 5))
    draw(ax, data[name], f"{MODEL_TITLES.get(model, model)} — {signal} input")
    png_path = os.path.join(results_dir, name.replace(".csv", ".png"))
    fig.savefig(png_path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"[SUCCESS] Graph saved: {png_path}")

# Сводный график: строки — модели, столбцы — входные сигналы
models = [m for m in MODEL_ORDER if any(split_name(n)[0] == m for n in data)]
signals = [s for s in SIGNAL_ORDER if any(split_name(n)[1] == s for n in data)]

fig, axes = plt.subplots(len(models), len(signals),
                         figsize=(5 * len(signals), 3.6 * len(models)),
                         squeeze=False)
for i, model in enumerate(models):
    for j, signal in enumerate(signals):
        ax = axes[i][j]
        name = f"{model}_{signal}.csv"
        if name in data:
            draw(ax, data[name], f"{MODEL_TITLES[model]} — {signal}")
        else:
            ax.axis("off")

fig.suptitle("Variant 7: transition processes", fontsize=14, fontweight="bold")
fig.tight_layout()
summary_path = os.path.join(results_dir, "simulation_results.png")
fig.savefig(summary_path, dpi=120, bbox_inches="tight")
print(f"[SUCCESS] Summary graph saved: {summary_path}")

if show_window:
    print("Opening graph window...")
    plt.show()
