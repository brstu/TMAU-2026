#include "models.h"
#include "stability.h"

#include <iostream>
#include <vector>

int main() {
    double a1 = 0.5;
    double b1 = 1.0;
    double b2 = 0.5;
    int N = 10;

    Model110 model(a1, b1, b2);

    // Проверяем устойчивость до начала моделирования.
    checkModelBeforeSimulation(model);

    // Ступенчатый входной сигнал.
    std::vector u(N, 1.0);

    auto y = model.calculate(u, N);

    std::cout << "\nSimulation results:\n";

    for (std::size_t i = 0; i < y.size(); ++i) {
        std::cout << "y[" << i << "] = " << y[i] << '\n';
    }

    return 0;
}
