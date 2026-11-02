#ifndef INPUT_DRIVE_MODEL_H
#define INPUT_DRIVE_MODEL_H

#include "Model.h"

// Модель 3.2 - скорость изменения выхода пропорциональна входу,
// решается явным методом Эйлера (формула в отчёте)
class InputDriveModel : public Model {
private:
    double b;
    double dt;

public:
    InputDriveModel(double bCoef, double timeStep) : b(bCoef), dt(timeStep) {}

    double step(double u) override {
        double next = output() + dt * b * u;
        setOutput(next);
        return next;
    }

    void reset() override {
        setOutput(0.0);
    }

    std::string name() const override {
        return "Model 3.2 (differential)";
    }
};

#endif
