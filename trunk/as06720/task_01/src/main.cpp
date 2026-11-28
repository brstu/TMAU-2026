#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "InputSignal.h"
#include "Model1_5.h"
#include "Model2_9.h"
#include "Model3_3.h"

namespace {

    constexpr double kAnyLow = std::numeric_limits<double>::lowest();
    constexpr double kAnyHigh = std::numeric_limits<double>::max();
    const std::string kOutputDir = "doc";

    // ===================== Консольный ввод =====================

    bool isBlank(std::string_view text) {
        return std::all_of(text.begin(), text.end(),
                           [](unsigned char c) { return std::isspace(c) != 0; });
    }

    // Строка должна целиком состоять из одного числа нужного типа
    template <typename T>
    bool parseWhole(const std::string& text, T& value) {
        std::istringstream stream(text);
        std::string tail;
        return static_cast<bool>(stream >> value) && !(stream >> tail);
    }

    // Запрос значения: пустая строка или конец ввода дают значение по умолчанию
    template <typename T>
    T ask(std::string_view label, T fallback, T low, T high) {
        while (true) {
            std::cout << label << " [" << fallback << "]: ";
            std::string line;
            if (!std::getline(std::cin, line) || isBlank(line)) {
                return fallback;
            }
            if (T value{}; parseWhole(line, value) && value >= low && value <= high) {
                return value;
            }
            std::cout << "  ! expected a number from " << low << " to " << high << '\n';
        }
    }

    double askCoefficient(std::string_view label, double fallback) {
        return ask(label, fallback, kAnyLow, kAnyHigh);
    }

    int askMenu(std::string_view title, const std::vector<std::string>& items) {
        std::cout << '\n' << title << '\n';
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << "  " << (i + 1) << ") " << items[i] << '\n';
        }
        const auto last = static_cast<int>(items.size());
        return ask("Your choice", last, 1, last);
    }

    // ===================== Создание моделей =====================

    struct NamedModel {
        std::string tag;
        std::unique_ptr<Model> model;
    };

    NamedModel setupLinear() {
        std::cout << "\n[Model 1.5] linear, state delay\n";
        const double a1 = askCoefficient("  a1", 0.5);
        const double a2 = askCoefficient("  a2", 0.3);
        const double b = askCoefficient("  b", 1.0);
        const int delay = ask("  k (delay)", 2, 1, 50);
        return { "Linear_1_5", std::make_unique<Model1_5>(a1, a2, b, delay) };
    }

    NamedModel setupNonlinear() {
        std::cout << "\n[Model 2.9] nonlinear, square root action\n";
        const double a = askCoefficient("  a", 0.7);
        const double b = askCoefficient("  b", 1.0);
        return { "Nonlinear_2_9", std::make_unique<Model2_9>(a, b) };
    }

    NamedModel setupDifferential() {
        std::cout << "\n[Model 3.3] first-order ODE, Euler method\n";
        const double a = askCoefficient("  a", 1.0);
        const double b = askCoefficient("  b", 1.0);
        const double dt = askCoefficient("  dt", 0.1);
        return { "DiffEquation_3_3", std::make_unique<Model3_3>(a, b, dt) };
    }

    std::vector<NamedModel> selectModels(int choice) {
        using Factory = NamedModel (*)();
        const std::vector<Factory> factories = { setupLinear, setupNonlinear, setupDifferential };

        std::vector<NamedModel> chosen;
        for (size_t i = 0; i < factories.size(); ++i) {
            const bool wanted = (choice == 4) || (choice == static_cast<int>(i) + 1);
            if (wanted) {
                chosen.push_back(factories[i]());
            }
        }
        return chosen;
    }

    // ===================== Устойчивость =====================

    void reportStability(const NamedModel& item) {
        std::string details;
        const bool converges = item.model->isStable(details);
        std::cout << "\n* Stability of " << item.tag << '\n'
                  << "  " << details << '\n';
        if (converges) {
            std::cout << "  result: stable, the process converges\n";
        } else {
            std::cout << "  [WARNING] unstable coefficients, the process DIVERGES!\n";
        }
    }

    // ===================== Симуляция и вывод =====================

    void printRule() {
        std::cout << std::string(38, '-') << '\n';
    }

    std::string simulate(NamedModel& item, const InputSignal& input, int steps) {
        item.model->reset();

        const std::string fileName = kOutputDir + "/" + item.tag + "_" + input.name() + ".csv";
        std::ofstream csv(fileName);
        if (!csv) {
            std::cout << "  ! cannot open " << fileName << '\n';
            return {};
        }
        csv << "Step,u,y\n";

        std::cout << "\n== " << item.tag << " / " << input.name() << " ==\n";
        printRule();
        std::cout << std::setw(8) << "step" << " | " << std::setw(10) << "input u"
                  << " | " << std::setw(12) << "output y" << '\n';
        printRule();

        for (int t = 0; t < steps; ++t) {
            const double u = input.valueAt(t);
            const double y = item.model->calculateNext(u);
            csv << t << "," << u << "," << y << "\n";
            std::cout << std::setw(8) << t << " | "
                      << std::setw(10) << std::fixed << std::setprecision(4) << u << " | "
                      << std::setw(12) << y << '\n';
        }
        printRule();
        std::cout << "  -> " << fileName << '\n';
        return fileName;
    }

} // namespace

int main() {
    std::cout << "Lab 1, variant 15: simulation of controlled objects\n"
              << "  1.5  y(t+1) = a1*y(t) + a2*y(t-k) + b*u(t)\n"
              << "  2.9  y(t+1) = a*y(t) + b*sqrt(|u(t)|)*sign(u(t))\n"
              << "  3.3  dy/dt = -a*y + b*u, Euler method\n"
              << "Press Enter to keep the value shown in brackets.\n\n";

    const int steps = ask("Number of simulation steps n", 30, 1, 1000);

    const int modelChoice = askMenu("Which model to simulate?",
        { "linear 1.5", "nonlinear 2.9", "differential 3.3", "all three" });
    const int signalChoice = askMenu("Which input signal?",
        { "step, u = 1", "impulse, u(0) = 1", "harmonic, u = sin(t)", "all three" });

    std::vector<NamedModel> models = selectModels(modelChoice);
    const auto inputs = makeSignals(signalChoice);

    for (const auto& item : models) {
        reportStability(item);
    }

    std::error_code ignored;
    std::filesystem::create_directories(kOutputDir, ignored);

    std::vector<std::string> written;
    for (auto& item : models) {
        for (const auto& input : inputs) {
            if (std::string path = simulate(item, *input, steps); !path.empty()) {
                written.push_back(path);
            }
        }
    }

    // Перечень CSV-файлов текущего запуска для скрипта plot.py
    std::ofstream index(kOutputDir + "/last_run.txt");
    for (const auto& path : written) {
        index << path << "\n";
    }

    std::cout << "\nDone, CSV files created: " << written.size() << '\n';
    return 0;
}
