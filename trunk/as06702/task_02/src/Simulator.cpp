#include "Simulator.h"

#include <stdexcept>

std::vector<ResultRow> Simulator::run(Model& model, const InputSignal& signal,
                                      int n, double dt) {
    if (n <= 0) {
        throw std::invalid_argument("The number of simulation steps must be positive");
    }
    if (dt <= 0.0) {
        throw std::invalid_argument("The integration step dt must be positive");
    }

    std::vector<ResultRow> rows;
    rows.reserve(static_cast<std::size_t>(n));
    model.reset();

    for (int tau = 0; tau < n; ++tau) {
        const double u = signal.valueAt(tau);
        const double y = model.step(u, dt);
        rows.push_back({model.name(), signal.name(), tau + 1, u, y});
    }
    return rows;
}
