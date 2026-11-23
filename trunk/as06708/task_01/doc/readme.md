<p align="center">Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">“Брестский Государственный технический университет”</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №1</p>
<p align="center">По дисциплине: "Теория и методы автоматического управления"</p>
<p align="center"> </p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 2-го курса</p>
<p align="right">Группы АС-67</p>
<p align="right">Кирик А.Д.</p>
<p align="right">Проверил</p>
<p align="right">Дворанович Д.А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>
<br><br><br>

## Вариант задания 18
- Линейная модель: "Model 1.8 (Generalized Autoregressive Linear Model)"
    - $y_{\tau+1} = a_1y_{\tau}+a_2y_{\tau-1}+b_1u_{\tau}+b_2u_{\tau-1}$
- Нелинейная модель: "Model 2.2 (Actuator Saturation Non-linearity)"
    - $y_{\tau+1} = ay_{\tau}+b*sat(u_{\tau})$
    - $$ \text{where } \text{sat}(u) = \begin{cases} U_{\max}, & u > U_{\max} \\ u, & U_{\min} \le u \le U_{\max} \\ U_{\min}, & u < U_{\min} \end{cases} $$
- Дифференциальное уравнение: Model 3.6 "(Cubic Growth and Control)"
    - $\frac{dy}{dt}=ay^3+bu$
    - для этой модели используется метод эйлера: $y_{\tau+1}=y_{\tau}+dt*(ay^3+bu)$

Была создана програма, реализующая интерфейс класс "*IModel*" с помощью классов-наследнников:
- *LinearModel*
- *NoneLinearModel*
- *SimpleDifferentialModel*

## Функционал программы:
- Выбор типа генерируемого сигнала
- Выбор типа модели
- Ввод коэффициентов для каждой модели
- Выбор количества шагов симуляции
- Вывод результатов в табличном виде
- Сохранение данных в файл с расширением .csv
- Вывод графика функции

## Пример работы программы
Вывод работы программы в консоли:

![alt text](cls.png)

График линейной модели со ступенчатым сигналом:

![alt text](work_1.png)

График нелинейной модели с гармоническим сигналом:

![alt text](work_2.png)

График дифференциальной модели с импульсным сигналом:

![alt text](work_3.png)

## UML
![alt text](<UML tmau lr_1.webp>)