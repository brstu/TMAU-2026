#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "models.h"
#include "stability_analyzer.h"

int main() {
    double lin_a = 1.2; // Неустойчивый параметр для демонстрации проверки
    double lin_b = 0.2;

    std::cout << "Запуск программы лабораторной работы №2..." << std::endl;

    // Автоматическая проверка устойчивости перед началом симуляции
    StabilityAnalyzer::checkAndWarnStability(lin_a);

    LinearModel1_4 model(lin_a, lin_b);
    std::vector input = {1.0, 1.0, 1.0, 1.0, 1.0};
    auto output = model.simulate(input);

    std::cout << "Симуляция выполнена успешно. Шагов: " << output.size() << std::endl;
    return 0;
}