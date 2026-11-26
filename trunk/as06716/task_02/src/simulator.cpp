#include "simulator.hpp"


std::vector<Sample> simulate(Model& model, const InputSignal& input, int steps) {
    std::vector<Sample> samples;
    if (steps > 0) {
        samples.reserve(static_cast<std::size_t>(steps));
    }

    for (int tau = 1; tau <= steps; ++tau) {
        const double u = input.value(tau - 1);
        const double y = model.step(u);
        samples.push_back({tau, u, y});
    }

    return samples;
}
