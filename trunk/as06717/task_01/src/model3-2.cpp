#include "model3-2.h"

using namespace std;

IntegratorModel::IntegratorModel(double _beta)
    : Model(0.0, "Integrator Model"),
      step_gain(_beta) {}

double IntegratorModel::calculateStep(double u_now, double u_prev1, double u_prev2) {
    (void)u_prev1;
    (void)u_prev2;
    double nextY = currentY + step_gain * u_now;
    currentY = nextY;
    return nextY;
}

void IntegratorModel::reset() {
    currentY = 0.0;
}

void IntegratorModel::setInitial(double y0) {
    currentY = y0;
}