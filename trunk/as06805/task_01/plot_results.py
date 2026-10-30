import os
import matplotlib.pyplot as plt

RESULTS_FILE = "results.txt"

possible_paths = [
    os.path.join("build", "src", "Debug", RESULTS_FILE),
    os.path.join("build", "src", "Release", RESULTS_FILE),
    os.path.join("build", RESULTS_FILE),
    RESULTS_FILE
]

file_path = None
for path in possible_paths:
    if os.path.exists(path):
        file_path = path
        break

tau = []
u = []
y = []
model_name = "Simulation"
signal_name = "result"

if file_path:
    print(f"[Инфо] Чтение данных из файла: {file_path}")
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            for line in f:
                if line.startswith("#"):
                    header = line.strip("# \n")
                    if "|" in header:
                        model_name, signal_name = header.split("|")
                    continue
                parts = line.replace(',', ' ').split()
                if len(parts) >= 3:
                    tau.append(float(parts[0]))
                    u.append(float(parts[1]))
                    y.append(float(parts[2]))

        if tau:
            plt.figure(figsize=(10, 6))
            plt.plot(tau, u, 'r--', label='u(tau) - Вход')
            plt.plot(tau, y, 'b-', linewidth=2, label='y(tau) - Выход')
            plt.title(f"График симуляции: {model_name}_{signal_name}")
            plt.xlabel("Шаг (tau)")
            plt.ylabel("Значение")
            plt.grid(True, linestyle='--', alpha=0.7)
            plt.legend()
            
            doc_dir = "doc"
            if not os.path.exists(doc_dir):
                os.makedirs(doc_dir)
            
            output_name = f"{model_name}_{signal_name}.png"
            output_img = os.path.join(doc_dir, output_name)
            plt.savefig(output_img)
            print(f"[Инфо] График обновлен в: {output_img}")

            plt.show()
        else:
            print("[Ошибка] Файл найден, но он пустой!")
    except Exception as e:
        print(f"[Ошибка] Ошибка при чтении файла: {e}")
else:
    print("[Ошибка] Файл результатов не найден!")