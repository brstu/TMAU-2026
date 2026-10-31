
#ifndef LINEAR_MODEL_H
#define LINEAR_MODEL_H

#include "Input.h"
#include "Model.h"

// Линейная модель 1.3: выход зависит от двух предыдущих значений выхода
// и текущего входа (формула приведена в отчёте)
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

    double nextStep(double u, double) override {
        double current = getOutput();
        double yNext = a1 * current + a2 * yPrevious + b * u;

        yPrevious = current;
        setOutput(yNext);

        return yNext;
    }

    void reset() override {
        setOutput(0.0);
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
