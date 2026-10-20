<p align="center">Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">“Брестский Государственный технический университет”</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №1</p>
<p align="center">По дисциплине “Теория и методы автоматического управления”</p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 3 курса</p>
<p align="right">Группы АС-67</p>
<p align="right">Ковалевич М. М.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>




## Вариант 1

- Линейная модель: **Model 1.1 (Standard First-Order Inertial System)**
- Нелинейная модель: **Model 2.3 (Dead-Zone Non-linearity)**
- Дифференциальное уравнение: **Model 3.5 (Harmonic Driving Force)**

## Реализованные модели

### Model 1.1 (Standard First-Order Inertial System)

```math
y_{\tau+1} = a\,y_\tau + b\,u_\tau
```

### Model 2.3 (Dead-Zone Non-linearity)

```math
y_{\tau+1} = a\,y_\tau + b\cdot \mathrm{deadzone}(u_\tau)
```

```math
\mathrm{deadzone}(u) =
\begin{cases}
u - \delta, & u > \delta \\
0, & -\delta \le u \le \delta \\
u + \delta, & u < -\delta
\end{cases}
```

### Model 3.5 (Harmonic Driving Force)

```math
\frac{dy}{dt} = b\,\sin(u)
```

Для данной модели используется Метод Эйлера:

```math
y_{\tau+1} = y_\tau + dt \cdot b\,\sin(u_\tau)
```

## Входные воздействия

1. Ступенчатое: `u(t) = A`
2. Импульсное: `u(0) = A`, `u(t > 0) = 0`
3. Гармоническое: `u(t) = A*sin(t)`

## ООП

Создан абстрактный базовый класс `Model` с чисто виртуальным методом `calculateNext(y, u, dt)`, который по текущему значению `y(t)`, входу `u(t)` и шагу `dt` вычисляет `y(t+1)`.

От него наследуются:

- `Model1_1`
- `Model2_3`
- `Model3_5`

### UML:
```mermaid
classDiagram
    class Model {
        <<abstract>>
        +calculateNext(y, u, dt) double*
        +getName() const char**
    }

    class Model1_1 {
        -a double
        -b double
    }

    class Model2_3 {
        -a double
        -b double
        -delta double
        -deadzone(u) double
    }

    class Model3_5 {
        -b double
    }

    Model <|-- Model1_1
    Model <|-- Model2_3
    Model <|-- Model3_5
```

## Функционал программы

Программа:

- позволяет выбрать модель;
- принимает коэффициенты;
- позволяет выбрать входное воздействие;
- позволяет задать количество шагов `n`;
- выводит результаты в табличном виде;
- сохраняет результаты в `results.csv`;
- позволяет построить график через Python/Matplotlib.

## Пример работы программы:

### Демонстрация исполняемого файла tmau_task_01.exe:

![](work.png)

### Демонстрация ступенчатого воздействия
![Результат выполнения файла plot.py](plot.png)
### Демонстрация импульсного воздействия
![](plot2.png)
### Демонстрация гармонического воздействия
![](plot3.png)
