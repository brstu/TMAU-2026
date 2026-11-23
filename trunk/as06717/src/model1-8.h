#pragma once
#include "model.h"

class ArModel : public Model {
private:
    double a1;
    double a2;
    double b1;
    double b2;
    double prevY1;
    double prevY2;

public:
    ArModel(double _a1, double _a2, double _b1, double _b2);
    double calculateStep(double u_now, double u_prev1, double u_prev2) override;
    void reset() override;
    void setInitial(double y0) override;
};