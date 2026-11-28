#include "Simulator.h"

std::vector<SimulationPoint> runSimulation(Model& model, const InputSignal& input, int steps) {
    std::vector<SimulationPoint> result;
    if (steps <= 0) {
        return result;
    }

    model.reset();
    result.reserve(static_cast<size_t>(steps));
    for (int t = 0; t < steps; ++t) {
        const double u = input.valueAt(t);
        const double y = model.calculateNext(u);
        result.push_back({ t, u, y });
    }
    return result;
}
