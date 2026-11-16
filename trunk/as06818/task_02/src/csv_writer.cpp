#include "csv_writer.hpp"

#include <fstream>

bool write_csv(const std::string& filename,
               const std::vector<SimulationPoint>& points) {
    if (filename.empty()) {
        return false;
    }

    std::ofstream file(filename.c_str());
    if (!file) {
        return false;
    }

    file << "tau,u,y\n";
    for (const SimulationPoint& point : points) {
        file << point.tau << ',' << point.u << ',' << point.y << '\n';
    }

    return static_cast<bool>(file);
}
