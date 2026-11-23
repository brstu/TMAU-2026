#pragma once
#include "Model.h"

class DifferentialModel35 final : public Model {
public:
    explicit DifferentialModel35(double b);
protected:
    double calculateNext(double u, double dt) override;
private:
    double b_;
};
