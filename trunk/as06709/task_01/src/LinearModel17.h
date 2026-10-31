#pragma once
#include "Model.h"

class LinearModel17 final : public Model {
public:
    LinearModel17(double a, double b1, double b2, double b3);
protected:
    double calculateNext(double u, double dt) override;
private:
    double a_, b1_, b2_, b3_;
};
