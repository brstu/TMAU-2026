#include "Model1_9.h"

Model1_9::Model1_9(double a1_val, double a2_val, double a3_val, double b1_val) 
    : a1(a1_val), a2(a2_val), a3(a3_val), b1(b1_val) {}

double Model1_9::calculateNext(double u) {
    double current_y = getY();
    double y_next = a1 * current_y + a2 * y_prev1 + a3 * y_prev2 + b1 * u;

    // Разделяем присваивания на отдельные строки по требованию анализатора
    y_prev2 = y_prev1;
    y_prev1 = current_y;
    setY(y_next);

    return y_next;
}

void Model1_9::reset() {
    setY(0.0);
    y_prev1 = 0.0;
    y_prev2 = 0.0;
}