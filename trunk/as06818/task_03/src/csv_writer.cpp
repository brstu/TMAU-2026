#include "csv_writer.hpp"

#include <fstream>

namespace {

void write_point(std::ofstream& file, int tau, double u, double y) {
    file << tau << ',' << u << ',' << y << '\n';
}

}  // namespace

bool write_csv(const std::string& filename,
               const std::vector<ControllerPoint>& points) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "tau,w,e,u,y\n";
    for (const ControllerPoint& point : points) {
        file << point.tau << ',' << point.w << ',' << point.e << ','
             << point.u << ',' << point.y << '\n';
    }
    return true;
}

bool write_csv(const std::string& filename,
               const std::vector<SimulationPoint>& points) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "tau,u,y\n";
    for (const SimulationPoint& point : points) {
        write_point(file, point.tau, point.u, point.y);
    }
    return true;
}
