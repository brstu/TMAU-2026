#include <cmath>
#include <cstddef>

#include "control_system.hpp"

ControlSystem::ControlSystem(Model& plant, PidController& pid)
    : plant(plant), pid(pid), y(plant.get_y()) {}

ControllerPoint ControlSystem::step(double w) {
    const double e = w - y;
    const double u = pid.compute(e);

    y = plant.step(u);

    return ControllerPoint{tau++, w, e, u, y};
}

void ControlSystem::reset() {
    plant.reset();
    pid.reset();
    y = plant.get_y();
    tau = 0;
}

std::vector<ControllerPoint> run_fixed_steps(ControlSystem& system, double w,
                                             int n) {
    std::vector<ControllerPoint> points;
    points.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) {
        points.push_back(system.step(w));
    }
    return points;
}

std::vector<ControllerPoint> run_until_steady(ControlSystem& system, double w,
                                              double eps, int window,
                                              int max_steps) {
    std::vector<ControllerPoint> points;
    points.reserve(static_cast<std::size_t>(max_steps));

    int below = 0;
    for (int i = 0; i < max_steps; ++i) {
        const ControllerPoint point = system.step(w);
        points.push_back(point);

        below = std::abs(point.e) < eps ? below + 1 : 0;
        if (below >= window) {
            break;
        }
    }
    return points;
}

std::vector<SimulationPoint> run_open_loop(Model& plant, double u, int n) {
    std::vector<SimulationPoint> points;
    points.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) {
        const double y = plant.step(u);
        points.push_back(SimulationPoint{i, u, y});
    }
    return points;
}
