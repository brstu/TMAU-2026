#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <utility>
#include <sstream>

#include "Model.h"
#include "LinearModel13.h"
#include "NonlinearModel25.h"
#include "ODEModel37.h"
#include "SignalGenerator.h"
#include "Simulator.h"

// Парсит строку в double без глубокой вложенности
bool tryParseDouble(std::string input, double& outValue) {
    for (char& c : input) {
        if (c == ',') c = '.';
    }
    std::stringstream ss(input);
    return (ss >> outValue) && ss.eof();
}

double getValidDouble(const std::string& prompt) {
    double value;
    std::string input;
    while (true) {
        std::cout << prompt;
        if (std::cin >> input && tryParseDouble(input, value)) {
            return value;
        }
        std::cout << " Ошибка: введите корректное число (например, 0.8 или -0.2).\n";
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

int getValidInt(const std::string& prompt, int min_val = 1, int max_val = 10000) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min_val && value <= max_val) {
            return value;
        }
        std::cout << " Ошибка: введите целое число от " << min_val << " до " << max_val << ".\n";
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

int main() {
    std::cout << "=== МОДЕЛИРОВАНИЕ ДИНАМИЧЕСКИХ СИСТЕМ ===\n\n";

    std::cout << "Выберите модель для исследования:\n";
    std::cout << "1. LinearModel13 (Linear 2nd order)\n";
    std::cout << "2. NonlinearModel25 (Nonlinear friction)\n";
    std::cout << "3. ODEModel37 (ODE Euler)\n";
    
    int modelChoice = getValidInt("Ваш выбор модели (1-3) [например, 1]: ", 1, 3);

    std::cout << "\nВыберите тип входного сигнала:\n";
    std::cout << "1. Ступенчатый (STEP)\n";
    std::cout << "2. Импульсный (IMPULSE)\n";
    std::cout << "3. Гармонический (HARMONIC)\n";
    
    int signalChoice = getValidInt("Ваш выбор сигнала (1-3) [например, 1]: ", 1, 3);
    double amplitude = getValidDouble("Введите амплитуду сигнала [например, 1.0]: ");

    SignalType sigType = SignalType::STEP;
    std::string signalName = "step";
    if (signalChoice == 2) {
        sigType = SignalType::IMPULSE;
        signalName = "impulse";
    } else if (signalChoice == 3) {
        sigType = SignalType::HARMONIC;
        signalName = "harmonic";
    }

    SignalGenerator signalGen(sigType, amplitude);

    int steps = getValidInt("\nВведите количество шагов N (1..10000) [например, 50]: ", 1, 10000);

    std::unique_ptr<Model> model;

    if (modelChoice == 1) {
        std::cout << "\n--- Параметры LinearModel13 ---\n";
        double a1 = getValidDouble("Введите a1 [например, 0.8]: ");
        double a2 = getValidDouble("Введите a2 [например, -0.2]: ");
        double b  = getValidDouble("Введите b [например, 0.5]: ");
        model = std::make_unique<LinearModel13>(a1, a2, b);
    } 
    else if (modelChoice == 2) {
        std::cout << "\n--- Параметры NonlinearModel25 ---\n";
        double a = getValidDouble("Введите a [например, 0.7]: ");
        double b = getValidDouble("Введите b [например, 0.3]: ");
        model = std::make_unique<NonlinearModel25>(a, b);
    } 
    else {
        std::cout << "\n--- Параметры ODEModel37 ---\n";
        double a  = getValidDouble("Введите a [например, 0.5]: ");
        double b  = getValidDouble("Введите b [например, 1.0]: ");
        double dt = getValidDouble("Введите шаг интегрирования dt [например, 0.1]: ");
        model = std::make_unique<ODEModel37>(a, b, dt);
    }

    std::string modelName = model->name();

    Simulator simulator(std::move(model), steps);
    std::vector<double> results = simulator.run(signalGen);

    simulator.printTable(results, signalGen);
    simulator.exportCSV(results, signalGen, "results.csv");

    if (std::ofstream fout("results.txt"); fout.is_open()) {
        fout << "# " << modelName << "|" << signalName << "\n";
        for (size_t i = 0; i < results.size(); ++i) {
            double uVal = signalGen.get(static_cast<double>(i));
            fout << i << " " << uVal << " " << results[i] << "\n";
        }
    }

    std::cout << "\n[Инфо] Результаты сохранены в results.csv и results.txt\n";

    return 0;
}