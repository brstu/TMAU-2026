#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

#include <iosfwd>
#include <string>
#include <vector>

class Model;
class Signal;

struct SimulationRecord {
    std::vector<double> time;
    std::vector<double> input;
    std::vector<double> output;
    bool diverged = false;
};

// Advances the model for `steps` samples using the supplied input signal.
// A human readable table is written to `table`.
SimulationRecord runSimulation(Model& model, const Signal& signal,
                               int steps, double dt, std::ostream& table);

// Saves the record as Time,Input,Output CSV. Returns false if the file cannot be opened.
bool writeCsv(const std::string& path, const SimulationRecord& record);

#endif // SIMULATION_RUNNER_H
