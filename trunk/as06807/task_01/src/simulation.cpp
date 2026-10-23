#include "Simulation.h"
#include "impact.h"
#include <utility>

Simulation::Simulation(int numberOfSteps)
    : numberOfSteps(numberOfSteps)
{
}

void Simulation::addModel(std::unique_ptr<Model> model)
{
    models.push_back(std::move(model));
}

std::vector<ModelResult> Simulation::run() const
{
    const auto inputs = getInput(numberOfSteps);

    std::vector<ModelResult> results;

    for (std::size_t i = 0; i < models.size(); ++i)
    {
        ModelResult result;
        result.ModelIndex = i;

        for (const auto& input : inputs)
        {
            ImpactResult impact;

            impact.type = input.type;
            impact.values =
                models[i]->calculate(input.values);

            result.calculateResult.push_back(
                std::move(impact)
            );
        }

        results.push_back(std::move(result));
    }

    return results;
}