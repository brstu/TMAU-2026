#pragma once
#include "Model.h"

class NonlinearModel21 final : public Model {
public:
    NonlinearModel21(double a, double b, double c, double d);
protected:
    double calculateNext(double u, double dt) override;
private:
    double a_, b_, c_, d_;
};
