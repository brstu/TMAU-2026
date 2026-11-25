#ifdef VAR17_BUILD_LIBRARY
#include "StabilityAnalyzer.h"

#include <cmath>
#include <stdexcept>

namespace var17 {

BaseSystemModel::~BaseSystemModel() = default;

ControlGenerator buildControlGenerator(SignalKind kind) {
    if (kind == SignalKind::Step) {
        return [](int) { return 1.0; };
    }
    if (kind == SignalKind::Impulse) {
        return [](int tau) { return tau == 0 ? 1.0 : 0.0; };
    }
    if (kind == SignalKind::Harmonic) {
        return [](int tau) { return std::sin(static_cast<double>(tau)); };
    }
    throw std::invalid_argument("Задан неподдерживаемый тип входного сигнала.");
}

MultiStepControlSystem::MultiStepControlSystem(double a, double b1, double b2, double b3)
    : a_(a), b1_(b1), b2_(b2), b3_(b3) {
    if (!std::isfinite(a) || !std::isfinite(b1) || !std::isfinite(b2) || !std::isfinite(b3)) {
        throw std::invalid_argument("Коэффициенты модели 1.7 должны быть конечными.");
    }
}

double MultiStepControlSystem::evaluateNext(const StepInputs& in) const {
    return a_ * in.yCurr + b1_ * in.uCurr + b2_ * in.uPrev1 + b3_ * in.uPrev2;
}

std::string_view MultiStepControlSystem::title() const {
    return "Model 1.7: System with Multi-Step Control History";
}

QuadraticHarmonicSystem::QuadraticHarmonicSystem(double a, double b, double c, double d)
    : a_(a), b_(b), c_(c), d_(d) {
    if (!std::isfinite(a) || !std::isfinite(b) || !std::isfinite(c) || !std::isfinite(d)) {
        throw std::invalid_argument("Коэффициенты модели 2.1 должны быть конечными.");
    }
}

double QuadraticHarmonicSystem::evaluateNext(const StepInputs& in) const {
    return a_ * in.yCurr - b_ * in.yPrev * in.yPrev
           + c_ * in.uCurr + d_ * std::sin(in.uPrev1);
}

std::string_view QuadraticHarmonicSystem::title() const {
    return "Model 2.1: Quadratic Feedback and Harmonic Control";
}

HarmonicDriveEulerSystem::HarmonicDriveEulerSystem(double b) : b_(b) {
    if (!std::isfinite(b)) {
        throw std::invalid_argument("Коэффициент b модели 3.5 должен быть конечным.");
    }
}

double HarmonicDriveEulerSystem::evaluateNext(const StepInputs& in) const {
    const double derivative = b_ * std::sin(in.uCurr);
    return in.yCurr + in.stepSize * derivative;
}

std::string_view HarmonicDriveEulerSystem::title() const {
    return "Model 3.5: Harmonic Driving Force (Euler)";
}

} // namespace var17
#else
#include "StabilityAnalyzer.h"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#ifndef VAR17_ROOT_DIR
#define VAR17_ROOT_DIR "."
#endif

namespace {

void flushBadInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

double promptNumber(const std::string& label) {
    double val = 0.0;
    while (true) {
        std::cout << label;
        if (std::cin >> val && std::isfinite(val)) {
            return val;
        }
        if (std::cin.eof()) {
            throw std::ios_base::failure("Поток ввода неожиданно закрыт.");
        }
        std::cout << "Некорректный ввод. Укажите конечное число.\n";
        flushBadInput();
    }
}

int promptRangeInt(const std::string& label, int low, int high) {
    int val = 0;
    while (true) {
        std::cout << label;
        if (std::cin >> val && val >= low && val <= high) {
            return val;
        }
        if (std::cin.eof()) {
            throw std::ios_base::failure("Поток ввода неожиданно закрыт.");
        }
        std::cout << "Введите целое число в диапазоне [" << low << "; " << high << "].\n";
        flushBadInput();
    }
}

int launchVisualizer(const std::filesystem::path& csvFile, int modelId) {
    const auto scriptFile = std::filesystem::path(VAR17_PLOT_SCRIPT);
#ifdef _WIN32
    const std::wstring cmd = L"py \"" + scriptFile.wstring() +
                             L"\" \"" + csvFile.wstring() +
                             L"\" " + std::to_wstring(modelId);
    return _wsystem(cmd.c_str());
#else
    const std::string cmd = "python3 \"" + scriptFile.string() +
                            "\" \"" + csvFile.string() +
                            "\" " + std::to_string(modelId);
    return std::system(cmd.c_str());
#endif
}

} // namespace

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    const bool enablePlot = (argc == 1);
       if (!enablePlot && (argc != 2 || std::string_view(argv[1]) != "--no-plot")) {
        std::cerr << "Формат запуска: task02 [--no-plot]\n";
        return 1;
    }

    try {
        std::cout << "=== Симулятор динамических систем (Вариант 17, as06802) ===\n"
                  << "Доступные модели:\n"
                  << "  1. Модель 1.7 — линейная система с историей управления\n"
                  << "  2. Модель 2.1 — нелинейная квадратично-гармоническая модель\n"
                  << "  3. Модель 3.5 — дифференциальное уравнение с гармоническим приводом\n";
        const int selectedModel = promptRangeInt("Номер модели (1-3): ", 1, 3);

        std::cout << "\nВыберите тип входного сигнала:\n"
                  << "  1. Ступенчатый: u(tau) = 1\n"
                  << "  2. Импульсный: u(0) = 1, при tau > 0 равно 0\n"
                  << "  3. Гармонический: u(tau) = sin(tau)\n";
        const int selectedSignal = promptRangeInt("Тип сигнала (1-3): ", 1, 3);
        const int stepCount = promptRangeInt("Число шагов симуляции n (1-100000): ", 1, 100000);
        const double yInit = promptNumber("Начальное состояние y0: ");

        double timeStep = 1.0;
        std::unique_ptr<var17::BaseSystemModel> sysModel;

        if (selectedModel == 1) {
            const double a = promptNumber("Параметр a: ");
            const double b1 = promptNumber("Параметр b1: ");
            const double b2 = promptNumber("Параметр b2: ");
            const double b3 = promptNumber("Параметр b3: ");
            sysModel = std::make_unique<var17::MultiStepControlSystem>(a, b1, b2, b3);
            var17::printStabilityAlert(std::cout, var17::inspectLinearStability(a));
            std::cout << std::flush;
        } else if (selectedModel == 2) {
            const double a = promptNumber("Параметр a: ");
            const double b = promptNumber("Параметр b: ");
            const double c = promptNumber("Параметр c: ");
            const double d = promptNumber("Параметр d: ");
            sysModel = std::make_unique<var17::QuadraticHarmonicSystem>(a, b, c, d);
        } else {
            const double b = promptNumber("Параметр b: ");
            timeStep = promptNumber("Шаг интегрирования delta_t (> 0): ");
            sysModel = std::make_unique<var17::HarmonicDriveEulerSystem>(b);
        }

        const auto generator = var17::buildControlGenerator(
            static_cast<var17::SignalKind>(selectedSignal));
        const auto points = sysModel->runSimulation(stepCount, yInit, timeStep, generator);

        const auto baseDir = std::filesystem::path(VAR17_ROOT_DIR);
        const auto csvOut = baseDir / "results.csv";
        std::ofstream outFile(csvOut);
        if (!outFile) {
            throw std::ios_base::failure("Не удалось открыть results.csv для записи.");
        }

        outFile << "tau,u,y\n" << std::setprecision(12);
        std::cout << "\n" << sysModel->title() << "\n"
                  << std::left << std::setw(10) << "tau"
                  << std::setw(18) << "u(tau)" << "y(tau)\n"
                  << std::string(44, '-') << '\n';

        for (const auto& pt : points) {
            std::cout << std::left << std::setw(10) << pt.tau
                      << std::setw(18) << std::setprecision(8) << pt.control
                      << pt.state << '\n';
            outFile << pt.tau << ',' << std::setprecision(12)
                    << pt.control << ',' << pt.state << '\n';
        }
        outFile.close();
        std::cout << "\nТаблица сохранена в файл: " << csvOut.string() << '\n';

        if (!enablePlot) {
            return 0;
        }
        std::cout << "Запуск построения графика...\n" << std::flush;
        if (launchVisualizer(csvOut, selectedModel) != 0) {
            std::cerr << "Не удалось запустить график. Убедитесь, что установлен matplotlib.\n";
        }
    } catch (const std::exception& err) {
        std::cerr << "Ошибка выполнения: " << err.what() << '\n';
        return 1;
    }
    return 0;
}
#endif
