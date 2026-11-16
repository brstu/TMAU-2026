#ifndef TASK02_AS06818_SIMULATOR_HPP
#define TASK02_AS06818_SIMULATOR_HPP

#include <vector>

#include "input_signal.hpp"
#include "model.hpp"

// Одна точка траектории симуляции.
struct SimulationPoint {
    int tau;
    double u;
    double y;
};

// Прогон модели: сброс состояния, подача входного воздействия заданного
// типа на n шагов и сбор траектории (tau, u_tau, y_tau).
std::vector<SimulationPoint> simulate(Model& model, SignalType type, int n);

#endif
