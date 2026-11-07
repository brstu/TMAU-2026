#pragma once

#include "Simulator.h"

#include <string>

class CsvWriter {
public:
    static std::string toCsv(const SimulationResult& result);
    static void write(const std::string& fileName,
                      const SimulationResult& result);
};
