#pragma once

#include "Model.h"

#include <cmath>
#include <string>
#include <vector>

// y_(τ+1) = a*y_τ + b*(u_τ - u_(τ-1))
class Model14Linear final : public Model {
private:
    double a_;
    double b_;

public:
    Model14Linear(double a, double b) : a_(a), b_(b) {}

    std::string name() const override { return "Model 1.4 — дискретный интегратор с потерями"; }
    std::string formula() const override { return "y(τ+1) = a*y(τ) + b*(u(τ) - u(τ-1))"; }

    double nextStep(int tau,
                    const std::vector<double>& y,
                    const std::vector<double>& u,
                    double) const override {
        const double previousU = (tau > 0) ? u[static_cast<std::size_t>(tau - 1)] : 0.0;
        return a_ * y[static_cast<std::size_t>(tau)] + b_ * (u[static_cast<std::size_t>(tau)] - previousU);
    }

    double coefficientA() const { return a_; }
};

// y_(τ+1) = a*y_τ*(1-y_τ) + b*u_τ + c*sin(y_(τ-1)*u_τ)
class Model28Nonlinear final : public Model {
private:
    double a_;
    double b_;
    double c_;

public:
    Model28Nonlinear(double a, double b, double c) : a_(a), b_(b), c_(c) {}

    std::string name() const override { return "Model 2.8 — логистическая модель с возмущением"; }
    std::string formula() const override { return "y(τ+1) = a*y(τ)*(1-y(τ)) + b*u(τ) + c*sin(y(τ-1)*u(τ))"; }

    double nextStep(int tau,
                    const std::vector<double>& y,
                    const std::vector<double>& u,
                    double) const override {
        const auto index = static_cast<std::size_t>(tau);
        const double previousY = (tau > 0) ? y[index - 1] : y[0];
        return a_ * y[index] * (1.0 - y[index])
             + b_ * u[index]
             + c_ * std::sin(previousY * u[index]);
    }
};

// dy/dt = b*u; Euler: y_(τ+1) = y_τ + Δt*b*u_τ
class Model32Differential final : public Model {
private:
    double b_;

public:
    explicit Model32Differential(double b) : b_(b) {}

    std::string name() const override { return "Model 3.2 — постоянное входное воздействие"; }
    std::string formula() const override { return "dy/dt = b*u; Euler: y(τ+1) = y(τ) + Δt*b*u(τ)"; }

    double nextStep(int tau,
                    const std::vector<double>& y,
                    const std::vector<double>& u,
                    double dt) const override {
        const auto index = static_cast<std::size_t>(tau);
        return y[index] + dt * b_ * u[index];
    }
};
