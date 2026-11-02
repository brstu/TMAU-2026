import pandas as pd
import matplotlib.pyplot as plt

for input_type in ['Step', 'Impulse', 'Harmonic']:
    try:
        df = pd.read_csv(f'output_{input_type}.csv')
    except FileNotFoundError:
        print(f"Файл output_{input_type}.csv не найден. Сначала запусти C++ программу!")
        continue
        
    plt.figure(figsize=(10, 5))
    plt.plot(df['Step'], df['u'], label='Input (u)', linestyle='--', color='black')
    plt.plot(df['Step'], df['Model 1.7'], label='Model 1.7')
    plt.plot(df['Step'], df['Model 2.1'], label='Model 2.1')
    plt.plot(df['Step'], df['Model 3.5'], label='Model 3.5')
    
    plt.title(f'Simulation results: {input_type}')
    plt.xlabel('Time steps (t)')
    plt.ylabel('y(t)')
    plt.legend()
    plt.grid(True)
    plt.savefig(f'doc/plot_{input_type}.png')
    plt.show()