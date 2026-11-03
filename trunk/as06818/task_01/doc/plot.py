import pandas as pd
import matplotlib.pyplot as plt
import os

models = [
    ('model1_8_results.csv', 'Model 1.8 (Generalized Autoregressive Linear)', 'model1_8.png'),
    ('model2_2_results.csv', 'Model 2.2 (Actuator Saturation Non-linearity)', 'model2_2.png'),
    ('model3_6_results.csv', 'Model 3.6 (Cubic Growth and Control)', 'model3_6.png')
]

script_dir = os.path.dirname(os.path.abspath(__file__))
build_dir = os.path.abspath(os.path.join(script_dir, '../src/build'))

for csv_name, title, output_img in models:
    csv_path = os.path.join(build_dir, csv_name)
    save_path = os.path.join(script_dir, output_img)

    if os.path.exists(csv_path):
        df = pd.read_csv(csv_path)
        
        plt.figure(figsize=(10, 6))
        plt.title(title, fontsize=14)

        # Рисуем реакции системы y(tau) для каждого входа на одном графике
        plt.plot(df['tau'], df['y_step'], 'b-', label='y (Step Input)', linewidth=2)
        plt.plot(df['tau'], df['y_impulse'], 'g--', label='y (Impulse Input)', linewidth=2)
        plt.plot(df['tau'], df['y_harmonic'], 'm-.', label='y (Harmonic Input)', linewidth=2)

        plt.xlabel('Tau', fontsize=12)
        plt.ylabel('Output y(tau)', fontsize=12)
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.legend(fontsize=11)
        plt.tight_layout()

        plt.savefig(save_path)
        print(f"График сохранен в doc/: {output_img}")
        plt.close()
    else:
        print(f"файл {csv_path} не найден. запустить ./task_01 в папке src/build")