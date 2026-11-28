#pragma once

#include "Model.h"

class LinearModel13 final : public Model {
private:
    double a1_;
    double a2_;
    double b_;
    double previousY_;
    double initialPreviousY_;

public:
    LinearModel13(double a1, double a2, double b,
                  double initialY = 0.0, double initialPreviousY = 0.0);

    double step(double u, double dt) override;
    std::string name() const override;
    void reset() override;

    double a1() const;
    double a2() const;
    double b() const;
    double previousValue() const;
};
