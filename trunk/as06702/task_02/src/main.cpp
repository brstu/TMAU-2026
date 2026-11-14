#include "DifferentialModel31.h"
#include "InputSignal.h"
#include "LinearModel13.h"
#include "RelayModel27.h"
#include "ResultWriter.h"
#include "Simulator.h"
#include "StabilityAnalyzer.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif

int readInt(const std::string& prompt, int defaultValue, int minValue, int maxValue) {
    while (true) {
        std::cout << prompt << " [по умолчанию " << defaultValue << "]: ";
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) return defaultValue;
        try {
            std::size_t pos = 0;
            const int value = std::stoi(line, &pos);
            if (pos == line.size() && value >= minValue && value <= maxValue) return value;
        } catch (const std::exception&) {
        }
        std::cout << "Введите целое число от " << minValue << " до " << maxValue << ".\n";
    }
}

double readDouble(const std::string& prompt, double defaultValue) {
    while (true) {
        std::cout << prompt << " [по умолчанию " << defaultValue << "]: ";
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) return defaultValue;
        for (char& ch : line) if (ch == ',') ch = '.';
        try {
            std::size_t pos = 0;
            const double value = std::stod(line, &pos);
            if (pos == line.size()) return value;
        } catch (const std::exception&) {
        }
        std::cout << "Введите корректное число.\n";
    }
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "Лабораторная работа № 2 — вариант 13\n";
    std::cout << "==============================================\n";
    std::cout << "Выберите модель:\n";
    std::cout << "1. Model 1.3 - линейная модель второго порядка\n";
    std::cout << "2. Model 2.7 - реле с гистерезисом\n";
    std::cout << "3. Model 3.1 - дифференциальное уравнение\n";
    const int modelChoice = readInt("Номер модели", 1, 1, 3);

    std::cout << "\nВыберите входное воздействие:\n";
    std::cout << "1. Ступенчатое\n2. Импульсное\n3. Гармоническое\n";
    const int inputChoice = readInt("Номер воздействия", 1, 1, 3);
    const int n = readInt("Количество шагов n", 50, 1, 100000);
    const double dt = readDouble("Шаг интегрирования dt", 0.1);
    if (dt <= 0.0) {
        std::cerr << "Ошибка: dt должен быть положительным.\n";
        return 1;
    }

    std::unique_ptr<Model> model;
    if (modelChoice == 1) {
        const double a1 = readDouble("a1", 1.2);
        const double a2 = readDouble("a2", -0.32);
        const double b = readDouble("b", 0.5);
        const StabilityReport report = StabilityAnalyzer::analyze(a1, a2);
        std::cout << "\n--- Анализ устойчивости Model 1.3 ---\n";
        std::cout << StabilityAnalyzer::formatReport(report);
        model = std::make_unique<LinearModel13>(a1, a2, b);
    } else if (modelChoice == 2) {
        const double a = readDouble("a", 0.8);
        const double b = readDouble("b", 0.4);
        const double epsilon = readDouble("epsilon", 0.1);
        model = std::make_unique<RelayModel27>(a, b, epsilon);
    } else {
        const double a = readDouble("a", 0.4);
        model = std::make_unique<DifferentialModel31>(a);
    }

    InputKind inputKind = InputKind::Step;
    if (inputChoice == 2) inputKind = InputKind::Impulse;
    if (inputChoice == 3) inputKind = InputKind::Harmonic;
    const std::unique_ptr<InputSignal> signal = createInputSignal(inputKind);

    const std::vector<ResultRow> rows = Simulator::run(*model, *signal, n, dt);
    const std::string csvPath = std::string(PROJECT_ROOT) + "/results.csv";
    if (!ResultWriter::saveCsv(rows, csvPath)) {
        std::cerr << "Не удалось сохранить CSV: " << csvPath << '\n';
        return 1;
    }
    std::cout << "Результаты сохранены: " << csvPath << '\n';

#ifdef _WIN32
    if (_chdir(PROJECT_ROOT) != 0) {
#else
    if (chdir(PROJECT_ROOT) != 0) {
#endif
        std::cerr << "Не удалось перейти в корень проекта.\n";
        return 1;
    }

#ifdef _WIN32
    const int plotResult = std::system("py plot.py");
#else
    const int plotResult = std::system("python3 plot.py");
#endif
    if (plotResult != 0) {
        std::cerr << "Не удалось запустить plot.py.\n";
        return 1;
    }
    return 0;
}
