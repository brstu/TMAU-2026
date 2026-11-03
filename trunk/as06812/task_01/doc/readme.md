<p align="center"> Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">“Брестский Государственный технический университет”</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №1</p>
<p align="center">По дисциплине “теория и методы автоматического управления”</p>
<p align="center">Тема: “Моделирование контролируемых объектов”</p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 3 курса</p>
<p align="right">Группы АС-68</p>
<p align="right">Пошвенчук Г. В.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

---

## Математические уравнения моделей (Вариант 2)
1. **Линейная модель (Model 1.2 - System with Pure Control Delay k) :**
   $$Y_{\tau+1} = a Y_{\tau} + b u_{\tau-k}$$
2. **Нелинейная модель (Model 2.4 - Logarithmic Multi-Variable Interaction):**
   $$Y_{\tau+1} = a Y_{\tau} \cdot \cos(Y_{\tau}) + b \ln(1 + |u_{\tau}|)$$
3. **Дифференциальное уравнение (Model 3.6 - Cubic Growth and Control):**
   $$\frac{dy}{dt} = a y^3 + b u$$
   Численное решение методом Эйлера: $y_{\tau+1} = y_\tau + \Delta t \cdot (a y_\tau^3 + b u_\tau)$

 
## Архитектура классов (UML-диаграмма)
```mermaid
classDiagram
    direction TB

    class Model {
        <<abstract>>
        -y_curr : double
        +processStep(u : double) double*
        +resetState() void*
        +getY() double
        +setY(value : double) void
        +~Model()*
    }

    class Model_1_2 {
        -a : double
        -b : double
        -k : int
        -u_history : queue~double~
        +processStep(u : double) double
        +resetState() void
    }

    class Model_2_4 {
        -a : double
        -b : double
        +processStep(u : double) double
        +resetState() void
    }

    class Model_3_6 {
        -a : double
        -b : double
        -dt : double
        +processStep(u : double) double
        +resetState() void
    }

    class InputSignal {
        -type_ : Type
        +getValue(t : int) double
        +toString() string
        +getType() Type
    }
    
    class InputSignal_Type {
        <<enumeration>>
        STEP
        IMPULSE
        HARMONIC
    }

    class Simulator {
        -steps : int
        +run(model : Model&, modelName : string, signal : InputSignal&) void
    }

    Model <|-- Model_1_2
    Model <|-- Model_2_4
    Model <|-- Model_3_6

    Simulator ..> Model : uses
    Simulator ..> InputSignal : uses
    InputSignal *-- InputSignal_Type : contains
```

## Результаты моделирования и визуализация
Для каждой из трёх моделей выполняется симуляция при ступенчатом, импульсном и гармоническом воздействиях, а полученные данные автоматически экспортируются в `.csv` файлы.

### Графики переходных процессов (Python/Matplotlib)
На основании табличных данных симуляции средствами библиотеки Matplotlib построены графики переходных процессов объектов управления:

<p align="center">
  <img src="Model_1_2_Step.png" width="32%" />
  <img src="Model_1_2_Impulse.png" width="32%" />
  <img src="Model_1_2_Harmonic.png" width="32%" />
</p>

<p align="center">
  <img src="Model_2_4_Step.png" width="32%" />
  <img src="Model_2_4_Impulse.png" width="32%" />
  <img src="Model_2_4_Harmonic.png" width="32%" />
</p>

<p align="center">
  <img src="Model_3_6_Step.png" width="32%" />
  <img src="Model_3_6_Impulse.png" width="32%" />
  <img src="Model_3_6_Harmonic.png" width="32%" />
</p>

