#include "LinearModel13.h"
#include "NonlinearModel25.h"
#include "ODEModel37.h"
#include "Simulator.h"
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#endif

void runModelSimulation(Model& model, int steps) {
    Simulator sim(std::unique_ptr<Model>(&model), steps); // Временное создание под вывод

    const std::array<SignalType, 3> signals = {
        SignalType::STEP,
        SignalType::IMPULSE,
        SignalType::HARMONIC
    };

    const std::array<std::string, 3> signalNames = {
        "step",
        "impulse",
        "harmonic"
    };

    for (size_t i = 0; i < signals.size(); ++i) {
        SignalGenerator gen(signals[i], 1.0);
        
        // Создаем симулятор заново под каждую генерацию
        Simulator activeSim(std::unique_ptr<Model>(nullptr), steps);
        
        std::cout << "\n-----------------------------------------\n";
        std::cout << " Воздействие: " << signalNames[i] << "\n";
        
        // Для демонстрации используем локальный запуск
        auto results = sim.run(gen);
        sim.printTable(results, gen);

        std::string filename = "plot_" + model.name() + "_" + signalNames[i] + ".csv";
        sim.exportCSV(results, gen, filename);
        
        std::string pyCmd = "python plot_single.py \"" + filename + "\"";
        
        if (int sysRes = std::system(pyCmd.c_str()); sysRes != 0) {
            std::cerr << "Предупреждение: Python скрипт вернул код " << sysRes << "\n";
        }
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    try {
        std::cout << "Выберете модель для запуска (1 - Linear1.3, 2 - NonLinear2.5, 3 - ODE3.7, 0 - Все): ";
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cerr << "Ошибка ввода!\n";
            return 1;
        }

        int steps = 30;

        if (choice == 1 || choice == 0) {
            std::cout << "\n--- Моделирование LinearModel13 ---\n";
            LinearModel13 m1(0.5, 0.1, 1.0);
            runModelSimulation(m1, steps);
        }

        if (choice == 2 || choice == 0) {
            std::cout << "\n--- Моделирование NonlinearModel25 ---\n";
            NonlinearModel25 m2(0.8, 0.5);
            runModelSimulation(m2, steps);
        }

        if (choice == 3 || choice == 0) {
            std::cout << "\n--- Моделирование ODEModel37 ---\n";
            ODEModel37 m3(0.1, 1.0, 0.1);
            runModelSimulation(m3, steps);
        }

    } catch (const std::exception& e) {
        std::cerr << "Произошла ошибка выполнения: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Произошла неизвестная ошибка!\n";
        return 1;
    }

    return 0;
}