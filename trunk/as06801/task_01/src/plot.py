"""Визуализация результатов: python plot.py [каталог_с_csv] [--show]"""
import csv, glob, os, sys
import matplotlib
if "--show" not in sys.argv:
    matplotlib.use("Agg")
import matplotlib.pyplot as plt

folder = next((a for a in sys.argv[1:] if not a.startswith("--")), "output")
files = sorted(glob.glob(os.path.join(folder, "model_*.csv")))
if not files:
    sys.exit(f"Нет csv-файлов в '{folder}'. Сначала запустите sim.exe")
os.makedirs(os.path.join(folder, "plots"), exist_ok=True)
for f in files:
    tau, u, y = [], [], []
    with open(f, newline="") as fh:
        for r in csv.DictReader(fh):
            tau.append(int(r["tau"])); u.append(float(r["u"])); y.append(float(r["y"]))
    name = os.path.splitext(os.path.basename(f))[0]
    fig, ax = plt.subplots(figsize=(8, 4))
    ax.step(tau, u, where="post", color="tab:orange", label="u(τ)")
    ax.plot(tau[:len(y)], y, "o-", ms=3, color="tab:blue", label="y(τ)")
    ax.set_title(name); ax.set_xlabel("τ"); ax.grid(alpha=.3); ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(folder, "plots", name + ".png"), dpi=120)
    if "--show" in sys.argv:
        plt.show()
    plt.close(fig)
print(f"Готово: {len(files)} графиков в {os.path.join(folder, 'plots')}")
