import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

# Ищем все csv файлы в текущей директории
csv_files = glob.glob("*signal*.csv")

if not csv_files:
    print("Файлы .csv не найдены. Сначала запустите C++ программу!")
    exit()

# Берем самый свежий файл (тот, который только что создала программа)
latest_file = max(csv_files, key=lambda f: os.path.getmtime(f))
print(f"Строим график для файла: {latest_file}")

try:
    df = pd.read_csv(latest_file)
    
    plt.figure(figsize=(10, 6))
    
    # Рисуем входной сигнал (пунктиром) и выходной (жирной линией)
    plt.plot(df['Time'], df['Input'], label='Вход (u)', linestyle='--', color='gray', alpha=0.7)
    plt.plot(df['Time'], df['Output'], label='Выход (y)', linewidth=2, color='blue')
    
    # Настройка осей и заголовков
    plt.title(f"Результат симуляции: {latest_file.replace('.csv', '')}", fontsize=14)
    plt.xlabel('Время (t)', fontsize=12)
    plt.ylabel('Значение', fontsize=12)
    plt.grid(True, linestyle=':', alpha=0.6)
    plt.legend(loc='best')
    
    # Сохраняем картинку
    plt.savefig('current_plot.png', dpi=300)
    print("График сохранен в файл 'current_plot.png'")
    
    # Показываем на экране
    plt.show()

except Exception as e:
    print(f"Ошибка при построении графика: {e}")