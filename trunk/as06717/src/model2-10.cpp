#include "model2-10.h"
#include <cmath>

using namespace std;

TanhModel::TanhModel(double _alpha, double _beta)
    : Model(_alpha, "Tanh Smoothing Model"),
      gain_k(_beta) {}

double TanhModel::calculateStep(double u_now, double u_prev1, double u_prev2) {
    (void)u_prev1;
    (void)u_prev2;
    double cubed = u_now * u_now * u_now;
    double nextY = alpha * tanh(prevY) + gain_k * cubed;
    prevY = nextY;
    return nextY;
}

void TanhModel::reset() {
    prevY = 0.0;
}

void TanhModel::setInitial(double y0) {
    prevY = y0;
}