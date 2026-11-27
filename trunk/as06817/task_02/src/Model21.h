#pragma once

#include "IModel.h"

// Model 2.1 (Quadratic Feedback and Harmonic Control):
//   y(tau+1) = a*y(tau) - b*y(tau-1)^2 + c*u(tau) + d*sin(u(tau-1))
class Model21 final : public IModel {
public:
    Model21(double a, double b, double c, double d, double initialY = 0.0);

    double step(double u) override;
    void reset() override;

    std::string name() const override;
    bool isLinear() const override;
    std::vector<double> characteristicPolynomial() const override;

    double output() const { return y_; }

private:
    double a_;
    double b_;
    double c_;
    double d_;
    double initialY_;

    double y_ = 0.0;
    double yPrev_ = 0.0;
    double uPrev_ = 0.0;
};
