#include "Model1_9.h"

Model1_9::Model1_9(double a1, double a2, double a3, double b1) 
    : a1(a1), a2(a2), a3(a3), b1(b1) {}

double Model1_9::calculateNext(double u) {
    double y_next = a1 * y_curr + a2 * y_prev1 + a3 * y_prev2 + b1 * u;
    y_prev2 = y_prev1;
    y_prev1 = y_curr;
    y_curr = y_next;
    return y_curr;
}

void Model1_9::reset() {
    y_curr = y_prev1 = y_prev2 = 0.0;
}
