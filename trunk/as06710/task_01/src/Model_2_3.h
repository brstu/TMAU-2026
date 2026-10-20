#pragma once
#include "Model.h"

class Model2_3 final : public Model
{
private:
    double a;
    double b;
    double delta;

    double deadzone(double u) const;

public:
    Model2_3(double a, double b, double delta);

    double calculateNext(
        double y,
        double u,
        double dt
    ) const override;

    const char* getName() const override;
};