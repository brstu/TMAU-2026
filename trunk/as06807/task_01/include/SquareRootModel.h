#pragma once
#include <vector>
#include "model.h"

class SquareRootModel : public Model {
private:
    double a;
    double b;

public:
    SquareRootModel(double a, double b);

    std::vector<double> calculate(
        const std::vector<double>& u
    ) override;
};
