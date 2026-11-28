#include "Model1_9.h"

Model1_9::Model1_9(double coeffA1, double coeffA2, double coeffA3, double coeffB1)
    : a1(coeffA1), a2(coeffA2), a3(coeffA3), b1(coeffB1) {}

double Model1_9::calculateNext(double input) {
    const double y_curr = getY();

    // Рекуррентное соотношение 2-го порядка
    const double y_next =
        a1 * y_curr +
        a2 * y_prev1 +
        a3 * y_prev2 +
        b1 * input;

    // Сдвиг истории выходов на один шаг назад
    y_prev2 = y_prev1;
    y_prev1 = y_curr;
    setY(y_next);

    return y_next;
}

void Model1_9::reset() {
    setY(0.0);
    y_prev1 = 0.0;
    y_prev2 = 0.0;
}