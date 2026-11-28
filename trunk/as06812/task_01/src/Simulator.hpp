#pragma once
#include "Model.hpp"
#include "InputSignal.hpp"
#include <string>

class Simulator 
{
private:
    int steps;
    
public:
    explicit Simulator(int steps);
    void run(Model& model, const std::string& modelName, const InputSignal& signal) const;
};