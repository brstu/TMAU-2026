#pragma once

#include <vector>

// Базовый абстрактный класс
class Model {
public:
    virtual ~Model() = default;

    virtual std::vector<double> calculate(
        const std::vector<double>& u,
        int N
    ) const = 0;
};

// Model 1.10
// y[t+1] = a1*y[t] + b1*u[t] + b2*u[t-2]
class Model110 : public Model {
private:
    double a1;
    double b1;
    double b2;

public:
    Model110(double a1, double b1, double b2);

    std::vector<double> calculate(
        const std::vector<double>& u,
        int N
    ) const override;

    double getA1() const;
};

// Model 2.2 — насыщение входного сигнала
class Model22 : public Model {
private:
    double a;
    double b;
    double Umin;
    double Umax;

public:
    Model22(
        double a,
        double b,
        double Umin = -1.0,
        double Umax = 1.0
    );

    std::vector<double> calculate(
        const std::vector<double>& u,
        int N
    ) const override;
};

// Model 3.4
// dy/dt = -a*y^2
class Model34 : public Model {
private:
    double a;
    double dt;

public:
    Model34(double a, double dt);

    std::vector<double> calculate(
        const std::vector<double>& u,
        int N
    ) const override;
};