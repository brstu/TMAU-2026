#pragma once

#include <vector>

#include "models.hpp"
#include "signals.hpp"


struct Sample {
    int tau = 0;
    double u = 0.0;
    double y = 0.0;
};

std::vector<Sample> simulate(Model& model, const InputSignal& input, int steps);
