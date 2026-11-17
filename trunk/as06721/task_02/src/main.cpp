#ifdef TASK02_SIMULATION_LIBRARY
#include "StabilityAnalyzer.h"

#include <cmath>
#include <stdexcept>

namespace task02 {

Model::~Model() = default;

int Model::inputDelay() const {
    return 0;
}

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
    const int delay = inputDelay();
    for (int step = 0; ; ++step) {
        const double u = signal(step);
        if (!std::isfinite(u)) {
            throw std::runtime_error("Получено недопустимое значение входа.");
        }
        samples.push_back({step, u, y});
        if (step == steps) {
            break;
        }

        // Before step zero, the input is zero.
        const double delayedInput = step < delay ? 0.0 : samples[step - delay].input;
        y = next(y, delayedInput, dt);
        if (!std::isfinite(y)) {
            throw std::runtime_error("Получено недопустимое значение выхода.");
        }
    }
    return samples;
}

DelayedControl::DelayedControl(double a, double b, int delay)
    : a_(a), b_(b), delay_(delay) {
    if (delay < 0) {
        throw std::invalid_argument("Задержка k должна быть неотрицательной.");
    }
}

double DelayedControl::next(double y, double delayedInput, double) const {
    return a_ * y + b_ * delayedInput;
}

std::string_view DelayedControl::name() const {
    return "Model 1.2: System with Pure Control Delay";
}

int DelayedControl::inputDelay() const {
    return delay_;
}

HysteresisRelay::HysteresisRelay(double a, double b, double epsilon)
    : a_(a), b_(b), epsilon_(epsilon) {
    if (!std::isfinite(epsilon) || epsilon < 0.0) {
        throw std::invalid_argument("Порог epsilon должен быть конечным и неотрицательным.");
    }
}

double HysteresisRelay::next(double y, double input, double) const {
    const bool positive = input > epsilon_ || (input >= -epsilon_ && y > 0.0);
    return a_ * y + b_ * (positive ? 1.0 : -1.0);
}

std::string_view HysteresisRelay::name() const {
    return "Model 2.7: Relay with Hysteresis Element";
}

HarmonicForce::HarmonicForce(double b) : b_(b) {}

double HarmonicForce::next(double y, double input, double dt) const {
    return y + dt * b_ * std::sin(input);
}

std::string_view HarmonicForce::name() const {
    return "Model 3.5: Harmonic Driving Force (Euler)";
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
        if (std::cin.eof()) throw std::runtime_error("Ввод завершён.");
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
        if (std::cin.eof()) throw std::runtime_error("Ввод завершён.");
        std::cout << "Ошибка: введите целое число от " << minValue
                  << " до " << maxValue << ".\n";
        std::cin.clear();
        clearInputLine();
    }
}

int runPlot(const std::filesystem::path& csvPath, int modelChoice) {
    const std::filesystem::path plotPath = std::filesystem::path(TASK02_PLOT_SCRIPT);

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
        std::cout << "=== Моделирование математических моделей ===\n"
                  << "Выберите модель:\n"
                  << "1. Линейная модель 1.2 — чистое запаздывание управления\n"
                  << "2. Модель 2.7 — реле с гистерезисом\n"
                  << "3. Дифференциальная модель 3.5 — гармоническая сила\n";
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
            const double a = readDouble("Коэффициент a: ");
            const double b = readDouble("Коэффициент b: ");
            const int delay = readInt("Запаздывание k (0-100000 шагов): ", 0, 100000);
            model = std::make_unique<DelayedControl>(a, b, delay);
            writeStabilityWarning(std::cout, analyzeDelayedControl(a));
            std::cout << std::flush;
        } else if (modelChoice == 2) {
            const double a = readDouble("Коэффициент a: ");
            const double b = readDouble("Коэффициент b: ");
            const double epsilon = readDouble("Порог epsilon (неотрицательное число): ");
            model = std::make_unique<HysteresisRelay>(a, b, epsilon);
        } else {
            const double b = readDouble("Коэффициент b: ");
            dt = readDouble("Шаг времени delta_t (положительное число): ");
            model = std::make_unique<HarmonicForce>(b);
        }

        const auto signal = makeInputSignal(static_cast<InputType>(signalChoice));
        const auto samples = model->simulate(n, y0, dt, signal);

        const std::filesystem::path projectDir = std::filesystem::path(PROJECT_SOURCE_DIR);
        const std::filesystem::path csvPath = projectDir / "results.csv";
        std::ofstream csv(csvPath);
        if (!csv) throw std::runtime_error("Не удалось создать results.csv в каталоге проекта.");
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
