#include "Simulation.h"
#include "impact.h"
#include <utility>

Simulation::Simulation(int numberOfSteps, InputType type,
                       std::string modelName, std::unique_ptr<Model> model)
    : numberOfSteps(numberOfSteps), type(type),
      modelName(std::move(modelName)), model(std::move(model)) {}

SimulationResult Simulation::run() const
{
    SimulationResult r;
    r.modelName = modelName;
    r.type = type;
    r.u = getInput(numberOfSteps, type);
    r.y = model->calculate(r.u);
    return r;
}