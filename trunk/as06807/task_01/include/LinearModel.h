#pragma once
#include <vector>
#include "model.h"

class LinearModel : public Model {
private:
    double a, b1, b2, b3;

public:
    LinearModel(double a, double b1, double b2, double b3);
    std::vector<double> calculate(const std::vector<double>& u) override;
};