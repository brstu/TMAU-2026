#pragma once
#include "Model.h"

class Model1_1 final : public Model
{
private:
    double a;
    double b;

public:
    Model1_1(double a, double b);

    double calculateNext(
        double y,
        double u,
        double dt
    ) const override;

    const char* getName() const override;
};