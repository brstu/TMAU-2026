#pragma once

#include <string>


class Model {
public:
    virtual ~Model() = default;

    virtual double step(double u) = 0;

    virtual double output() const = 0;

    virtual std::string name() const = 0;
};


// model 1.6
class LinearModel16 : public Model {
public:
    LinearModel16(double a1, double a2, double a3, double b);

    double step(double u) override;
    double output() const override;
    std::string name() const override;

private:
    double a1_;
    double a2_;
    double a3_;
    double b_;
    double y0_ = 0.0;   // y[t]
    double y1_ = 0.0;   // y[t-1]
    double y2_ = 0.0;   // y[t-2]
};


// model 2.10
class NonlinearModel210 : public Model {
public:
    NonlinearModel210(double a, double b);

    double step(double u) override;
    double output() const override;
    std::string name() const override;

private:
    double a_;
    double b_;
    double y_ = 0.0;
};


// model 3.4
class DiffModel34 : public Model {
public:
    DiffModel34(double a, double dt, double y0);

    double step(double u) override;
    double output() const override;
    std::string name() const override;

private:
    double a_;
    double dt_;
    double y_;
};


double diffModel34Exact(double a, double y0, double t);
