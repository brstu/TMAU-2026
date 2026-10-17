import os
import glob
import pandas as pd
import matplotlib.pyplot as plt

def main():
    csv_files = glob.glob("build/*.csv")
    
    if not csv_files:
        print("CSV файлы не найдены! Сначала запустите task_01.exe")
        return
        
    print(f"Обработка файлов: {csv_files}")
    models = ["linear", "nonlinear", "diff"]
    
    doc_dir = os.path.join("..", "doc")
    os.makedirs(doc_dir, exist_ok=True)
    
    for model in models:
        model_files = [f for f in csv_files if os.path.basename(f).startswith(model)]
        if not model_files:
            continue
            
        plt.figure(figsize=(10, 6))
        
        for file in model_files:
            filename = os.path.basename(file)
            signal_type = filename.replace(f"{model}_", "").replace(".csv", "")
            
            data = pd.read_csv(file)
            plt.plot(data["Time"], data["Output"], label=f"Output ({signal_type})", linewidth=2)
            
            if signal_type == "step":
                plt.plot(data["Time"], data["Input"], '--', color='gray', alpha=0.6, label="Input Step")
                
        plt.title(f"Simulation Results: {model.capitalize()} Model (Variant 14)")
        plt.xlabel("Time step (t)")
        plt.ylabel("Value")
        plt.grid(True, linestyle="--", alpha=0.7)
        plt.legend()

        output_img = os.path.join(doc_dir, f"{model}_plot.png")
        plt.savefig(output_img, dpi=300)
        print(f"График сохранен: {output_img}")
        
        plt.show()
        plt.close()

if __name__ == "__main__":
    main()