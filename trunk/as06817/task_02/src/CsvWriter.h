#pragma once

#include "Simulator.h"

#include <stdexcept>
#include <string>

// Исключение при работе с CSV-файлами.
class CsvWriterException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class CsvWriter {
public:
    static std::string toCsv(const SimulationResult& result);
    static void write(const std::string& fileName,
                      const SimulationResult& result);
};
