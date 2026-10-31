import os
import subprocess
import sys

try:
    import matplotlib.pyplot as plt
    import pandas as pd
except ImportError:
    print("Installing missing packages...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "matplotlib", "pandas"])
    import matplotlib.pyplot as plt
    import pandas as pd

# Папка, где лежат результаты
doc_dir = "../doc" if os.path.basename(os.getcwd()) == "src" else "doc"

# Ищем самый свежий csv файл в папке doc
csv_files = [f for f in os.listdir(doc_dir) if f.endswith('.csv')]

if not csv_files:
    print("[ERROR] No CSV files found in doc/ directory. Run C++ simulation first.")
    sys.exit(1)

# Сортируем по времени изменения, чтобы взять тот файл, который только что создала C++ программа
csv_files.sort(key=lambda x: os.path.getmtime(os.path.join(doc_dir, x)), reverse=True)
latest_csv = csv_files[0]

full_path = os.path.join(doc_dir, latest_csv)
print(f"\nGenerating graph automatically for selected mode: {latest_csv}")

data = pd.read_csv(full_path)

plt.figure(figsize=(8, 5))
plt.plot(data['Step'], data['y'], label='Output y(t)', color='blue', marker='o', linewidth=2)
plt.plot(data['Step'], data['u'], label='Input u(t)', color='orange', linestyle='--', linewidth=1.5)

plt.title(f"Transition Process: {latest_csv.replace('.csv', '')}", fontsize=12, fontweight='bold')
plt.xlabel('Step (t)', fontsize=10)
plt.ylabel('Value', fontsize=10)
plt.grid(True, linestyle=':', alpha=0.6)
plt.legend(loc='best')

# 1. Сначала АВТОМАТИЧЕСКИ СОХРАНЯЕМ картинку в папку doc/ для отчета GitHub
png_name = latest_csv.replace(".csv", ".png")
output_path = os.path.join(doc_dir, png_name)
plt.savefig(output_path, dpi=150, bbox_inches='tight')
print(f"[SUCCESS] Graph saved automatically as: {output_path}")

# 2. Теперь ПРИНУДИТЕЛЬНО ВЫВОДИМ ГРАФИК НА ЭКРАН для пользователя
print("Opening graph window...")
plt.show()