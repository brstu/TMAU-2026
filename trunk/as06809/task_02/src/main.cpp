#ifdef VAR16_STATIC_LIB
#include "StabilityAnalyzer.h"

#include <cmath>
#include <stdexcept>

namespace var16 {

AbstractDynamicModel::~AbstractDynamicModel() = default;

InputFn createInputFn(InputMode mode) {
    switch (mode) {
        case InputMode::Step:
            return [](int) { return 1.0; };
        case InputMode::Impulse:
            return [](int step) { return step == 0 ? 1.0 : 0.0; };
        case InputMode::Harmonic:
            return [](int step) { return std::sin(static_cast<double>(step)); };
    }
    throw std::invalid_argument("Указан неизвестный режим входного воздействия.");
}

ThirdOrderLinearModel::ThirdOrderLinearModel(double a1, double a2, double a3, double b)
    : a1_(a1), a2_(a2), a3_(a3), b_(b) {
    if (!std::isfinite(a1) || !std::isfinite(a2) || !std::isfinite(a3) || !std::isfinite(b)) {
        throw std::invalid_argument("Коэффициенты модели 1.6 должны быть конечными числами.");
    }
}

double ThirdOrderLinearModel::computeStep(const StepFrame& frame) const {
    return a1_ * frame.y0 + a2_ * frame.y1 + a3_ * frame.y2 + b_ * frame.u0;
}

std::string_view ThirdOrderLinearModel::label() const {
    return "Model 1.6: Third-Order Dynamic System";
}

TanhCubicNonlinearModel::TanhCubicNonlinearModel(double a, double b)
    : a_(a), b_(b) {
    if (!std::isfinite(a) || !std::isfinite(b)) {
        throw std::invalid_argument("Коэффициенты модели 2.10 должны быть конечными числами.");
    }
}

double TanhCubicNonlinearModel::computeStep(const StepFrame& frame) const {
    return a_ * std::tanh(frame.y0) + b_ * frame.u0 * frame.u0 * frame.u0;
}

std::string_view TanhCubicNonlinearModel::label() const {
    return "Model 2.10: Hyperbolic Tangent Smoothing";
}

QuadraticSelfDecayModel::QuadraticSelfDecayModel(double a) : a_(a) {
    if (!std::isfinite(a)) {
        throw std::invalid_argument("Коэффициент модели 3.4 должен быть конечным числом.");
    }
}

double QuadraticSelfDecayModel::computeStep(const StepFrame& frame) const {
    const double dy = -a_ * frame.y0 * frame.y0;
    return frame.y0 + frame.dt * dy;
}

std::string_view QuadraticSelfDecayModel::label() const {
    return "Model 3.4: Quadratic Self-Decay (Euler)";
}

} // namespace var16
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

#ifndef VAR16_PROJECT_DIR
#define VAR16_PROJECT_DIR "."
#endif

namespace {

void resetStdinLine() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

double inputFiniteDouble(const std::string& message) {
    double num = 0.0;
    while (true) {
        std::cout << message;
        if (std::cin >> num && std::isfinite(num)) {
            return num;
        }
        if (std::cin.eof()) {
            throw std::ios_base::failure("Ввод прерван (конец потока).");
        }
        std::cout << "Ошибка: введите корректное конечное число.\n";
        resetStdinLine();
    }
}

int inputBoundedInt(const std::string& message, int minVal, int maxVal) {
    int num = 0;
    while (true) {
        std::cout << message;
        if (std::cin >> num && num >= minVal && num <= maxVal) {
            return num;
        }
        if (std::cin.eof()) {
            throw std::ios_base::failure("Ввод прерван (конец потока).");
        }
        std::cout << "Ошибка: допустимо целое число от " << minVal << " до " << maxVal << ".\n";
        resetStdinLine();
    }
}

int openPythonPlot(const std::filesystem::path& csvPath, int modelNum) {
    const auto pyScript = std::filesystem::path(VAR16_CHART_SCRIPT);
#ifdef _WIN32
    const std::wstring command = L"py \"" + pyScript.wstring() +
                                 L"\" \"" + csvPath.wstring() +
                                 L"\" " + std::to_wstring(modelNum);
    return _wsystem(command.c_str());
#else
    const std::string command = "python3 \"" + pyScript.string() +
                                "\" \"" + csvPath.string() +
                                "\" " + std::to_string(modelNum);
    return std::system(command.c_str());
#endif
}

} // namespace

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    const bool drawPlot = (argc == 1);
    if (!drawPlot && (argc != 2 || std::string_view(argv[1]) != "--no-plot")) {
        std::cerr << "Использование: task02 [--no-plot]\n";
        return 1;
    }

    try {
        std::cout << "=== Расчёт динамических моделей (Вариант 16, as06809) ===\n"
                  << "Выберите объект моделирования:\n"
                  << "  1. Модель 1.6  — линейная динамическая система 3-го порядка\n"
                  << "  2. Модель 2.10 — нелинейное сглаживание гиперболическим тангенсом\n"
                  << "  3. Модель 3.4  — дифференциальное уравнение квадратичного затухания\n";
        const int modelChoice = inputBoundedInt("Ваш выбор (1-3): ", 1, 3);

        std::cout << "\nВыберите форму входного воздействия:\n"
                  << "  1. Единичная ступенька: u(tau) = 1\n"
                  << "  2. Единичный импульс: u(0) = 1, иначе 0\n"
                  << "  3. Гармонический сигнал: u(tau) = sin(tau)\n";
        const int signalChoice = inputBoundedInt("Форма сигнала (1-3): ", 1, 3);
        const int totalSteps = inputBoundedInt("Количество тактов n (1-100000): ", 1, 100000);
        const double startState = inputFiniteDouble("Начальное состояние y0: ");

        double deltaT = 1.0;
        std::unique_ptr<var16::AbstractDynamicModel> activeModel;

        if (modelChoice == 1) {
            const double a1 = inputFiniteDouble("Коэффициент a1: ");
            const double a2 = inputFiniteDouble("Коэффициент a2: ");
            const double a3 = inputFiniteDouble("Коэффициент a3: ");
            const double b = inputFiniteDouble("Коэффициент b: ");
            activeModel = std::make_unique<var16::ThirdOrderLinearModel>(a1, a2, a3, b);
            var16::reportStabilityStatus(std::cout, var16::evaluateThirdOrderStability(a1, a2, a3));
            std::cout << std::flush;
        } else if (modelChoice == 2) {
            const double a = inputFiniteDouble("Коэффициент a: ");
            const double b = inputFiniteDouble("Коэффициент b: ");
            activeModel = std::make_unique<var16::TanhCubicNonlinearModel>(a, b);
        } else {
            const double a = inputFiniteDouble("Коэффициент a: ");
            deltaT = inputFiniteDouble("Шаг времени delta_t (> 0): ");
            activeModel = std::make_unique<var16::QuadraticSelfDecayModel>(a);
        }

        const auto inputFn = var16::createInputFn(static_cast<var16::InputMode>(signalChoice));
        const auto results = activeModel->execute(totalSteps, startState, deltaT, inputFn);

        const auto rootDir = std::filesystem::path(VAR16_PROJECT_DIR);
        const auto csvPath = rootDir / "results.csv";
        std::ofstream csvStream(csvPath);
        if (!csvStream) {
            throw std::ios_base::failure("Не удалось создать файл results.csv.");
        }

        csvStream << "tau,u,y\n" << std::setprecision(12);
        std::cout << "\n" << activeModel->label() << "\n"
                  << std::left << std::setw(10) << "tau"
                  << std::setw(18) << "u(tau)" << "y(tau)\n"
                  << std::string(44, '-') << '\n';

        for (const auto& row : results) {
            std::cout << std::left << std::setw(10) << row.step
                      << std::setw(18) << std::setprecision(8) << row.uVal
                      << row.yVal << '\n';
            csvStream << row.step << ',' << std::setprecision(12)
                      << row.uVal << ',' << row.yVal << '\n';
        }
        csvStream.close();
        std::cout << "\nДанные записаны в: " << csvPath.string() << '\n';

        if (!drawPlot) {
            return 0;
        }
        std::cout << "Построение графика...\n" << std::flush;
        if (openPythonPlot(csvPath, modelChoice) != 0) {
            std::cerr << "Ошибка запуска Matplotlib. Проверьте наличие Python и matplotlib.\n";
        }
    } catch (const std::exception& exc) {
        std::cerr << "Сбой выполнения: " << exc.what() << '\n';
        return 1;
    }
    return 0;
}
#endif
