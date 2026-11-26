#include "Runner.h"

Trace simulateSteps(Model& plant, const Excitation& source, int horizon) {
    Trace trace;
    if (horizon < 1) {
        return trace;
    }

    plant.reset();
    for (int tau = 0; tau < horizon; ++tau) {
        const double u = source.sample(tau);
        trace.input.push_back(u);
        trace.output.push_back(plant.calculateNext(u));
    }
    return trace;
}
