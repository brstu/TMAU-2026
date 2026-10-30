<p align="center">Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">“Брестский Государственный технический университет”</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №1</p>
<p align="center">По дисциплине “Теория и моделирование автоматического управления”</p>
<p align="center">Тема: “Моделирование объектов управления”</p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 3 курса</p>
<p align="right">Группы АС-68</p>
<p align="right">Дворак В. А.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

---

## Авторское примечание
> Практическую работу выполнил его Величество Дворак Владислав Андреевич.

---

## Выбранный вариант и спецификация моделей (Вариант 3)

В рамках данной работы исследуется динамика процессов для трёх типов систем:

1. **Дискретная линейная динамика (Model 1.3):**
   Представляет собой инерционное звено второго порядка с разностным уравнением:
   $$y_{\tau+1} = a_1 \cdot y_{\tau} + a_2 \cdot y_{\tau-1} + b \cdot u_{\tau}$$
   * Базовые параметры эксперимента: $a_1 = 0.5$, $a_2 = 0.1$, $b = 1.0$.

2. **Нелинейный объект с трением (Model 2.5):**
   Моделирует динамический процесс с экспоненциальным насыщением и учетом знака воздействия:
   $$y_{\tau+1} = a \cdot y_{\tau} + b \cdot \text{sign}(u_{\tau}) \cdot \left(1 - e^{-|u_{\tau}|}\right)$$
   * Базовые параметры эксперимента: $a = 0.8$, $b = 0.5$.

3. **Непрерывное дифференциальное уравнение (Model 3.7):**
   Решается численным методом Эйлера первого порядка:
   $$\frac{dy}{dt} = -e^a \cdot y + b \cdot u \quad\Rightarrow\quad y_{\tau+1} = y_{\tau} + \Delta t \cdot \left(-e^a \cdot y_{\tau} + b \cdot u_{\tau}\right)$$
   * Базовые параметры эксперимента: $a = 0.1$, $b = 1.0$, $\Delta t = 0.1$.

---

## Виды тестовых воздействий

Для анализа реакции систем подавались следующие сигналы $u_{\tau}$:
* **Ступенчатый отклик (Step):** $u_{\tau} = 1.0$ (для всех $\tau$).
* **Импульсный отклик (Impulse):** $u_0 = 1.0$, $u_{\tau > 0} = 0.0$.
* **Гармонический отклик (Harmonic):** $u_{\tau} = \sin(\tau)$.

---

## Архитектура программного обеспечения (ООП)

Программа спроектирована с соблюдением принципов ООП на языке C++. Базовый класс `Model` определяет единый интерфейс для расчёта шагов симуляции (`step()`) и сброса внутреннего состояния (`reset()`). Вычисление симуляции вынесено в отдельный управляющий класс `Simulator`.

### Диаграмма классов (UML)

```mermaid
classDiagram
    class Model {
        <<abstract>>
        +step(double u)* double
        +reset()* void
        +name()* const char*
        +setInitialState(vector~double~ init) void
    }

    class LinearModel13 {
        -double a1_
        -double a2_
        -double b_
        -double y_
        -double yPrev_
        +step(double u) double
        +reset() void
        +name() const char*
    }

    class NonlinearModel25 {
        -double a_
        -double b_
        -double y_
        +step(double u) double
        +reset() void
        +name() const char*
    }

    class ODEModel37 {
        -double a_
        -double b_
        -double dt_
        -double y_
        +step(double u) double
        +reset() void
        +name() const char*
    }

    class SignalGenerator {
        -SignalType type_
        -double amplitude_
        +get(double tau) double
    }

    class Simulator {
        -unique_ptr~Model~ model_
        -int n_
        +run(SignalGenerator signal) vector~double~
        +printTable(vector~double~ y, SignalGenerator signal) void
        +exportCSV(vector~double~ y, SignalGenerator signal, string fname) void
    }

    Model <|-- LinearModel13
    Model <|-- NonlinearModel25
    Model <|-- ODEModel37
    Simulator --> Model : owns
    Simulator --> SignalGenerator : uses