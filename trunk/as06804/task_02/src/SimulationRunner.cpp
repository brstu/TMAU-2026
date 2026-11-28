#include "SimulationRunner.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <ostream>

#include "Model.h"
#include "InputSignal.h"

namespace {
constexpr double kDivergenceLimit = 1e12;
}

SimulationRecord runSimulation(Model& model, const Signal& signal,
                               int steps, double dt, std::ostream& table) {
    SimulationRecord record;

    table << std::setw(10) << "Step"
          << std::setw(15) << "Time"
          << std::setw(15) << "Input U"
          << std::setw(15) << "Output Y" << '\n';

    for (int i = 0; i < steps; ++i) {
        const double t = i * dt;
        const double u = signal.value(i, t);

        model.step(u);
        const double y = model.getY();

        record.time.push_back(t);
        record.input.push_back(u);
        record.output.push_back(y);

        if (!std::isfinite(y) || std::fabs(y) > kDivergenceLimit) {
            record.diverged = true;
        }

        table << std::setw(10) << i
              << std::setw(15) << std::fixed << std::setprecision(3) << t
              << std::setw(15) << u
              << std::setw(15) << y << '\n';
    }

    return record;
}

bool writeCsv(const std::string& path, const SimulationRecord& record) {
    std::ofstream out(path);
    if (!out.is_open()) {
        return false;
    }

    out << "Time,Input,Output\n";
    out << std::setprecision(10);
    for (std::size_t i = 0; i < record.output.size(); ++i) {
        out << record.time[i] << ',' << record.input[i] << ',' << record.output[i] << '\n';
    }
    return true;
}
