#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif
#include "input_signal.h"
#include "model.h"
#include "model1-8.h"
#include "model2-10.h"
#include "model3-2.h"
#include "stability_core.h"

static double readNumber(const std::string& msg)
{
    double val;
    std::cout << msg;
    while (!(std::cin >> val))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Ошибка ввода, повторите: ";
    }
    return val;
}

int main()
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
    std::cout << "TMAU-2026 | Лабораторная работа №2 | Вариант 8/10/2\n";

    const auto pick = static_cast<int>(
        readNumber("\nМодель (1 - ArModel, 2 - TanhModel, 3 - IntegratorModel): "));
    std::unique_ptr<Model> mdl;
    double a1 = 0.0, a2 = 0.0;

    switch (pick)
    {
    case 1:
    {
        std::cout << "Коэффициенты AR:\n";
        a1 = readNumber("  a1 = ");
        a2 = readNumber("  a2 = ");
        const double b1 = readNumber("  b1 = ");
        const double b2 = readNumber("  b2 = ");
        mdl = std::make_unique<ArModel>(a1, a2, b1, b2);
        break;
    }
    case 2:
    {
        std::cout << "Коэффициенты Tanh:\n";
        const double alpha = readNumber("  alpha = ");
        const double beta  = readNumber("  beta  = ");
        mdl = std::make_unique<TanhModel>(alpha, beta);
        break;
    }
    case 3:
    {
        std::cout << "Коэффициент Integrator:\n";
        const double beta = readNumber("  beta = ");
        mdl = std::make_unique<IntegratorModel>(beta);
        break;
    }
    default:
        std::cout << "Нет такой модели.\n";
        return 1;
    }

    const auto kind = static_cast<int>(
        readNumber("Воздействие (1 - ступень, 2 - импульс, 3 - гармоника): "));
    const double amp = readNumber("Амплитуда A = ");
    const auto cnt = static_cast<int>(readNumber("Количество шагов n = "));
    const double y0 = readNumber("Начальное значение y0 = ");

    if (pick == 1)
        warnIfUnstable(a1, a2, std::cout);

    mdl->setInitial(y0);
    std::cout << "\nМодель: " << mdl->getName() << "\n";
    std::cout << std::setw(6)  << "tau"
              << std::setw(14) << "u"
              << std::setw(14) << "y" << "\n";
    std::cout << std::fixed << std::setprecision(6);

    double u_prev1 = 0.0;
    double u_prev2 = 0.0;

    for (int tau = 1; tau <= cnt; ++tau)
    {
        const double u_now = InputSignal::getSignal(
            static_cast<InputType>(kind), tau, amp);

        const double y_now = mdl->calculateStep(u_now, u_prev1, u_prev2);

        std::cout << std::setw(6)  << tau
                  << std::setw(14) << u_now
                  << std::setw(14) << y_now << "\n";

        u_prev2 = u_prev1;
        u_prev1 = u_now;
    }
    return 0;
}