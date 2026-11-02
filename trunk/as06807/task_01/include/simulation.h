#pragma once
#include <memory>
#include <string>
#include "model.h"
#include "SimulationResult.h"
#include "InputType.h"

class Simulation {
private:
    int numberOfSteps;
    InputType type;
    std::string modelName;
    std::unique_ptr<Model> model;

public:
    Simulation(int numberOfSteps, InputType type,
               std::string modelName, std::unique_ptr<Model> model);

    SimulationResult run() const;
};