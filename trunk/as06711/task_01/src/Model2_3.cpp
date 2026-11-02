#include "Model2_3.h"
#include <cmath>

Model2_3::Model2_3(double coeffA1, double coeffB1, double coeffC1)
    : a1(coeffA1), b1(coeffB1), c1(coeffC1) {}

double Model2_3::calculateNext(double input) {
    const double y_curr = getY();
    const double prevY = y_prev1;

    // Нелинейное рекуррентное соотношение:
    // y[k] = a1*y[k-1] - b1*(y[k-2])^2 + c1*u[k]
    const double y_next =
        a1 * y_curr -
        b1 * std::pow(prevY, 2) +
        c1 * input;

    // Сдвиг истории выходов
    y_prev1 = y_curr;
    setY(y_next);

    return y_next;
}

void Model2_3::reset() {
    setY(0.0);
    y_prev1 = 0.0;
}