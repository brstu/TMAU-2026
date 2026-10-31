import os
import glob
import pandas as pd
import matplotlib.pyplot as plt

def main():
    csv_files = glob.glob("*.csv") + glob.glob("build/*.csv")
    
    if not csv_files:
        print("CSV файлы не найдены! Сначала запустите симуляцию C++")
        return

    doc_dir = os.path.join("..", "doc")
    os.makedirs(doc_dir, exist_ok=True)

    model_names = {
        "linear": "Linear Model",
        "nonlinear": "Nonlinear Model",
        "diff": "Differential Model"
    }
    
    signal_names = {
        "step": "Step Signal",
        "impulse": "Impulse Signal",
        "harmonic": "Harmonic Signal"
    }

    for file_path in csv_files:
        filename = os.path.basename(file_path)
        name_no_ext = os.path.splitext(filename)[0]

        parts = name_no_ext.split("_")
        if len(parts) != 2:
            continue
            
        model, signal_type = parts[0], parts[1]
        
        data = pd.read_csv(file_path)
        
        plt.figure(figsize=(10, 6))

        plt.plot(
            data["Time"], 
            data["Input"], 
            '--', 
            color='gray', 
            alpha=0.7, 
            label=f"Input ({signal_names.get(signal_type, signal_type)})", 
            linewidth=1.5
        )
        
        plt.plot(
            data["Time"], 
            data["Output"], 
            color='tab:blue', 
            label=f"Output ({model_names.get(model, model)})", 
            linewidth=2
        )
        
        title_model = model_names.get(model, model.capitalize())
        title_signal = signal_names.get(signal_type, signal_type.capitalize())
        
        plt.title(f"Simulation: {title_model} - {title_signal}")
        plt.xlabel("Time step (t)")
        plt.ylabel("Value")
        plt.grid(True, linestyle="--", alpha=0.7)
        plt.legend()

        output_img = os.path.join(doc_dir, f"{model}_{signal_type}.png")
        plt.savefig(output_img, dpi=300)
        print(f"График сохранен: {output_img}")
        
        plt.show()

if __name__ == "__main__":
    main()