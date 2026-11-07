#include "CsvWriter.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

std::string CsvWriter::toCsv(const SimulationResult& result) {
    std::ostringstream out;
    out << "tau,u,y\n";
    out << std::setprecision(10);

    for (const auto& step : result.steps) {
        out << step.tau << ','
            << step.u << ','
            << step.y << '\n';
    }

    return out.str();
}

void CsvWriter::write(const std::string& fileName,
                      const SimulationResult& result) {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + fileName);
    }

    file << toCsv(result);
}
