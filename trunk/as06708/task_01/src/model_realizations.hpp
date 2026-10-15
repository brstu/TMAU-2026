#include "model.hpp"

class LinearModel : public IModel{
    private:
    double a1;
    double a2;
    double b1;
    double b2;
    public:
    LinearModel(double a1_, double a2_, double b1_, double b2_);
    double next_step(double yCurr, double yPrev, double u, double uPrev) override;
};

class NoneLinearModel : public IModel{
    private:
    double a;
    double b;
    double uMin;
    double uMax;
    public:
    NoneLinearModel(double a_, double b_, double uMin_, double uMax_);
    double next_step(double yCurr, double yPrev, double u, double uPrev) override;

    private:
    const double sat(double u) const;
};

class SimpleDifferentialModel : public IModel{
    private:
    double a;
    double b;
    double dt;
    public:
    SimpleDifferentialModel(double a_, double b_, double dt_);
    double next_step(double yCurr, double yPrev, double u, double uPrev) override;
};