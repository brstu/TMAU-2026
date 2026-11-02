#ifndef LOGISTIC_MODEL_H
#define LOGISTIC_MODEL_H

#include <cmath>
#include "Model.h"

// Модель 2.8 - логистическое отображение с управляющим воздействием
// и синусоидальным возмущением (формула в отчёте)
class LogisticModel : public Model {
private:
    double a;
    double b;
    double c;
    double yPrev = 0.0;   // выход на предыдущем шаге

public:
    LogisticModel(double aCoef, double bCoef, double cCoef) : a(aCoef), b(bCoef), c(cCoef) {}

    double step(double u) override {
        double current = output();
        double next = a * current * (1.0 - current) + b * u + c * std::sin(yPrev * u);
        yPrev = current;
        setOutput(next);
        return next;
    }

    void reset() override {
        setOutput(0.0);
        yPrev = 0.0;
    }

    std::string name() const override {
        return "Model 2.8 (nonlinear)";
    }
};

#endif
