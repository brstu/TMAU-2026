import sys
import os
import pandas as pd

# Важно: принудительно используем TkAgg для отображения окна на Windows
import matplotlib
matplotlib.use('TkAgg') 
import matplotlib.pyplot as plt

# Проверяем, передал ли C++ программу имя файла
if len(sys.argv) < 2:
    print("Ошибка: Не передано имя CSV-файла.")
    sys.exit(1)

filename = sys.argv[1]

if not os.path.exists(filename):
    print(f"Ошибка: Файл {filename} не найден!")
    sys.exit(1)

# Читаем данные
try:
    df = pd.read_csv(filename)
except Exception as e:
    print(f"Ошибка при чтении файла: {e}")
    sys.exit(1)

# Красивое название для заголовка в зависимости от модели
title_text = "TMAU-2026 — результат моделирования, вариант 11"
if "model1" in filename:
    title_text += "\n(Model 1.1)"
elif "model2" in filename:
    title_text += "\n(Model 2.5)"
elif "model3" in filename:
    title_text += "\n(Model 3.9)"

# Настраиваем график
fig, ax = plt.subplots(figsize=(10, 7))
ax.plot(df['tau'], df['y'], label='y(t)', linewidth=2, color='#1f77b4')

ax.set_title(title_text, fontsize=16)
ax.set_xlabel(r'Время', fontsize=14)
ax.set_ylabel('y - значение функции', fontsize=14)
ax.grid(True, linestyle='-', color='0.75')
ax.legend(loc='upper left', fontsize=12)
ax.tick_params(axis='both', which='major', labelsize=12)

plt.tight_layout()

# Сохраняем график в файл (чтобы потом вставить в отчет)
save_name = filename.replace('.csv', '.png')
plt.savefig(save_name, dpi=150)
print(f"График сохранен как {save_name}")

# ВАЖНО: Эта команда открывает окно с графиком на экране
# Программа будет ждать, пока вы не закроете окно
plt.show() 