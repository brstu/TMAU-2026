<p align="center">Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">“Брестский Государственный технический университет”</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №2</p>
<p align="center">По дисциплине “Теория и моделирование автоматического управления”</p>
<p align="center">Тема: “Модульное тестирование и анализ устойчивости”</p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 3 курса</p>
<p align="right">Группы АС-67</p>
<p align="right">Кошелев Б. В.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

## Вариант 19

Выбранные модели:
- линейная дискретная — **Model 1.9**;
- нелинейная квадратичная — **Model 2.3**;
- процесс, заданный дифференциальным уравнением, — **Model 3.7**.

Параметры наследуются из лабораторной работы №1. Для линейной модели: `a1 = 0.4·a`, `a2 = 0.3·a`, `a3 = 0.1·a`, `b1 = b`. Для нелинейной: `a1 = a`, `b1 = 0.1`, `c1 = b`. Для дифференциального уравнения используются `a`, `b` и шаг интегрирования `Δt = 0.1`.

## Реализованные модели

### Model 1.9 — линейная дискретная система третьего порядка

$$y_{\tau+1} = a_1 y_{\tau} + a_2 y_{\tau-1} + a_3 y_{\tau-2} + b_1 u_{\tau}$$

### Model 2.3 — нелинейная квадратичная система

$$y_{\tau+1} = a_1 y_{\tau} - b_1 y_{\tau-1}^{2} + c_1 u_{\tau}$$

### Model 3.7 — экспоненциальный процесс, интегрирование методом Эйлера

$$\frac{dy}{dt} = -e^{a} y + b u \quad\Rightarrow\quad y_{\tau+1} = y_{\tau} + \Delta t\left(-e^{a} y_{\tau} + b u_{\tau}\right)$$

## Анализ устойчивости

Для линейной системы 1.9 предусмотрен отдельный модуль `StabilityAnalyzer`. Поведение определяется корнями характеристического уравнения

$$z^3 - a_1 z^2 - a_2 z - a_3 = 0,$$

которые вычисляются по формуле Кардано и затем уточняются итерациями Ньютона. Дискретная система считается устойчивой, если модули всех трёх корней меньше единицы, то есть $|z_i| < 1$; случай $|z| = 1$ трактуется как неустойчивый. В качестве независимой проверки дополнительно применяется критерий Жури для полинома третьей степени:

$$|a_3| < 1,\quad 1 - a_1 - a_2 - a_3 > 0,\quad 1 + a_1 - a_2 + a_3 > 0,\quad 1 + a_2 + a_1 a_3 - a_3^2 > 0.$$

Если введённые коэффициенты $a_i$ дают расходящийся процесс, `main.cpp` выдаёт предупреждение ещё до старта симуляции.

## Структура классов

За основу взята абстракция `Model` с набором чисто виртуальных методов `step()`, `reset()`, `name()` и необязательным `setInitialState()`. Конкретные реализации — `LinearModel19`, `NonlinearModel23` и `ODE37` — наследуются от неё.

Для моделирования различных режимов входа введён интерфейс `ISignalGenerator`. Его реализуют рабочий генератор `SignalGenerator` (ступенчатый, импульсный и гармонический сигналы) и тестовый двойник `MockSignal`. Класс `Simulator` хранит модель как `std::unique_ptr<Model>` и выполняет прогон, обращаясь к переданному генератору.

## Тестирование

Используются **Google Test** и **Google Mock**; фреймворк размещён локально в `third_party/googletest`, поэтому сборка не обращается к сети.

1. **Параметризованные тесты (`TEST_P`)** — проверяют шаги моделей 1.9, 2.3, 3.7 и выдачу генератора на нескольких наборах входных данных без копирования кода.
2. **Google Mock** — `MockSignal` заменяет реальный вход и позволяет проверить, как `Simulator` взаимодействует с генератором.
3. **Анализ устойчивости** — покрыты устойчивые и расходящиеся наборы коэффициентов, граница $|z| = 1$, согласованность критерия Жури с проверкой по корням, а также вывод предупреждения.
4. **Граничные случаи** — нулевой вход при нулевом начальном состоянии, сброс состояния модели, сверка шага Эйлера с аналитическим расчётом.

### Результаты и покрытие

* **Запуск:** скрипт `run_tests.bat` (CMake → Ninja → CTest → gcov).
* **Число тестов:** 33.
* **Пройдено:** 33 из 33 (100 %).

| Модуль / Класс | Покрытие строк (Line) | Покрытие функций (Func) | Статус |
| :--- | :---: | :---: | :---: |
| `StabilityAnalyzer.h` | 100 % (47/47) | 100 % (5/5) | OK |
| `LinearModel19.h` | 100 % (21/21) | 100 % (5/5) | OK |
| `NonlinearModel23.h` | 100 % (12/12) | 100 % (4/4) | OK |
| `ODE37.h` | 100 % (7/7) | 100 % (4/4) | OK |
| `SignalGenerator.h` | 100 % (8/8) | 100 % (2/2) | OK |
| `Simulator.h` | 100 % (9/9) | 100 % (2/2) | OK |
| `Model.h`, `ISignalGenerator.h` | 100 % (3/3) | 100 % (3/3) | OK |
| **Итоговый результат** | **100 % (107/107)** | **100 % (25/25)** | **Success** |

Отчёты лежат в `doc/test_report.txt` (протокол CTest) и `doc/coverage_report.txt` (сводка gcov). При подсчёте покрытия учитываются только исходники проекта; системные заголовки и код GoogleTest не включаются.

## UML-диаграмма классов

```mermaid
classDiagram
    class Model {
        <<abstract>>
        +step(double u)* double
        +reset()* void
        +name()* const char*
        +setInitialState(vector init) void
    }

    class LinearModel19 {
        -double a1_
        -double a2_
        -double a3_
        -double b1_
        -double y_curr_
        -double y_prev1_
        -double y_prev2_
        +step(double u) double
        +reset() void
        +name() const char*
        +setInitialState(vector init) void
    }

    class NonlinearModel23 {
        -double a1_
        -double b1_
        -double c1_
        -double y_curr_
        -double y_prev1_
        +step(double u) double
        +reset() void
        +name() const char*
    }

    class ODE37 {
        -double a_
        -double b_
        -double dt_
        -double y_curr_
        +step(double u) double
        +reset() void
        +name() const char*
    }

    class StabilityAnalyzer {
        +cubicRoots(a1, a2, a3) Roots$
        +isStable(a1, a2, a3) bool$
        +isStableJury(a1, a2, a3) bool$
        +warnIfUnstable(a1, a2, a3, out) bool$
    }

    class ISignalGenerator {
        <<interface>>
        +get(double tau)* double
    }

    class SignalGenerator {
        -InputType type_
        -double amplitude_
        +get(double tau) double
    }

    class Simulator {
        -unique_ptr~Model~ model_
        -int n_
        +run(ISignalGenerator signal) vector~double~
    }

    Model <|-- LinearModel19
    Model <|-- NonlinearModel23
    Model <|-- ODE37
    ISignalGenerator <|.. SignalGenerator
    Simulator --> Model : owns
    Simulator --> ISignalGenerator : uses
    LinearModel19 ..> StabilityAnalyzer : analyzed by