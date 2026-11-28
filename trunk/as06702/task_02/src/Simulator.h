#pragma once

#include "InputSignal.h"
#include "Model.h"

#include <string>
#include <vector>

struct ResultRow {
    std::string model;
    std::string input;
    int tau;
    double u;
    double y;
};

class Simulator {
public:
    static std::vector<ResultRow> run(Model& model, const InputSignal& signal,
                                      int n, double dt = 0.1);
};
