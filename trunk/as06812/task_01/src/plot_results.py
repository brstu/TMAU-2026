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


def find_latest_csv(directory):
    csv_files = [f for f in os.listdir(directory) if f.endswith('.csv')]
    if not csv_files:
        print(f"[Error] No CSV files in '{directory}'. Run the C++ simulation first")
        sys.exit(1)
    
    # Сортировка по времени изменения (новые сверху)
    csv_files.sort(key=lambda f: os.path.getmtime(os.path.join(directory, f)), reverse=True)
    return csv_files[0]


def plot_transition(csv_path, output_dir):
    data = pd.read_csv(csv_path)
    filename = os.path.basename(csv_path).replace(".csv", "")
    
    print(f"\nGenerating graph for: {filename}")
    
    # Настройка графика
    plt.figure(figsize=(9, 5))
    plt.plot(data['Step'], data['y'], label='Output y(t)', color='#1f77b4', marker='o', markersize=4, linewidth=2)
    plt.plot(data['Step'], data['u'], label='Input u(t)', color='#ff7f0e', linestyle='--', linewidth=1.5)
    
    plt.title(f"{filename}", fontsize=13, fontweight='bold')
    plt.xlabel("Step (t)", fontsize=11)
    plt.ylabel("Value", fontsize=11)
    plt.grid(True, linestyle=':', alpha=0.5)
    plt.legend(loc='best', fontsize=10)
    plt.tight_layout()
    
    # Сохранение
    output_path = os.path.join(output_dir, f"{filename}.png")
    plt.savefig(output_path, dpi=150, bbox_inches='tight')
    print(f"Saved: {output_path}")
    
    return output_path


def main():
    doc_dir = "../doc" if os.path.basename(os.getcwd()) == "src" else "doc"
    
    if not os.path.isdir(doc_dir):
        print(f"[Error] Directory '{doc_dir}' not found")
        sys.exit(1)
    
    latest_csv = find_latest_csv(doc_dir)
    plot_transition(os.path.join(doc_dir, latest_csv), doc_dir)
    
    print("Opening graph window...")
    plt.show()


if __name__ == "__main__":
    main()