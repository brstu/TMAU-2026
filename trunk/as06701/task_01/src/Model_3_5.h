#pragma once
#include "Model.h"

class Model3_5 final : public Model
{
private:
    double b;

public:
    explicit Model3_5(double b);

    double calculateNext(
        double y,
        double u,
        double dt
    ) const override;

    const char* getName() const override;
};