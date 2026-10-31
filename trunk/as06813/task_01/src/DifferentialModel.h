
#ifndef DIFFERENTIAL_MODEL_H
#define DIFFERENTIAL_MODEL_H

#include "Input.h"
#include "Model.h"

// Дифференциальная модель 3.1 (линейное затухание),
// решается явным методом Эйлера (формула приведена в отчёте)
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

    // Вход u в уравнение не входит, шаг step берётся из timeStep()
    double nextStep(double, double step) override {
        double current = getOutput();
        double dydt = -a * current;
        double yNext = current + step * dydt;

        setOutput(yNext);
        return yNext;
    }

    void reset() override {
        setOutput(1.0);
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
