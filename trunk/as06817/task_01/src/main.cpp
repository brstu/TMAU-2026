#include <iostream>
#include <memory>
#include <windows.h>

#include "Model17.h"
#include "Model21.h"
#include "Model35.h"
#include "Simulator.h"

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int n;

    std::cout << "Лабораторная работа 1\n";
    std::cout << "ТиМAУ, вариант 17\n";

    std::cout << "Введите количество шагов n: ";
    std::cin >> n;

    if (n <= 0) {
        std::cout << "Количество шагов должно быть больше 0.\n";
        return 1;
    }

    int signalChoice;

    std::cout << "\nВыберите входное воздействие:\n";
    std::cout << "1 - ступенчатое\n";
    std::cout << "2 - импульсное\n";
    std::cout << "3 - гармоническое\n";
    std::cout << "Ваш выбор: ";
    std::cin >> signalChoice;

    if (signalChoice < 1 || signalChoice > 3) {
        std::cout << "Неверный выбор.\n";
        return 1;
    }

    SignalType signalType =
        static_cast<SignalType>(signalChoice);

    Model17 model17(0.7, 0.5, 0.2, 0.1);
    Model21 model21(0.5, 0.05, 0.8, 0.2);
    Model35 model35(1.0, 0.1);

    std::cout << "\nMODEL 1.7\n";

    Simulator::run(
        model17,
        signalType,
        n,
        0.0,
        "model17.csv"
    );

    std::cout << "\nMODEL 2.1\n";

    Simulator::run(
        model21,
        signalType,
        n,
        0.0,
        "model21.csv"
    );

    std::cout << "\nMODEL 3.5\n";

    Simulator::run(
        model35,
        signalType,
        n,
        0.0,
        "model35.csv"
    );

    std::cout << "\nМоделирование завершено.\n";

    return 0;
}