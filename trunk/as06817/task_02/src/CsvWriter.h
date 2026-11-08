#pragma once

#include "Simulator.h"

#include <stdexcept>
#include <string>

// Исключение при работе с CSV-файлами.
class CsvWriterException : public std::runtime_error {
public:
    explicit CsvWriterException(const std::string& message)
        : std::runtime_error(message) {
    }
};

class CsvWriter {
public:
    static std::string toCsv(const SimulationResult& result);
    static void write(const std::string& fileName,
                      const SimulationResult& result);
};
