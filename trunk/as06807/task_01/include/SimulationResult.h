#pragma once

#include <vector>
#include <cstddef>
#include "InputType.h"

struct ImpactResult
{
    InputType type;
    std::vector<double> values;
};

struct ModelResult
{
    std::size_t ModelIndex{};
    std::vector<ImpactResult> calculateResult;
};