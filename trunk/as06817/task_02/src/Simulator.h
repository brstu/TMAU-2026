#pragma once

#include "IModel.h"
#include "ISignalSource.h"
#include "StabilityAnalyzer.h"

#include <iostream>
#include <vector>

struct SimulationStep {
    int tau = 0;
    double u = 0.0;
    double y = 0.0;
};

struct SimulationResult {
    std::vector<SimulationStep> steps;
    StabilityReport stability;
};

class Simulator {
public:
    Simulator(IModel& model, const ISignalSource& signal, int steps);

    SimulationResult run(std::ostream& log = std::cout);

    const IModel& model() const { return model_; }
    int steps() const { return steps_; }

private:
    IModel& model_;
    const ISignalSource& signal_;
    int steps_;
};
