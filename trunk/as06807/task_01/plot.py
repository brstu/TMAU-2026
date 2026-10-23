
import csv
import matplotlib.pyplot as plt
from collections import defaultdict

filename = "results.csv"

# Данные будут сгруппированы по модели и воздействию.
data = defaultdict(lambda: defaultdict(lambda: [[], []]))

with open(filename, "r", newline="", encoding="utf-8") as file:
    reader = csv.DictReader(file)

    for row in reader:
        model = int(row["ModelIndex"])
        input_type = row["InputType"]
        step = int(row["Step"])
        y = float(row["Y"])

        data[model][input_type][0].append(step)
        data[model][input_type][1].append(y)

if not data:
    print("В CSV-файле нет результатов.")
    raise SystemExit

print("Доступные модели:", ", ".join(map(str, sorted(data))))

try:
    model_index = int(input("Введите индекс модели для графика: "))
except ValueError:
    print("Нужно ввести целое число.")
    raise SystemExit

if model_index not in data:
    print("Модель с таким индексом не найдена.")
    raise SystemExit

plt.figure(figsize=(10, 6))

for input_type, (steps, values) in data[model_index].items():
    # Сортируем точки по времени на случай изменения порядка в CSV.
    points = sorted(zip(steps, values))
    steps = [point[0] for point in points]
    values = [point[1] for point in points]

    plt.plot(steps, values, marker="o", label=input_type)

plt.title(f"Результаты моделирования: модель {model_index}")
plt.xlabel("Шаг времени")
plt.ylabel("Выходное значение y")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
