import os
import sys

# --- Проверка зависимостей ---
try:
    import matplotlib
except ImportError:
    print("[error] matplotlib is not installed. Run: pacman -S mingw-w64-ucrt-x86_64-python-matplotlib")
    input("Press Enter to close...")
    sys.exit(1)

matplotlib.use("TkAgg")
import matplotlib.pyplot as plt

# --- Пути ---
DOC_DIR = "doc"
CSV_PATH = os.path.join(DOC_DIR, "results.csv")
PNG_PATH = os.path.join(DOC_DIR, "plot.png")

if not os.path.exists(CSV_PATH):
    print(f"[error] file not found: {CSV_PATH}")
    print("Run the C++ program first so it writes doc/results.csv")
    input("Press Enter to close...")
    sys.exit(1)

# --- Чтение данных ---
time: list[int] = []
y: list[float] = []
model: str = ""
signal: str = ""

with open(CSV_PATH, 'r', encoding="UTF-8") as file:
    lines = file.readlines()
    
    # Первая строка: модель и сигнал
    model, signal = lines[0].strip().split(';')
    
    # Остальные строки: данные
    lines.pop(0)
    for line in lines:
        if not line.strip(): continue
        raw_data = line.strip().split(';')
        time.append(int(raw_data[0]))
        # Индекс 2 - это y (выход), так как 1 - это u (вход)
        y.append(float(raw_data[2]))

# --- Построение графика ---
fig, ax = plt.subplots(figsize=(8, 5))

ax.plot(time, y, color="blue", marker="o", linewidth=1.5) 
ax.set_xlabel("Шаги")
ax.set_ylabel("Значений функции")

plt.title("Тип модели: " + model + ".\nТип сигнала: " + signal)

# Шаг по оси X (каждые 5 шагов)
plt.xticks(range(0, len(time) + 1, 5))

plt.grid(True, linestyle=":", alpha=0.6)

# Сохранение и показ
os.makedirs(DOC_DIR, exist_ok=True)
fig.savefig(PNG_PATH, dpi=300)
print(f"[ok] plot saved to {PNG_PATH}")

plt.show()