# Общее задание

1. Написать отчет по выполненной лабораторной работе №1 в .md формате
   (readme.md) и с помощью запроса на внесение изменений (**pull request**)
   разместить его в следующем каталоге: **trunk\as0xxyy\task_01\doc** (где
   **xx** - номер группы, **yy** - номер студента, например **as02302**).
2. Исходный код написанной программы разместить в каталоге:
   **trunk\as0xxyy\task_01\src**.
3. Выполнить рецензирование ([review](https://linearb.io),
   [checklist](https://linearb.io)) запросов других студентов (минимум 2-е
   рецензии).
4. Отразить выполнение работы в файле readme.md в соответствующей строке
   (например, для студента под порядковым номером 1 - https://github.com).
5. Выходные данные должны быть представлены в табличном виде (в консоли или
   экспортированы в `.csv` / `.txt` файл).
6. В программе должна быть предусмотрена возможность получения нужного
   количества значений (настройка количества шагов симуляции $n$).
7. В программе должна быть визуализация полученных результатов (инструменты для
   визуализации выбрать самостоятельно: gnuplot, Python/Matplotlib, MATLAB,
   Excel или встроенные C++ графические библиотеки).
8. Создать UML Диаграмму созданной программы (диаграмму классов, отражающую
   структуру объектно-ориентированной реализации).
9. Архитектурное требование: реализовать программу на основе принципов **ООП**.
   Создать базовый абстрактный класс (например, `Model`) с виртуальным методом
   для расчета следующего шага симуляции, а конкретные уравнения реализовать в
   виде классов-наследников.
10. Для тестирования объектов использовать три типа входных воздействий
    $u_{\tau}$: ступенчатое ($u_{\tau} = \text{const}$), импульсное ($u_0 = 1,
    u_{\tau > 0} = 0$) и гармоническое ( $u_{\tau} = \sin(\tau)$ ).

### Дополнительное задание (Advanced Level): (Необязательное)

11. **Анализ устойчивости систем:** Для выбранных линейных дискретных моделей
    (из блока 1) аналитически определить критерий устойчивости (найти корни
    характеристического уравнения на Z-плоскости). В программе реализовать
    проверку: если заданные пользователем коэффициенты $a_i$ приводят к
    расходящемуся (неустойчивому) процессу, программа должна выводить
    предупреждение в консоль перед началом симуляции.

---

### Таблица вариантов

| Вариант | Линейная модель (Блок 1) | Нелинейная модель (Блок 2) | Дифференциальное уравнение (Блок 3) |
| :-----: | :----------------------: | :------------------------: | :---------------------------------: |
|  **1**  |        Model 1.1         |         Model 2.3          |              Model 3.5              |
|  **2**  |        Model 1.2         |         Model 2.4          |              Model 3.6              |
|  **3**  |        Model 1.3         |         Model 2.5          |              Model 3.7              |
|  **4**  |        Model 1.4         |         Model 2.6          |              Model 3.8              |
|  **5**  |        Model 1.5         |         Model 2.7          |              Model 3.9              |
|  **6**  |        Model 1.6         |         Model 2.8          |             Model 3.10              |
|  **7**  |        Model 1.7         |         Model 2.9          |              Model 3.1              |
|  **8**  |        Model 1.8         |         Model 2.10         |              Model 3.2              |
|  **9**  |        Model 1.9         |         Model 2.1          |              Model 3.3              |
| **10**  |        Model 1.10        |         Model 2.2          |              Model 3.4              |
| **11**  |        Model 1.1         |         Model 2.5          |              Model 3.9              |
| **12**  |        Model 1.2         |         Model 2.6          |             Model 3.10              |
| **13**  |        Model 1.3         |         Model 2.7          |              Model 3.1              |
| **14**  |        Model 1.4         |         Model 2.8          |              Model 3.2              |
| **15**  |        Model 1.5         |         Model 2.9          |              Model 3.3              |
| **16**  |        Model 1.6         |         Model 2.10         |              Model 3.4              |
| **17**  |        Model 1.7         |         Model 2.1          |              Model 3.5              |
| **18**  |        Model 1.8         |         Model 2.2          |              Model 3.6              |
| **19**  |        Model 1.9         |         Model 2.3          |              Model 3.7              |
| **20**  |        Model 1.10        |         Model 2.4          |              Model 3.8              |
| **21**  |        Model 1.2         |         Model 2.7          |              Model 3.5              |
| **22**  |        Model 1.4         |         Model 2.9          |              Model 3.1              |
| **23**  |        Model 1.6         |         Model 2.1          |              Model 3.7              |
| **24**  |        Model 1.8         |         Model 2.3          |              Model 3.3              |
| **25**  |        Model 1.10        |         Model 2.5          |             Model 3.10              |
| **26**  |        Model 1.1         |         Model 2.8          |              Model 3.6              |
| **27**  |        Model 1.3         |         Model 2.10         |              Model 3.2              |
| **28**  |        Model 1.5         |         Model 2.2          |              Model 3.8              |
| **29**  |        Model 1.7         |         Model 2.4          |              Model 3.4              |
| **30**  |        Model 1.9         |         Model 2.6          |              Model 3.9              |

---

## Task 1. Modeling controlled object (Extended Version)

Let's get some object to be controlled. We want to simulate its behavior and
temperature transition processes using a wide range of mathematical
approximations.

The task is to write a program (**C++**) that implements **10 linear models**,
**10 non-linear models**, and **10 simple differential equations**, simulates
their behavior over discrete time moments $\tau$ ($1,2,3{\dots}n$), and outputs
the results.

---

### 1. Linear Models (Линейные модели)

- **Model 1.1 (Standard First-Order Inertial System):** $$\Large y_{\tau+1} =
  ay_{\tau} + bu_{\tau}$$
- **Model 1.2 (System with Pure Control Delay $k$):** $$\Large y_{\tau+1} =
  ay_{\tau} + bu_{\tau-k}$$
- **Model 1.3 (Second-Order Oscillatory/Inertial System):** $$\Large y_{\tau+1}
  = a_1y_{\tau} + a_2y_{\tau-1} + bu_{\tau}$$
- **Model 1.4 (Discrete Integrator with Loss):** $$\Large y_{\tau+1} = ay_{\tau} + b(u_{\tau} - u_{\tau-1})$$
- **Model 1.5 (System with State Delay):** $$\Large y_{\tau+1} = a_1y_{\tau} +
  a_2y_{\tau-k} + bu_{\tau}$$
- **Model 1.6 (Third-Order Dynamic System):**
$$\Large y_{\tau+1} = a_1y_{\tau} + a_2y_{\tau-1} + a_3y_{\tau-2} + bu_{\tau}$$
- **Model 1.7 (System with Multi-Step Control History):** $$\Large y_{\tau+1} =
  ay_{\tau} + b_1u_{\tau} + b_2u_{\tau-1} + b_3u_{\tau-2}$$
- **Model 1.8 (Generalized Autoregressive Linear Model):** $$\Large y_{\tau+1} =
  a_1y_{\tau} + a_2y_{\tau-1} + b_1u_{\tau} + b_2u_{\tau-1}$$
- **Model 1.9 (Three-Step Moving Average State Model):** $$\Large y_{\tau+1} =
  a_1y_{\tau} + a_2y_{\tau-1} + a_3y_{\tau-2} + b_1u_{\tau}$$
- **Model 1.10 (Distributed Control Window Model):** $$\Large y_{\tau+1} =
  a_1y_{\tau} + b_1u_{\tau} + b_2u_{\tau-2}$$

Where $a, a_i, b, b_i$ — constant coefficients; $k$ — delay step ($k \ge 1$).

---

### 2. Non-linear Models (Нелинейные модели)

- **Model 2.1 (Quadratic Feedback and Harmonic Control):** $$\Large y_{\tau+1} =
  ay_{\tau} - by_{\tau-1}^2 + cu_{\tau} + d\sin(u_{\tau-1})$$
- **Model 2.2 (Actuator Saturation Non-linearity):** $$\Large y_{\tau+1} =
  ay_{\tau} + b \cdot \text{sat}(u_{\tau})$$

$$ \text{where } \text{sat}(u) = \begin{cases} U_{\max}, & u > U_{\max} \\
u, & U_{\min} \le u \le U_{\max} \\
U_{\min}, & u < U_{\min} \end{cases} $$

- **Model 2.3 (Dead-Zone Non-linearity):** $$\Large y_{\tau+1} = ay_{\tau} + b
  \cdot \text{deadzone}(u_{\tau})$$

$$ \text{where } \text{deadzone}(u) = \begin{cases} u - \delta, & u > \delta \\
0, & -\delta \le u \le \delta \\
u + \delta, & u < -\delta \end{cases} $$

- **Model 2.4 (Logarithmic Multi-Variable Interaction):** $$\Large y_{\tau+1} =
  ay_{\tau} \cdot \cos(y_{\tau}) + b \ln(1 + |u_{\tau}|)$$
- **Model 2.5 (Signum Friction and Exponential Growth):** $$\Large y_{\tau+1} =
  ay_{\tau} + b \cdot \text{sign}(u_{\tau}) \cdot (1 - e^{-|u_{\tau}|})$$
- **Model 2.6 (Asymmetric Polynomial Backlash Simulation):** $$\Large y_{\tau+1}
  = a_1y_{\tau}^3 - a_2y_{\tau-1} + bu_{\tau}^2$$
- **Model 2.7 (Relay with Hysteresis Element):** $$\Large y_{\tau+1} = ay_{\tau} + b \cdot \text{relay}(u_{\tau}, y_{\tau})$$

$$ \text{where } \text{relay}(u, y) = \begin{cases} 1, & u > \epsilon \text{ or
} (u \ge -\epsilon \text{ and } y_{\tau} > 0) \\
-1, & u < -\epsilon \text{ or } (u \le \epsilon \text{ and } y_{\tau} \le 0)
\end{cases} $$

- **Model 2.8 (Chaotic Logistic Map Disturbance):** $$\Large y_{\tau+1} =
  ay_{\tau}(1 - y_{\tau}) + bu_{\tau} + c\sin(y_{\tau-1} \cdot u_{\tau})$$
- **Model 2.9 (Square Root Modulated Action):** $$\Large y_{\tau+1} = ay_{\tau} + b\sqrt{|u_{\tau}|} \cdot \text{sign}(u_{\tau})$$
- **Model 2.10 (Hyperbolic Tangent Smoothing):** $$\Large y_{\tau+1} = a \cdot
  \tanh(y_{\tau}) + b \cdot u_{\tau}^3$$

Where $a, a_i, b, c, d, U_{max}, U_{min}, \delta, \epsilon$ — parameters and
physical limits of the system.

---

### 3. Simple Differential Equations (Дифференциальные уравнения)

_Continuous equations solved via Euler's method with time step_ $$\Delta t$$ _:_

- **Model 3.1 (Pure Linear Decay):** $$\Large \frac{dy}{dt} = -a \cdot y$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} - \Delta t \cdot a y_{\tau}$$
  -->
- **Model 3.2 (Pure Constant Input Drive):** $$\Large \frac{dy}{dt} = b \cdot
  u$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} + \Delta t \cdot b u_{\tau}$$
  -->
- **Model 3.3 (Standard First-Order Process):** $$\Large \frac{dy}{dt} = -a
  \cdot y + b \cdot u$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} + \Delta t (-a y_{\tau} + b u_{\tau})$$
  -->
- **Model 3.4 (Quadratic Self-Decay):** $$\Large \frac{dy}{dt} = -a \cdot y^2$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} - \Delta t \cdot a y_{\tau}^2$$
  -->
- **Model 3.5 (Harmonic Driving Force):** $$\Large \frac{dy}{dt} = b \cdot
  \sin(u)$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} + \Delta t \cdot b \sin(u_{\tau})$$
  -->
- **Model 3.6 (Cubic Growth and Control):** $$\Large \frac{dy}{dt} = ay^3 + bu$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} + \Delta t (a y_{\tau}^3 + b u_{\tau})$$
  -->
- **Model 3.7 (Exponential Scaling Process):** $$\Large \frac{dy}{dt} = -e^{a}y + bu$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} + \Delta t (-e^{a}y_{\tau} + b u_{\tau})$$
  -->
- **Model 3.8 (Combined Linear-Quadratic Decay):** $$\Large \frac{dy}{dt} =
  -a_1y - a_2y^2 + bu$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} + \Delta t (-a_1y_{\tau} - a_2y_{\tau}^2 + b u_{\tau})$$
  -->
- **Model 3.9 (Bounded Saturation Rate):** $$\Large \frac{dy}{dt} = b \cdot
  \tanh(u)$$
  <!--
  $$ \implies y_{\tau+1} = y_{\tau} + \Delta t \cdot b \tanh(u_{\tau})$$
  -->
- **Model 3.10 (Basic External Constant Offset):** $$\Large \frac{dy}{dt} = -ay + b + u$$
  <!--
  $$\implies y_{\tau+1} = y_{\tau} + \Delta t (-a y_{\tau} + b + u_{\tau})$$
  -->

Where $a, a_1, a_2, b$ — constants; $\Delta t$ — simulation time step.
