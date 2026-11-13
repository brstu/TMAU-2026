#include "Simulator.h"

#include <stdexcept>

Simulator::Simulator(IModel& model, const ISignalSource& signal, int steps)
    : model_(model),
      signal_(signal),
      steps_(steps) {
    if (steps_ <= 0) {
        throw std::invalid_argument(
            "Количество шагов моделирования должно быть больше нуля.");
    }
}

SimulationResult Simulator::run(std::ostream& log) {
    model_.reset();

    SimulationResult result;
    result.stability = StabilityAnalyzer::analyze(model_);

    // Предупреждение выводится до начала моделирования.
    if (result.stability.analyzed && !result.stability.stable) {
        log << "\n"
            << StabilityAnalyzer::buildWarning(model_, result.stability)
            << "\n\n";
    }

    result.steps.reserve(static_cast<std::size_t>(steps_));
    for (int tau = 0; tau < steps_; ++tau) {
        const double u = signal_.value(tau);
        const double y = model_.step(u);
        result.steps.push_back(SimulationStep{tau, u, y});
    }

    return result;
}
