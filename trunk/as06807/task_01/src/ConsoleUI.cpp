#include "ConsoleUI.h"
#include "Simulation.h"

#include "LinearModel.h"
#include "SquareRootModel.h"
#include "PureLinearDecayModel.h"

#include "results_csv.h"

#include <iostream>
#include <memory>
#include <cstdlib>

void ConsoleUI::run()
{
    int numberOfSteps;

    std::cout << "Input number of steps: ";
    std::cin >> numberOfSteps;

    if (!std::cin || numberOfSteps <= 0) {
        std::cout << "Invalid number of steps!\n";
        return;
    }

    // Создаём объект моделирования.
    Simulation sim(numberOfSteps);

    // Модель 1: LinearModel.
    sim.addModel(
        std::make_unique<LinearModel>(
            0.5, 1.0, 0.3, 0.1
        )
    );

    // Модель 2.9: Square Root Modulated Action.
    sim.addModel(
        std::make_unique<SquareRootModel>(
            0.5, 1.0
        )
    );

    // Модель 3.1: Pure Linear Decay.
    sim.addModel(
        std::make_unique<PureLinearDecayModel>(
            0.2
        )
    );

    // Запускаем моделирование.
    auto results = sim.run();

    // Сохраняем результаты в CSV.
    saveResultsToCsv(results, "results.csv");

    std::cout << "Results saved to results.csv\n";

    // После сохранения результатов:
    saveResultsToCsv(results, "results.csv");

    int result = std::system("python plot.py");

    if (result != 0) {
        std::cout << "Не удалось запустить plot.py\n";
    }
}