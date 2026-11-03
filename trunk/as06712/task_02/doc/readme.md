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
<p align="right">Лискович И. Г.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

## Вариант 3

- Линейная модель: **Model 1.3**
- Нелинейная модель: **Model 2.5**
- Дифференциальное уравнение: **Model 3.7**

## Реализованные модели

### Model 1.3 (Second-Order Oscillatory/Inertial System)

$$y_{\tau+1} = a_1 \cdot y_{\tau} + a_2 \cdot y_{\tau-1} + b \cdot u_{\tau}$$

### Model 2.5 (Signum Friction and Exponential Growth)

$$y_{\tau+1} = a \cdot y_{\tau} + b \cdot \text{sign}(u_{\tau}) \cdot \left(1 - e^{-|u_{\tau}|}\right)$$

### Model 3.7 (Exponential Scaling Process)

$$\frac{dy}{dt} = -e^a \cdot y + b \cdot u \quad\Rightarrow\quad y_{\tau+1} = y_{\tau} + \Delta t \cdot \left(-e^a \cdot y_{\tau} + b \cdot u_{\tau}\right)$$

## Анализ устойчивости систем

Для линейной модели 1.3 реализован модуль `StabilityAnalyzer`. Характеристическое уравнение системы:
$$z^2 - a_1 z - a_2 = 0$$

Критерий устойчивости на Z-плоскости: система устойчива, если оба корня уравнения лежат строго внутри единичной окружности ($|z_1| < 1$ и $|z_2| < 1$). Если пользователь задает коэффициенты $a_1$ и $a_2$, приводящие к расходящемуся процессу, программа выводит предупреждение в консоль перед началом симуляции.

## Модульное тестирование

В ходе работы были написаны модульные тесты с использованием фреймворка **Google Test**:
1. **Параметризованные тесты (`TEST_P`):** Использованы для проверки расчета шагов модели 1.3 при различных входных наборах данных без дублирования кода.
2. **Mock-объекты (Google Mock):** Создан мок `MockSignal` для проверки того, как класс `Simulator` взаимодействует с генератором сигналов.
3. **Граничные значения:** Проверены границы устойчивости Z-плоскости ($|z| = 1$), а также границы функции `sign(u)` для нелинейной модели.
4. **Инварианты:** Проверено состояние покоя системы при нулевом входе и нулевом начальном состоянии.

### Тестовое покрытие и результаты

* **Способ запуска:** Локальный скрипт (`run_tests.bat`)
* **Всего модульных тестов:** 13
* **Успешно пройденных тестов:** 13 / 13 (100%)

| Модуль / Класс | Покрытие строк (Line Coverage) | Покрытие функций (Func Coverage) | Статус |
| :--- | :---: | :---: | :---: |
| `StabilityAnalyzer.h` | 100 % | 100 % | OK |
| `LinearModel13.h` | 100 % | 100 % | OK |
| `ISignalGenerator.h` | 100 % | 100 % | OK |
| `SignalGenerator.h` | 90 % | 100 % | OK |
| `Simulator.h` | 90 % | 100 % | OK |
| `NonlinearModel25.h` | 88 % | 100 % | OK |
| `ODE37.h` | 84 % | 100 % | OK |
| **Итоговый результат** | **91 %** | **100 %** | **Success** |

> **Примечание:** Покрытие составляет 91%, так как некоторые строки кода физически не могут быть выполнены в рамках тестов (например, пустая виртуальная функция `setInitialState` в `Model.h`, либо ветка `return 0.0` в `SignalGenerator.h`, до которой код не доходит, так как все 3 типа сигналов обработаны в `switch`). Оставшиеся непокрытыми методы `name()` в нелинейных моделях не влияют на математическую логику.

## ООП Архитектура

Создан абстрактный базовый класс `Model` с чистыми виртуальными методами `step()` и `reset()`, а также методом `name()`. От базового класса наследуются модели `LinearModel13`, `NonlinearModel25` и `ODE37`.

Для реализации тестирования с помощью Google Mock был создан интерфейс `ISignalGenerator`, от которого наследуется реальный класс `SignalGenerator`. Это позволяет подменять входные сигналы в тестах.

## UML Диаграмма классов

```mermaid
classDiagram
    class Model {
        <<abstract>>
        +step(double u)* double
        +reset()* void
        +name()* const char*
    }

    class LinearModel13 {
        -double a1_
        -double a2_
        -double b_
        -double y_curr_
        -double y_prev_
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

    class ODE37 {
        -double a_
        -double b_
        -double dt_
        -double y_
        +step(double u) double
        +reset() void
        +name() const char*
    }

    class StabilityAnalyzer {
        +isStableLinear13(double a1, double a2) bool$
    }

    class ISignalGenerator {
        <<interface>>
        +get(double tau)* double
    }

    class SignalGenerator {
        -SignalType type_
        -double amplitude_
        +get(double tau) double
    }

    class Simulator {
        -unique_ptr~Model~ model_
        -int n_
        +run(ISignalGenerator signal) vector~double~
    }

    Model <|-- LinearModel13
    Model <|-- NonlinearModel25
    Model <|-- ODE37
    
    ISignalGenerator <|.. SignalGenerator
    Simulator --> Model : owns
    Simulator --> ISignalGenerator : uses
    LinearModel13 ..> StabilityAnalyzer : uses
