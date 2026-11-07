#pragma once

#include "IModel.h"

// Model 1.7 (System with Multi-Step Control History):
//   y(tau+1) = a*y(tau) + b1*u(tau) + b2*u(tau-1) + b3*u(tau-2)
class Model17 final : public IModel {
public:
    Model17(double a, double b1, double b2, double b3, double initialY = 0.0);

    double step(double u) override;
    void reset() override;

    std::string name() const override;
    bool isLinear() const override;
    std::vector<double> characteristicPolynomial() const override;

    double output() const { return y_; }

private:
    double a_;
    double b1_;
    double b2_;
    double b3_;
    double initialY_;

    double y_ = 0.0;
    double uPrev1_ = 0.0;
    double uPrev2_ = 0.0;
};
