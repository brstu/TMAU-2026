#ifndef DISCRETE_INTEGRATOR_H
#define DISCRETE_INTEGRATOR_H

#include "Model.h"

// Модель 1.4 - дискретный интегратор с потерями:
// реагирует на изменение входа между соседними шагами (формула в отчёте)
class DiscreteIntegrator : public Model {
private:
    double a;
    double b;
    double uPrev = 0.0;   // вход на предыдущем шаге

public:
    DiscreteIntegrator(double aCoef, double bCoef) : a(aCoef), b(bCoef) {}

    double step(double u) override {
        double next = a * output() + b * (u - uPrev);
        uPrev = u;
        setOutput(next);
        return next;
    }

    void reset() override {
        setOutput(0.0);
        uPrev = 0.0;
    }

    std::string name() const override {
        return "Model 1.4 (linear)";
    }
};

#endif
