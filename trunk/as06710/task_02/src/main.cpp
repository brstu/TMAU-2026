#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Signal.h"
#include "Model.h"
#include "Model_1_1.h"
#include "Model_2_3.h"
#include "Model_3_5.h"
#include "StabilityAnalyzer.h"

static double ask(const std::string& text)
{
    double value;
    std::cout << text;
    while (!(std::cin >> value))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Ошибка ввода, повторите: ";
    }
    return value;
}

int main()
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    std::cout << "TMAU-2026 | Лабораторная работа №2 | Вариант 1\n";

    const auto choice = static_cast<int>(ask("\nМодель (1 - Model 1.1, 2 - Model 2.3, 3 - Model 3.5): "));

    std::unique_ptr<Model> model;
    double dt = 1.0;
    double a = 0.0;

    if (choice == 1)
    {
        a = ask("a = ");
        const double b = ask("b = ");
        model = std::make_unique<Model1_1>(a, b);
    }
    else if (choice == 2)
    {
        const double a2    = ask("a = ");
        const double b     = ask("b = ");
        const double delta = ask("delta = ");
        model = std::make_unique<Model2_3>(a2, b, delta);
    }
    else if (choice == 3)
    {
        const double b = ask("b = ");
        model = std::make_unique<Model3_5>(b);
        dt = ask("Шаг интегрирования dt = ");
    }
    else
    {
        std::cout << "Нет такой модели.\n";
        return 1;
    }

    const auto type = static_cast<int>(ask("Воздействие (1 - ступень, 2 - импульс, 3 - гармоника): "));
    const double amplitude = ask("Амплитуда A = ");
    const auto n = static_cast<int>(ask("Количество шагов n = "));
    double y = ask("Начальное значение y0 = ");

    if (choice == 1)
        warnIfUnstable(a, std::cout);

    std::cout << "\nМодель: " << model->getName() << "\n";
    std::cout << std::setw(6) << "tau" << std::setw(14) << "u" << std::setw(14) << "y" << "\n";
    std::cout << std::fixed << std::setprecision(6);

    for (int tau = 0; tau <= n; ++tau)
    {
        const double u = getInput(static_cast<SignalType>(type), tau, amplitude);
        std::cout << std::setw(6) << tau << std::setw(14) << u << std::setw(14) << y << "\n";
        y = model->calculateNext(y, u, dt);
    }
    return 0;
}
