
#ifndef NONLINEAR_MODEL_H
#define NONLINEAR_MODEL_H

#include "Input.h"
#include "Model.h"

// Нелинейная модель 2.7 с релейным элементом с гистерезисом
// (формула приведена в отчёте)
class NonlinearModel : public Model {
private:
    double a = 0.7;
    double b = 0.5;
    double epsilon = 0.2;

    // Реле с гистерезисом: внутри зоны [-epsilon; epsilon] выход реле зависит от знака y
    double relay(double u) const {
        if (u > epsilon || (u >= -epsilon && getOutput() > 0.0)) {
            return 1.0;
        }
        return -1.0;
    }

public:
    void readParameters() override {
        std::cout << "Коэффициенты модели 2.7 (Enter - значение по умолчанию):\n";
        a = readCoef("a", 0.7);
        b = readCoef("b", 0.5);
        epsilon = readCoef("epsilon", 0.2, 0.0);
    }

    double nextStep(double u, double) override {
        double yNext = a * getOutput() + b * relay(u);

        setOutput(yNext);
        return yNext;
    }

    std::string name() const override {
        return "Нелинейная модель 2.7";
    }

    std::string code() const override {
        return "2.7";
    }
};

#endif
