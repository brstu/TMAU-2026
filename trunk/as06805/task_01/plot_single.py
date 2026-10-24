import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Использование: python plot_single.py <path_to_csv> [output_png_name]")
    sys.exit(1)

csv_path = sys.argv[1]
output_name = sys.argv[2] if len(sys.argv) > 2 else os.path.basename(csv_path).replace(".csv", ".png")

try:
    df = pd.read_csv(csv_path)
    plt.figure(figsize=(8, 5))
    plt.plot(df['tau'], df['u'], label='u(tau) - Вход', linestyle='--')
    plt.plot(df['tau'], df['y'], label='y(tau) - Выход', linewidth=2)
    plt.title(f"График симуляции: {output_name.replace('.png', '')}")
    plt.xlabel("Шаг (tau)")
    plt.ylabel("Значение")
    plt.grid(True)
    plt.legend()
    
    # Определяем корень проекта (на уровень выше, если мы внутри build/)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Если мы находимся в build, поднимаемся на уровень выше к task_01
    if os.path.basename(os.getcwd()).lower() == 'build':
        project_root = os.path.abspath(os.path.join(os.getcwd(), ".."))
    else:
        project_root = script_dir

    doc_dir = os.path.join(project_root, "doc")
    if not os.path.exists(doc_dir):
        os.makedirs(doc_dir)

    img_path = os.path.join(doc_dir, output_name)
    
    plt.savefig(img_path)
    print(f"График успешно сохранен в: {img_path}")
except Exception as e:
    print(f"Ошибка при генерации графика: {e}")