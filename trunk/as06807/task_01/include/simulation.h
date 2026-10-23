#pragma once

#include <vector>
#include <memory>

#include "model.h"
#include "impact.h"
#include "SimulationResult.h"

class Simulation {
private:
    int numberOfSteps;
    std::vector<std::unique_ptr<Model>> models;

public:
    Simulation(
        int numberOfSteps
    );

    void addModel(std::unique_ptr<Model> model);

    std::vector<ModelResult> run() const;
};