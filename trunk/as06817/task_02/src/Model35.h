#pragma once

#include "IModel.h"

// Model 3.5 (Harmonic Driving Force), уравнение решается методом Эйлера:
//   dy/dt = b*sin(u)
//   y(tau+1) = y(tau) + dt*b*sin(u(tau))
class Model35 final : public IModel {
public:
    Model35(double b, double dt, double initialY = 0.0);

    double step(double u) override;
    void reset() override;

    std::string name() const override;
    bool isLinear() const override;
    std::vector<double> characteristicPolynomial() const override;

    double output() const { return y_; }

private:
    double b_;
    double dt_;
    double initialY_;
    double y_ = 0.0;
};
