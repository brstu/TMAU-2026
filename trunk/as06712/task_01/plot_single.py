import sys
import re
import os
import subprocess

if len(sys.argv) < 2:
    print("Использование: python plot_single.py <file.csv>")
    sys.exit(1)

fname = sys.argv[1]

# Защита от command injection: разрешаем только безопасные имена
if not re.match(r"^[A-Za-z0-9_.\-]+\.csv$", fname):
    print(f"Недопустимое имя файла: {fname}")
    sys.exit(1)
if os.path.sep in fname or "/" in fname or "\\" in fname:
    print("Путь не разрешён, только имя файла без директорий")
    sys.exit(1)

try:
    import pandas as pd
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ImportError as e:
    print(f"Не найден модуль: {e}")
    print("Установите: pip install pandas matplotlib")
    sys.exit(1)

if not os.path.exists(fname):
    print(f"Файл не найден: {fname}")
    sys.exit(1)

df = pd.read_csv(fname)

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6), sharex=True)

ax1.plot(df["tau"], df["y"], "b-", linewidth=1.5, label="y(τ)")
ax1.set_ylabel("y")
ax1.set_title(fname)
ax1.grid(True, alpha=0.3)
ax1.legend()

ax2.plot(df["tau"], df["u"], "r--", linewidth=1.2, label="u(τ)")
ax2.set_xlabel("τ")
ax2.set_ylabel("u")
ax2.grid(True, alpha=0.3)
ax2.legend()

plt.tight_layout()

out_png = fname.replace(".csv", ".png")
plt.savefig(out_png, dpi=150)
plt.close()
print(f"График сохранён: {out_png}")

if sys.platform.startswith("win"):
    os.startfile(out_png)
elif sys.platform == "darwin":
    subprocess.Popen(["open", out_png])
else:
    subprocess.Popen(["xdg-open", out_png])
