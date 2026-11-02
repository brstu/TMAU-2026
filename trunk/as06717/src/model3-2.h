#pragma once
#include "model.h"

class IntegratorModel : public Model {
private:
    double step_gain;
    double currentY;

public:
    explicit IntegratorModel(double _beta);
    double calculateStep(double u_now, double u_prev1, double u_prev2) override;
    void reset() override;
    void setInitial(double y0) override;
};