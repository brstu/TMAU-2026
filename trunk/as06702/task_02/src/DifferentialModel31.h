#pragma once

#include "Model.h"

class DifferentialModel31 final : public Model {
private:
    double a_;

public:
    explicit DifferentialModel31(double a, double initialY = 1.0);

    double step(double u, double dt) override;
    std::string name() const override;
    double a() const;
};
