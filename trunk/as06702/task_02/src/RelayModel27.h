#pragma once

#include "Model.h"

class RelayModel27 final : public Model {
private:
    double a_;
    double b_;
    double epsilon_;

    double relay(double u) const;

public:
    RelayModel27(double a, double b, double epsilon, double initialY = 0.0);

    double step(double u, double dt) override;
    std::string name() const override;
    double epsilon() const;
};
