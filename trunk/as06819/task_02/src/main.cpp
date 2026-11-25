#ifdef TASK02_SIMULATION_LIBRARY
#include "StabilityAnalyzer.h"

#include <cmath>
#include <stdexcept>

namespace task02 {

Model::~Model() = default;

InputSignal makeInputSignal(InputType type) {
    switch (type) {
        case InputType::Step: return [](int) { return 1.0; };
        case InputType::Impulse: return [](int step) { return step == 0 ? 1.0 : 0.0; };
        case InputType::Harmonic: return [](int step) { return std::sin(step); };
    }
    throw std::invalid_argument("Неизвестный тип входного сигнала.");
}

std::vector<Sample> Model::simulate(int steps, double y0, double dt,
                                    const InputSignal& signal) const {
    if (steps < 0 || !std::isfinite(y0) || !std::isfinite(dt) || dt <= 0.0) {
        throw std::invalid_argument("Недопустимые параметры симуляции.");
    }

    std::vector<Sample> samples;
    samples.reserve(static_cast<std::size_t>(steps) + 1);
    double y = y0;

    for (int step = 0; ; ++step) {
        const double u = signal(step);
        if (!std::isfinite(u)) {
            throw std::runtime_error("Получено недопустимое значение входа.");
        }
        samples.push_back({step, u, y});
        if (step == steps) {
            break;
        }

        const double uPrev2 = step >= 2 ? samples[step - 2].input : 0.0;
        y = next({y, u, uPrev2, dt});
        if (!std::isfinite(y)) {
            throw std::runtime_error("Получено недопустимое значение выхода.");
        }
    }
    return samples;
}

DistributedControlWindow::DistributedControlWindow(double a1, double b1, double b2)
    : a1_(a1), b1_(b1), b2_(b2) {
    if (!std::isfinite(a1) || !std::isfinite(b1) || !std::isfinite(b2)) {
        throw std::invalid_argument("Коэффициенты модели 1.10 должны быть конечными.");
    }
}

double DistributedControlWindow::next(const StepContext& ctx) const {
    return a1_ * ctx.y + b1_ * ctx.u + b2_ * ctx.uPrev2;
}

std::string_view DistributedControlWindow::name() const {
    return "Model 1.10: Distributed Control Window Model";
}

LogarithmicInteraction::LogarithmicInteraction(double a, double b)
    : a_(a), b_(b) {
    if (!std::isfinite(a) || !std::isfinite(b)) {
        throw std::invalid_argument("Коэффициенты модели 2.4 должны быть конечными.");
    }
}

double LogarithmicInteraction::next(const StepContext& ctx) const {
    return a_ * ctx.y * std::cos(ctx.y) + b_ * std::log(1.0 + std::abs(ctx.u));
}

std::string_view LogarithmicInteraction::name() const {
    return "Model 2.4: Logarithmic Multi-Variable Interaction";
}

CombinedQuadraticDecay::CombinedQuadraticDecay(double a1, double a2, double b)
    : a1_(a1), a2_(a2), b_(b) {
    if (!std::isfinite(a1) || !std::isfinite(a2) || !std::isfinite(b)) {
        throw std::invalid_argument("Коэффициенты модели 3.8 должны быть конечными.");
    }
}

double CombinedQuadraticDecay::next(const StepContext& ctx) const {
    const double dydt = -a1_ * ctx.y - a2_ * ctx.y * ctx.y + b_ * ctx.u;
    return ctx.y + ctx.dt * dydt;
}

std::string_view CombinedQuadraticDecay::name() const {
    return "Model 3.8: Combined Linear-Quadratic Decay (Euler)";
}

} // namespace task02
#else
#include "StabilityAnalyzer.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <filesystem>
#include <cstdlib>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#ifndef PROJECT_SOURCE_DIR
#define PROJECT_SOURCE_DIR "."
#endif

using namespace task02;

void clearInputLine() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

double readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && std::isfinite(value)) return value;
        if (std::cin.eof()) throw std::ios_base::failure("Ввод завершён.");
        std::cout << "Ошибка: введите конечное числовое значение.\n";
        std::cin.clear();
        clearInputLine();
    }
}

int readInt(const std::string& prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minValue && value <= maxValue) return value;
        if (std::cin.eof()) throw std::ios_base::failure("Ввод завершён.");
        std::cout << "Ошибка: введите целое число от " << minValue
                  << " до " << maxValue << ".\n";
        std::cin.clear();
        clearInputLine();
    }
}

int runPlot(const std::filesystem::path& csvPath, int modelChoice) {
    const auto plotPath = std::filesystem::path(TASK02_PLOT_SCRIPT);

#ifdef _WIN32
    const std::wstring command = L"py \"" + plotPath.wstring() +
                                 L"\" \"" + csvPath.wstring() +
                                 L"\" " + std::to_wstring(modelChoice);
    return _wsystem(command.c_str());
#else
    const std::string command = "python3 \"" + plotPath.string() +
                                "\" \"" + csvPath.string() +
                                "\" " + std::to_string(modelChoice);
    return std::system(command.c_str());
#endif
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    const bool showPlot = argc == 1;
    if (!showPlot && (argc != 2 || std::string_view(argv[1]) != "--no-plot")) {
        std::cerr << "Использование: task02 [--no-plot]\n";
        return 1;
    }
    try {
        std::cout << "=== Моделирование математических моделей (Вариант 20) ===\n"
                  << "Выберите модель:\n"
                  << "1. Линейная модель 1.10 — распределённое окно управления\n"
                  << "2. Нелинейная модель 2.4 — логарифмическое взаимодействие\n"
                  << "3. Дифференциальная модель 3.8 — линейно-квадратичное затухание\n";
        const int modelChoice = readInt("Модель (1-3): ", 1, 3);

        std::cout << "\nТип входного воздействия:\n"
                  << "1. Ступенчатое: u(tau) = 1\n"
                  << "2. Импульсное: u(0) = 1, далее 0\n"
                  << "3. Гармоническое: u(tau) = sin(tau)\n";
        const int signalChoice = readInt("Воздействие (1-3): ", 1, 3);
        const int n = readInt("Количество шагов n (1-100000): ", 1, 100000);
        const double y0 = readDouble("Начальное значение y0: ");
        double dt = 1.0;
        std::unique_ptr<Model> model;
        if (modelChoice == 1) {
            const double a1 = readDouble("Коэффициент a1: ");
            const double b1 = readDouble("Коэффициент b1: ");
            const double b2 = readDouble("Коэффициент b2: ");
            model = std::make_unique<DistributedControlWindow>(a1, b1, b2);
            writeStabilityWarning(std::cout, analyzeDistributedControl(a1));
            std::cout << std::flush;
        } else if (modelChoice == 2) {
            const double a = readDouble("Коэффициент a: ");
            const double b = readDouble("Коэффициент b: ");
            model = std::make_unique<LogarithmicInteraction>(a, b);
        } else {
            const double a1 = readDouble("Коэффициент a1: ");
            const double a2 = readDouble("Коэффициент a2: ");
            const double b = readDouble("Коэффициент b: ");
            dt = readDouble("Шаг времени delta_t (положительное число): ");
            model = std::make_unique<CombinedQuadraticDecay>(a1, a2, b);
        }

        const InputSignal signal = makeInputSignal(static_cast<InputType>(signalChoice));
        const std::vector<Sample> samples = model->simulate(n, y0, dt, signal);

        const auto projectDir = std::filesystem::path(PROJECT_SOURCE_DIR);
        const std::filesystem::path csvPath = projectDir / "results.csv";
        std::ofstream csv(csvPath);
        if (!csv) throw std::ios_base::failure("Не удалось создать results.csv в каталоге проекта.");
        csv << "tau,u,y\n" << std::setprecision(12);
        std::cout << "\n" << model->name() << "\n";
        std::cout << std::left << std::setw(10) << "tau"
                  << std::setw(18) << "u(tau)" << "y(tau)\n";
        std::cout << std::string(44, '-') << '\n';
        for (const auto& sample : samples) {
            std::cout << std::left << std::setw(10) << sample.step
                      << std::setw(18) << std::setprecision(8) << sample.input
                      << sample.output << '\n';
            csv << sample.step << ',' << std::setprecision(12)
                << sample.input << ',' << sample.output << '\n';
        }
        csv.close();
        std::cout << "\nРезультаты сохранены в: " << csvPath.string() << '\n';
        if (!showPlot) return 0;
        std::cout << "Построение графика...\n" << std::flush;

        const int plotResult = runPlot(csvPath, modelChoice);
        if (plotResult != 0) {
            std::cerr << "Не удалось автоматически открыть график.\n"
                      << "Проверьте Python launcher (py) и Matplotlib.\n"
                      << "Установка Matplotlib: py -m pip install matplotlib\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
#endif
