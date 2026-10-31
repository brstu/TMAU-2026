import matplotlib.pyplot as plt

time : list[int] = []
y : list[float]  = []

model : str
signal : str
with open("./result_data.csv", 'r', encoding="UTF-8") as file:
    lines : list[str] = file.readlines()

    model , signal = lines[0].split(';')
    lines.pop(0)
    for line in lines:
        raw_data : list[str] = line.split(';')
        time.append(int(raw_data[0]))
        y.append(float(raw_data[2]))

    fig, ax = plt.subplots()
    ax.plot(time, y)
    ax.set_xlabel("Шаги")
    ax.set_ylabel("Значений функции")

    plt.title("Тип модели: " + model + ".\n Тип сигнала: " + signal)
    plt.xticks(range(0, len(time), 5))
    plt.show()
