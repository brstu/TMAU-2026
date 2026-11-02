#include "simulator.h"

#include <cmath>
#include <iostream>

std::vector<Sample> Simulator::run(const Model& model,
                                   const InputSignal& signal,
                                   std::size_t steps,
                                   double initialY,
                                   double dt) const {
    std::vector u(steps + 1, 0.0);
    std::vector y(steps + 1, 0.0);
    std::vector<Sample> samples;
    samples.reserve(steps + 1);

    for (std::size_t tau = 0; tau <= steps; ++tau) {
        u[tau] = signal.value(tau);
    }
    y[0] = initialY;
    samples.push_back({0, u[0], y[0]});

    for (std::size_t tau = 0; tau < steps; ++tau) {
        const double nextY = model.nextStep(tau, y, u, dt);
        if (!std::isfinite(nextY)) {
            std::cout << "\nПРЕДУПРЕЖДЕНИЕ: расчет остановлен на шаге " << tau + 1
                      << ", так как значение y стало бесконечным или неопределенным.\n";
            break;
        }
        y[tau + 1] = nextY;
        samples.push_back({tau + 1, u[tau + 1], y[tau + 1]});
    }

    return samples;
}
