#include "Simulator.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <windows.h>

void Simulator::run(
    Model& model,
    SignalType signalType,
    int n,
    double initialY,
    const std::string& fileName
) {
    std::ofstream file(fileName);

    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла: " << fileName << std::endl;
        return;
    }

    file << "tau,u,y\n";

    double y = initialY;
    double yPrev = initialY;

    double uPrev = InputSignal::getValue(signalType, 0);
    double uPrev2 = uPrev;

    std::cout << "\n";
    std::cout << std::setw(5) << "tau"
        << std::setw(15) << "u"
        << std::setw(15) << "y" << std::endl;

    std::cout << "-----------------------------------\n";

    for (int tau = 0; tau < n; ++tau) {
        double u = InputSignal::getValue(signalType, tau);

        double nextY = model.calculateNext(
            y,
            yPrev,
            u,
            uPrev,
            uPrev2
        );

        std::cout << std::setw(5) << tau
            << std::setw(15) << std::fixed << std::setprecision(6) << u
            << std::setw(15) << nextY
            << std::endl;

        file << tau << ","
            << u << ","
            << nextY << "\n";

        yPrev = y;
        y = nextY;

        uPrev2 = uPrev;
        uPrev = u;
    }

    file.close();

    std::cout << "\nРезультаты сохранены в: " << fileName << std::endl;
}