#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "InputSignal.h"
#include "Model.h"

// Один отсчёт симуляции
struct SimulationPoint {
    int step;
    double u;
    double y;
};

// Сбрасывает модель и рассчитывает steps шагов под действием сигнала input.
// Работает только через интерфейсы Model и InputSignal, поэтому
// в тестах вместо них подставляются mock-объекты (Google Mock).
std::vector<SimulationPoint> runSimulation(Model& model, const InputSignal& input, int steps);

#endif // SIMULATOR_H
