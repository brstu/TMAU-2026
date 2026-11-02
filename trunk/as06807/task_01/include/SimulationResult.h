#pragma once
#include <string>
#include <vector>
#include "InputType.h"

struct SimulationResult
{
    std::string modelName;
    InputType type;
    std::vector<double> u;
    std::vector<double> y;
};