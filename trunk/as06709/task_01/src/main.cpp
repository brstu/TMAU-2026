#include "LinearModel17.h"
#include "NonlinearModel21.h"
#include "DifferentialModel35.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

enum class Signal { Step = 1, Impulse = 2, Harmonic = 3 };

struct Row {
    int tau;
    double u;
    double y;
};

double readDouble(const std::string& prompt) {
    double value{};
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && std::isfinite(value)) return value;
        std::cout << "Некорректное число. Повторите ввод.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int readInt(const std::string& prompt, int minValue, int maxValue) {
    int value{};
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minValue && value <= maxValue) return value;
        std::cout << "Введите целое число от " << minValue << " до " << maxValue << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double getInput(Signal signal, int tau, double stepValue) {
    switch (signal) {
        case Signal::Step: return stepValue;
        case Signal::Impulse: return tau == 0 ? 1.0 : 0.0;
        case Signal::Harmonic: return std::sin(static_cast<double>(tau));
    }
    return 0.0;
}

std::string signalLabel(Signal signal) {
    switch (signal) {
        case Signal::Step: return "step";
        case Signal::Impulse: return "impulse";
        case Signal::Harmonic: return "harmonic";
    }
    return "unknown";
}

bool saveCsv(const std::vector<Row>& rows, const std::string& model,
             const std::string& signal) {
    std::ofstream file("results.csv");
    if (!file) return false;
    file << "model,signal,tau,u,y\n" << std::setprecision(15);
    for (const auto& r : rows)
        file << model << ',' << signal << ',' << r.tau << ',' << r.u << ',' << r.y << '\n';
    return true;
}

int main() {
#ifdef _WIN32
    // Keep the Windows console and UTF-8 source literals in the same encoding.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::cout << "============================================\n";
    std::cout << " LAB 1 / VARIANT 17 - MODEL SIMULATOR\n";
    std::cout << "============================================\n\n";
    std::cout << "Выберите модель (за один запуск рассчитывается только одна):\n";
    std::cout << "1 - Линейная Model 1.7\n";
    std::cout << "2 - Нелинейная Model 2.1\n";
    std::cout << "3 - Дифференциальная Model 3.5\n";
    const int modelChoice = readInt("Ваш выбор: ", 1, 3);

    std::cout << "\nВыберите входное воздействие:\n";
    std::cout << "1 - Ступенчатое u(tau) = const\n";
    std::cout << "2 - Импульсное u(0)=1, далее 0\n";
    std::cout << "3 - Гармоническое u(tau) = sin(tau)\n";
    const int signalChoice = readInt("Ваш выбор: ", 1, 3);
    const auto signal = static_cast<Signal>(signalChoice);

    const int n = readInt("\nКоличество шагов n (1..10000): ", 1, 10000);
    double dt = 1.0;
    if (modelChoice == 3)
        dt = readDouble("Шаг интегрирования dt (>0): ");
    while (modelChoice == 3 && dt <= 0.0) {
        std::cout << "dt должен быть больше нуля.\n";
        dt = readDouble("Введите dt: ");
    }

    auto model = std::unique_ptr<Model>{};
    std::string modelName;
    if (modelChoice == 1) {
        std::cout << "\nВведите коэффициенты линейной модели 1.7:\n";
        const double a = readDouble("a = ");
        const double b1 = readDouble("b1 = ");
        const double b2 = readDouble("b2 = ");
        const double b3 = readDouble("b3 = ");
        if (std::abs(a) >= 1.0)
            std::cout << "ПРЕДУПРЕЖДЕНИЕ: |a| >= 1, однородная часть модели неустойчива.\n";
        else
            std::cout << "Проверка устойчивости однородной части: |a| < 1.\n";
        model = std::make_unique<LinearModel17>(a, b1, b2, b3);
        modelName = "Model_1.7";
    } else if (modelChoice == 2) {
        std::cout << "\nВведите коэффициенты нелинейной модели 2.1:\n";
        const double a = readDouble("a = ");
        const double b = readDouble("b = ");
        const double c = readDouble("c = ");
        const double d = readDouble("d = ");
        model = std::make_unique<NonlinearModel21>(a, b, c, d);
        modelName = "Model_2.1";
    } else {
        std::cout << "\nВведите коэффициент дифференциальной модели 3.5:\n";
        const double b = readDouble("b = ");
        model = std::make_unique<DifferentialModel35>(b);
        modelName = "Model_3.5";
    }

    double stepValue = 1.0;
    if (signal == Signal::Step)
        stepValue = readDouble("\nЗначение постоянного воздействия const = ");

    std::vector<Row> rows;
    rows.reserve(static_cast<std::size_t>(n));
    model->reset();

    for (int tau = 0; tau < n; ++tau) {
        const double u = getInput(signal, tau, stepValue);
        const double y = model->step(u, dt);
        rows.push_back({tau, u, y});
    }

    std::cout << "\nРезультаты моделирования: " << modelName
              << ", сигнал: " << signalLabel(signal) << "\n";
    std::cout << std::left << std::setw(10) << "tau"
              << std::setw(18) << "u(tau)" << std::setw(18) << "y(tau+1)" << '\n';
    std::cout << std::string(46, '-') << '\n';
    std::cout << std::fixed << std::setprecision(6);
    for (const auto& r : rows)
        std::cout << std::left << std::setw(10) << r.tau
                  << std::setw(18) << r.u << std::setw(18) << r.y << '\n';

    if (!saveCsv(rows, modelName, signalLabel(signal))) {
        std::cerr << "Не удалось создать results.csv\n";
        return 1;
    }
    std::cout << "\nДанные сохранены в results.csv.\n";
    std::cout << "Сейчас будет запущено окно с графиком результатов.\n";
    return 0;
}
