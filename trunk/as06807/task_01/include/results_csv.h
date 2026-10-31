#pragma once
#include "SimulationResult.h"
#include <fstream>
#include <stdexcept>
#include <string>

class CsvFileError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

inline std::string inputTypeToString(InputType type)
{
    switch (type)
    {
    case InputType::Step:     return "Step";
    case InputType::Impulse:  return "Impulse";
    case InputType::Harmonic: return "Harmonic";
    }
    return "Unknown";
}

inline void saveResultToCsv(const SimulationResult& r, const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        throw CsvFileError("Cannot create CSV file");

    file << "Model,InputType,Step,U,Y\n";
    for (std::size_t i = 0; i < r.y.size(); ++i)
        file << r.modelName << "," << inputTypeToString(r.type) << ","
             << i << "," << r.u[i] << "," << r.y[i] << "\n";
}
