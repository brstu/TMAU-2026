#include "simulator.hpp"

std::vector<SimulationPoint> simulate(Model& model, SignalType type, int n) {
    model.reset();

    std::vector<SimulationPoint> points;
    points.reserve(static_cast<std::size_t>(n));

    for (int tau = 0; tau < n; ++tau) {
        const double u = input_value(type, tau);
        const double y = model.step(u);
        points.push_back(SimulationPoint{tau, u, y});
    }
    return points;
}
