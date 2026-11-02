#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "input_signal.h"
#include "model.h"

#include <cstddef>
#include <vector>

struct Sample {
    std::size_t tau;
    double input;
    double output;
};

class Simulator {
public:
    std::vector<Sample> run(const Model& model,
                            const InputSignal& signal,
                            std::size_t steps,
                            double initialY,
                            double dt) const;
};

#endif
