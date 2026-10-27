#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <filesystem>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#ifndef PROJECT_SOURCE_DIR
#define PROJECT_SOURCE_DIR "."
#endif

struct Sample {
    int step;
    double input;
    double output;
};

enum class InputType { Step = 1, Impulse = 2, Harmonic = 3 };

class InputSignal {
public:
    explicit InputSignal(InputType type) : type_(type) {}

    double value(int tau) const {
        switch (type_) {
            case InputType::Step: return 1.0;
            case InputType::Impulse: return tau == 0 ? 1.0 : 0.0;
            case InputType::Harmonic: return std::sin(static_cast<double>(tau));
        }
        return 0.0;
    }

private:
    InputType type_;
};

class Model {
public:
    virtual ~Model() = default;
    virtual double next(double y, double u, double previousU, double dt) const = 0;
    virtual std::string name() const = 0;

    std::vector<Sample> simulate(int n, double y0, double dt,
                                 const InputSignal& signal) const {
        std::vector<Sample> samples;
        samples.reserve(static_cast<std::size_t>(n + 1));
        double y = y0;
        double previousU = signal.value(0);
        samples.push_back({0, previousU, y});

        for (int tau = 0; tau < n; ++tau) {
            const double u = signal.value(tau);
            const double nextY = next(y, u, previousU, dt);
            if (!std::isfinite(nextY)) {
                throw std::runtime_error("Получено недопустимое значение. Проверьте параметры модели.");
            }
            y = nextY;
            previousU = u;
            samples.push_back({tau + 1, signal.value(tau + 1), y});
        }
        return samples;
    }
};

// Model 1.4: y(tau+1) = a*y(tau) + b*(u(tau)-u(tau-1))
class LinearIntegratorLoss final : public Model {
public:
    LinearIntegratorLoss(double a, double b) : a_(a), b_(b) {}
    double next(double y, double u, double previousU, double) const override {
        return a_ * y + b_ * (u - previousU);
    }
    std::string name() const override { return "Model 1.4: Linear Integrator with Loss"; }
private:
    double a_, b_;
};

// Model 2.9: y(tau+1) = a*y(tau) + b*|u|*sign(u) = a*y(tau) + b*u
class SquareRootModulatedAction final : public Model {
public:
    SquareRootModulatedAction(double a, double b) : a_(a), b_(b) {}
    double next(double y, double u, double, double) const override {
        return a_ * y + b_ * std::abs(u) * sign(u);
    }
    std::string name() const override { return "Model 2.9: Square Root Modulated Action"; }
private:
    static double sign(double value) {
        if (value > 0.0) return 1.0;
        if (value < 0.0) return -1.0;
        return 0.0;
    }
    double a_, b_;
};

// Model 3.1: dy/dt = -a*y, explicit Euler: y(k+1) = y(k) - dt*a*y(k)
class PureLinearDecay final : public Model {
public:
    explicit PureLinearDecay(double a) : a_(a) {}
    double next(double y, double, double, double dt) const override {
        return y - dt * a_ * y;
    }
    std::string name() const override { return "Model 3.1: Pure Linear Decay (Euler)"; }
private:
    double a_;
};

void clearInputLine() {
    std::cin.ignore(1000000, '\n');
}

double readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && std::isfinite(value)) return value;
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
        std::cout << "Ошибка: введите целое число от " << minValue
                  << " до " << maxValue << ".\n";
        std::cin.clear();
        clearInputLine();
    }
}

int runPlot(const std::filesystem::path& csvPath, int modelChoice) {
    const std::filesystem::path projectDir = std::filesystem::path(PROJECT_SOURCE_DIR);
    const std::filesystem::path plotPath = projectDir / "plot.py";

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

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        std::cout << "=== Моделирование математических моделей ===\n"
                  << "Выберите модель:\n"
                  << "1. Линейная модель 1.4\n"
                  << "2. Модель 2.9 — Square Root Modulated Action\n"
                  << "3. Дифференциальная модель 3.1 — Pure Linear Decay\n";
        const int modelChoice = readInt("Модель (1-3): ", 1, 3);

        std::cout << "\nТип входного воздействия:\n"
                  << "1. Ступенчатое: u(tau) = 1\n"
                  << "2. Импульсное: u(0) = 1, далее 0\n"
                  << "3. Гармоническое: u(tau) = sin(tau)\n";
        const int signalChoice = readInt("Воздействие (1-3): ", 1, 3);
        const int n = readInt("Количество шагов n (1-100000): ", 1, 100000);
        const double y0 = readDouble("Начальное значение y0: ");
        const double dt = readDouble("Шаг времени delta_t (положительное число): ");
        if (dt <= 0.0) {
            std::cerr << "Ошибка: delta_t должен быть больше нуля.\n";
            return 1;
        }

        std::unique_ptr<Model> model;
        if (modelChoice == 1) {
            const double a = readDouble("Коэффициент a: ");
            const double b = readDouble("Коэффициент b: ");
            model = std::make_unique<LinearIntegratorLoss>(a, b);
        } else if (modelChoice == 2) {
            const double a = readDouble("Коэффициент a: ");
            const double b = readDouble("Коэффициент b: ");
            model = std::make_unique<SquareRootModulatedAction>(a, b);
        } else {
            const double a = readDouble("Коэффициент a: ");
            model = std::make_unique<PureLinearDecay>(a);
        }

        const InputSignal signal(static_cast<InputType>(signalChoice));
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
