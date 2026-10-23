#pragma once
#include <vector>
#include "model.h"

class PureLinearDecayModel : public Model {
private:
    double a;
    double h;

public:
    PureLinearDecayModel(double a, double h = 1.0);

    std::vector<double> calculate(
        const std::vector<double>& u
    ) override;
};