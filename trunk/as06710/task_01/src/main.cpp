#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Signal.h"
#include "Model.h"
#include "Model_1_1.h"
#include "Model_2_3.h"
#include "Model_3_5.h"

struct Row
{
    int tau;
    double u;
    double y;
};

static int readInt(const std::string& prompt, int minValue)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value && value >= minValue)
            return value;
        std::cout << "Ошибка ввода. Повторите.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

static double readDouble(const std::string& prompt)
{
    double value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value && std::isfinite(value))
            return value;
        std::cout << "Ошибка ввода. Повторите.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

static SignalType readSignalType()
{
    const int choice = readInt(
        "\nТип входного воздействия:\n"
        "1 - ступенчатое\n"
        "2 - импульсное\n"
        "3 - гармоническое\n"
        "Ваш выбор: ", 1);
    if (choice > 3)
        return readSignalType();
    return static_cast<SignalType>(choice);
}

static std::unique_ptr<Model> createModel()
{
    const int choice = readInt(
        "\nВыберите модель:\n"
        "1 - Model 1.1\n"
        "2 - Model 2.3\n"
        "3 - Model 3.5\n"
        "Ваш выбор: ", 1);

    switch (choice)
    {
    case 1:
    {
        const double a = readDouble("a = ");
        const double b = readDouble("b = ");
        return std::make_unique<Model1_1>(a, b);
    }
    case 2:
    {
        const double a     = readDouble("a = ");
        const double b     = readDouble("b = ");
        const double delta = readDouble("delta = ");
        return std::make_unique<Model2_3>(a, b, delta);
    }
    case 3:
    {
        const double b = readDouble("b = ");
        return std::make_unique<Model3_5>(b);
    }
    default:
        std::cout << "Нет такой модели. Повторите выбор.\n";
        return createModel();
    }
}

static void printTable(const std::vector<Row>& rows)
{
    std::cout << "\n"
              << std::setw(10)  << "tau"
              << std::setw(20) << "u"
              << std::setw(20) << "y"
              << '\n';
    std::cout << std::string(50, '-') << '\n';
    for (const Row& row : rows)
    {
        std::cout << std::setw(10) << row.tau
                  << std::setw(20) << std::fixed << std::setprecision(6) << row.u
                  << std::setw(20) << row.y
                  << '\n';
    }
}

static void saveCsv(const std::vector<Row>& rows)
{
    std::ofstream file("results.csv");
    if (!file)
    {
        std::cerr << "Не удалось открыть results.csv для записи.\n";
        return;
    }
    file << "tau,u,y\n";
    for (const Row& row : rows)
        file << row.tau << ',' << row.u << ',' << row.y << '\n';
    std::cout << "\nРезультаты сохранены в results.csv\n";
}

int main()
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    std::cout << "============================================\n";
    std::cout << " TMAU-2026 | Лабораторная работа №1\n";
    std::cout << " Вариант 1\n";
    std::cout << "============================================\n";

    std::unique_ptr<Model> model = createModel();
    const SignalType signal = readSignalType();
    const double amplitude  = readDouble("Амплитуда A = ");
    const int n             = readInt("Количество шагов n = ", 1);
    const double y0         = readDouble("Начальное значение y0 = ");

    double dt = 1.0;

    if (std::string(model->getName()).find("Model 3.5") != std::string::npos)
        dt = readDouble("Шаг интегрирования dt = ");

    std::vector<Row> rows;
    rows.reserve(n + 1);

    double y = y0;

    rows.push_back({0, getInput(signal, 0, amplitude), y});

    for (int tau = 0; tau < n; ++tau){
        const double u     = getInput(signal, tau, amplitude);
        const double nextY = model->calculateNext(y, u, dt);   // ← 3 аргумента
        y = nextY;

        const double uNext = getInput(signal, tau + 1, amplitude);
        rows.push_back({tau + 1, uNext, y});
    }

    std::cout << "\nМодель: " << model->getName() << '\n';
    printTable(rows);
    saveCsv(rows);

    std::system("python plot.py");
    std::system("pause");
    return 0;
}