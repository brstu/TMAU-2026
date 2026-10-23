
#pragma once

#include "simulation.h"
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

inline std::string inputTypeToString(InputType type)
{
    switch (type)
    {
        case InputType::Step:
            return "Step";

        case InputType::Impulse:
            return "Impulse";

        case InputType::Harmonic:
            return "Harmonic";
    }

    return "Unknown";
}

inline void saveResultsToCsv(
    const std::vector<ModelResult>& results,
    const std::string& filename
)
{
    std::ofstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось создать CSV-файл");
    }

    file << "ModelIndex,InputType,Step,Y\n";

    for (const auto& model : results)
    {
        for (const auto& impact : model.calculateResult)
        {
            for (std::size_t i = 0; i < impact.values.size(); i++)
            {
                file << model.ModelIndex << ","
                     << inputTypeToString(impact.type) << ","
                     << i << ","
                     << impact.values[i] << "\n";
            }
        }
    }
}
