
#ifndef DIFFERENTIAL_MODEL_H
#define DIFFERENTIAL_MODEL_H

#include "Input.h"
#include "Model.h"

// Модель 3.1: dy/dt = -a*y, метод Эйлера: y(k+1) = y(k) + dt*(-a*y(k))
class DifferentialModel : public Model {
private:
    double a = 0.5;
    double dt = 0.1;

public:
    void readParameters() override {
        std::cout << "Коэффициенты модели 3.1 (Enter - значение по умолчанию):\n";
        a = readCoef("a", 0.5);
        dt = readCoef("dt", 0.1, 0.0001, 100.0);
    }

    double nextStep(double u, double dt) override {
        double dydt = -a * y;

        y = y + dt * dydt;

        return y;
    }

    void reset() override {
        y = 1.0;
    }

    double timeStep() const override {
        return dt;
    }

    std::string name() const override {
        return "Дифференциальная модель 3.1";
    }

    std::string code() const override {
        return "3.1";
    }
};

#endif
