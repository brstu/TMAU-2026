<p align="center">Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">“Брестский Государственный технический университет”</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №1</p>
<p align="center">По дисциплине “Теория и методы автоматического управления”</p>
<p align="center">Тема: “Моделирование объектов управления”</p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 3 курса</p>
<p align="right">Группы АС-67</p>
<p align="right">Доста Е. В.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

---

## Вариант 6

- Линейная модель: **Model 1.6**
- Нелинейная модель: **Model 2.8**
- Дифференциальное уравнение: **Model 3.10**

## Реализованные модели

### Model 1.6 (Third-Order Dynamic System)

$$y_{\tau+1} = a_1 \cdot y_{\tau} + a_2 \cdot y_{\tau-1} + a_3 \cdot y_{\tau-2} + b \cdot u_{\tau}$$

Параметры: $a_1 = 0.7$, $a_2 = -0.2$, $a_3 = 0.1$, $b = 1.0$.


### Model 2.8 (Chaotic Logistic Map Disturbance)

$$y_{\tau+1} = a \cdot y_{\tau} \cdot (1 - y_{\tau}) + b \cdot u_{\tau} + c \cdot \sin(y_{\tau-1} \cdot u_{\tau})$$

Параметры: $a = 0.8$, $b = 0.5$, $c = 0.1$, $y_0 = 0.2$.

### Model 3.10 (Basic External Constant Offset)

$$\frac{dy}{dt} = -a \cdot y + b + u \quad\Rightarrow\quad y_{\tau+1} = y_{\tau} + \Delta t \cdot (-a \cdot y_{\tau} + b + u_{\tau})$$

Параметры: $a = 0.5$, $b = 0.1$, $\Delta t = 0.1$, $y_0 = 0$.

## Входные воздействия

1. **Ступенчатое** (Step): $u_{\tau} = \text{const} = 1$
2. **Импульсное** (Impulse): $u_0 = 1,\ u_{\tau > 0} = 0$
3. **Гармоническое** (Harmonic): $u_{\tau} = \sin(\tau)$

## ООП Архитектура

Создан абстрактный базовый интерфейс `IModel` с чисто виртуальными методами
`reset()`, `advance()` и `title()`, а также виртуальным `setInitialState()`.

От `IModel` наследуются:
- `ThirdOrderLinear` — линейная модель третьего порядка (Model 1.6)
- `LogisticMap` — нелинейная модель с логистическим отображением (Model 2.8)
- `OffsetODE` — дифференциальное уравнение (Model 3.10)

Сигналы реализованы как иерархия `ISignal` с наследниками `StepSignal`,
`ImpulseSignal`, `HarmonicSignal` и фабрикой `makeSignal()`.

Симуляция инкапсулирована в классе `Runner`, возвращающем структуру `Trace`
с рядами `u` и `y`. Результаты выводятся в консоль (`Runner::show`),
экспортируются в CSV (`Runner::dumpCsv`) и визуализируются через внешний
Python-скрипт `render_plot.py` (matplotlib).

## UML Диаграмма классов

```mermaid
classDiagram
    class IModel {
        <<abstract>>
        +reset()* void
        +advance(double u)* double
        +title()* const char*
        +setInitialState(vector~double~ init) void
    }

    class ThirdOrderLinear {
        -double a1_
        -double a2_
        -double a3_
        -double b_
        -double y0_
        -double y1_
        -double y2_
        +reset() void
        +advance(double u) double
        +title() const char*
    }

    class LogisticMap {
        -double a_
        -double b_
        -double c_
        -double y0_
        -double cur_
        -double prev_
        +reset() void
        +advance(double u) double
        +title() const char*
    }

    class OffsetODE {
        -double a_
        -double b_
        -double dt_
        -double y0_
        -double y_
        +reset() void
        +advance(double u) double
        +title() const char*
    }

    class ISignal {
        <<abstract>>
        +at(int tau)* double
        +id()* const char*
    }

    class StepSignal {
        -double amp_
        +at(int tau) double
        +id() const char*
    }

    class ImpulseSignal {
        -double amp_
        +at(int tau) double
        +id() const char*
    }

    class HarmonicSignal {
        -double amp_
        +at(int tau) double
        +id() const char*
    }

    class Trace {
        +vector~double~ u
        +vector~double~ y
    }

    class Runner {
        -unique_ptr~IModel~ model_
        -int steps_
        +execute(ISignal sig) Trace
        +show(Trace tr)$ void
        +dumpCsv(Trace tr, string path)$ void
        +diverged(Trace tr)$ bool
    }

    IModel <|-- ThirdOrderLinear
    IModel <|-- LogisticMap
    IModel <|-- OffsetODE

    ISignal <|-- StepSignal
    ISignal <|-- ImpulseSignal
    ISignal <|-- HarmonicSignal

    Runner --> IModel : owns
    Runner ..> ISignal : uses
    Runner ..> Trace : produces
```

## Результаты моделирования

Все расчёты выполнены при $n = 20$ шагах и амплитуде входа $A = 1$.
Параметры моделей — значения по умолчанию.

### 1. Model 1.6 — линейная модель третьего порядка

**Ступенчатое воздействие** ($u_\tau = 1$):

![Model 1.6 step](out_model16_step.png)

**Импульсное воздействие** ($u_0 = 1$, $u_{\tau>0} = 0$):

![Model 1.6 impulse](out_model16_impulse.png)

**Гармоническое воздействие** ($u_\tau = \sin\tau$):

![Model 1.6 harmonic](out_model16_harmonic.png)

### 2. Model 2.8 — нелинейная модель с логистическим отображением

**Ступенчатое воздействие:**

![Model 2.8 step](out_model28_step.png)

**Импульсное воздействие:**

![Model 2.8 impulse](out_model28_impulse.png)

**Гармоническое воздействие:**

![Model 2.8 harmonic](out_model28_harmonic.png)

### 3. Model 3.10 — дифференциальное уравнение (метод Эйлера)

**Ступенчатое воздействие:**

![Model 3.10 step](out_model310_step.png)

**Импульсное воздействие:**

![Model 3.10 impulse](out_model310_impulse.png)

**Гармоническое воздействие:**

![Model 3.10 harmonic](out_model310_harmonic.png)