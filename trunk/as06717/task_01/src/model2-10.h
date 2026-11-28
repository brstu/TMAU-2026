#pragma once
#include "model.h"

class TanhModel : public Model {
private:
    double gain_k;
    double prevY;

public:
    TanhModel(double _alpha, double _beta);
    double calculateStep(double u_now, double u_prev1, double u_prev2) override;
    void reset() override;
    void setInitial(double y0) override;
};