
#ifndef LINEAR_MODEL_H
#define LINEAR_MODEL_H

#include "Input.h"
#include "Model.h"

// Модель 1.3: y(k+1) = a1*y(k) + a2*y(k-1) + b*u(k)
class LinearModel : public Model {
private:
    double a1 = 0.8;
    double a2 = -0.2;
    double b = 0.5;
    double yPrevious = 0.0;

public:
    void readParameters() override {
        std::cout << "Коэффициенты модели 1.3 (Enter - значение по умолчанию):\n";
        a1 = readCoef("a1", 0.8);
        a2 = readCoef("a2", -0.2);
        b = readCoef("b", 0.5);
    }

    double nextStep(double u, double dt) override {
        double yNext = a1 * y + a2 * yPrevious + b * u;

        yPrevious = y;
        y = yNext;

        return y;
    }

    void reset() override {
        y = 0.0;
        yPrevious = 0.0;
    }

    std::string name() const override {
        return "Линейная модель 1.3";
    }

    std::string code() const override {
        return "1.3";
    }
};

#endif
