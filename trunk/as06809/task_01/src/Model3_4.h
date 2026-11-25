#pragma once
#include "Model.h"

class Model3_4 : public Model {
private:
    double a;  // Отдельная строка
    double dt; // Отдельная строка
public:
    Model3_4(double p_a, double p_dt);
    double calculateNext(double u) override;
    void reset() override;
};