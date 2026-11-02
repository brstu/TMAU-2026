#include "model1-8.h"

using namespace std;

ArModel::ArModel(double _a1, double _a2, double _b1, double _b2)
    : Model(_a1, "AR Linear Model"),
      a1(_a1), a2(_a2), b1(_b1), b2(_b2) {}

double ArModel::calculateStep(double u_now, double u_prev1, double u_prev2) {
    (void)u_prev2;
    double nextY = a1 * prevY1 + a2 * prevY2 + b1 * u_now + b2 * u_prev1;
    prevY2 = prevY1;
    prevY1 = nextY;
    return nextY;
}

void ArModel::reset() {
    prevY1 = 0.0;
    prevY2 = 0.0;
}

void ArModel::setInitial(double y0) {
    prevY1 = y0;
    prevY2 = y0;
}