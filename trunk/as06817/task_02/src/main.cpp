#include <cctype>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "CsvWriter.h"
#include "InputSignal.h"
#include "Model17.h"
#include "Model21.h"
#include "Model35.h"
#include "Simulator.h"
#include "StabilityAnalyzer.h"

namespace {

int readInt(const std::string& prompt, int defaultValue) {
    while (true) {
        std::cout << prompt << " [" << defaultValue << "]: ";
        std::string line;
        if (!std::getline(std::cin, line) || line.empty()) {
            return defaultValue;
        }

        try {
            std::size_t processed = 0;
            const int value = std::stoi(line, &processed);
            if (processed == line.size()) {
                return value;
            }
        } catch (const std::exception&) {
            // Ниже будет выведено сообщение и запрошен повторный ввод.
        }

        std::cout << "Некорректный ввод, повторите.\n";
    }
}

double readDouble(const std::string& prompt, double defaultValue) {
    while (true) {
        std::cout << prompt << " [" << defaultValue << "]: ";
        std::string line;
        if (!std::getline(std::cin, line) || line.empty()) {
            return defaultValue;
        }

        try {
            std::size_t processed = 0;
            const double value = std::stod(line, &processed);
            if (processed == line.size()) {
                return value;
            }
        } catch (const std::exception&) {
            // Ниже будет выведено сообщение и запрошен повторный ввод.
        }

        std::cout << "Некорректный ввод, повторите.\n";
    }
}

} // namespace

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "Лабораторная работа №2\n";
    std::cout << "ТиМАУ, вариант 17: Model 1.7, Model 2.1, Model 3.5\n\n";

    const int steps = readInt("Введите количество шагов n", 20);
    if (steps <= 0) {
        std::cerr << "Количество шагов должно быть больше нуля.\n";
        return 1;
    }

    const int signalChoice = readInt(
        "Выберите входное воздействие (1 - ступенчатое, 2 - импульсное, "
        "3 - гармоническое)",
        1);

    if (signalChoice < 1 || signalChoice > 3) {
        std::cerr << "Неверный выбор входного воздействия.\n";
        return 1;
    }

    const SignalType signalType = static_cast<SignalType>(signalChoice);

    std::cout << "\nКоэффициенты по умолчанию:\n";
    std::cout << "  Model 1.7: a=0.7, b1=0.5, b2=0.2, b3=0.1\n";
    std::cout << "  Model 2.1: a=0.5, b=0.05, c=0.8, d=0.2\n";
    std::cout << "  Model 3.5: b=1.0, dt=0.1\n";

    const bool customCoefficients =
        readInt("Ввести коэффициенты вручную? (1 - да, 0 - нет)", 0) == 1;

    double a17 = 0.7;
    double b117 = 0.5;
    double b217 = 0.2;
    double b317 = 0.1;

    double a21 = 0.5;
    double b21 = 0.05;
    double c21 = 0.8;
    double d21 = 0.2;

    double b35 = 1.0;
    double dt35 = 0.1;

    if (customCoefficients) {
        std::cout << "\nModel 1.7\n";
        a17 = readDouble("  a", a17);
        b117 = readDouble("  b1", b117);
        b217 = readDouble("  b2", b217);
        b317 = readDouble("  b3", b317);

        std::cout << "\nModel 2.1\n";
        a21 = readDouble("  a", a21);
        b21 = readDouble("  b", b21);
        c21 = readDouble("  c", c21);
        d21 = readDouble("  d", d21);

        std::cout << "\nModel 3.5\n";
        b35 = readDouble("  b", b35);
        dt35 = readDouble("  dt", dt35);
    }

    InputSignal signal(signalType);

    std::vector<std::unique_ptr<IModel>> models;
    models.push_back(std::make_unique<Model17>(a17, b117, b217, b317));
    models.push_back(std::make_unique<Model21>(a21, b21, c21, d21));
    models.push_back(std::make_unique<Model35>(b35, dt35));

    const std::vector<std::string> fileNames = {
        "model17.csv",
        "model21.csv",
        "model35.csv"
    };

    std::cout << "\n=== Анализ устойчивости ===\n";
    for (const auto& model : models) {
        const StabilityReport report = StabilityAnalyzer::analyze(*model);
        std::cout << model->name() << ": " << report.message;
        if (report.analyzed && !report.roots.empty()) {
            std::cout << " |z|max = " << std::fixed << std::setprecision(6)
                      << report.maxMagnitude;
        }
        std::cout << "\n";
    }

    for (std::size_t i = 0; i < models.size(); ++i) {
        std::cout << "\n=== " << models[i]->name() << " ===\n";

        Simulator simulator(*models[i], signal, steps);
        const SimulationResult result = simulator.run();

        std::cout << std::setw(6) << "tau"
                  << std::setw(16) << "u"
                  << std::setw(16) << "y" << "\n";
        std::cout << std::string(38, '-') << "\n";

        for (const auto& step : result.steps) {
            std::cout << std::setw(6) << step.tau
                      << std::setw(16) << std::fixed << std::setprecision(6)
                      << step.u
                      << std::setw(16) << step.y << "\n";
        }

        try {
            CsvWriter::write(fileNames[i], result);
            std::cout << "Результаты сохранены в: " << fileNames[i] << "\n";
        } catch (const std::exception& error) {
            std::cerr << error.what() << "\n";
        }
    }

    std::cout << "\nМоделирование завершено.\n";
    return 0;
}
