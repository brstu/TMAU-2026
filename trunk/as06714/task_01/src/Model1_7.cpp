#include "Model1_7.h"
#include <cmath>
#include <sstream>

Model1_7::Model1_7(double coeffA, double coeffB1, double coeffB2, double coeffB3)
    : a(coeffA), b1(coeffB1), b2(coeffB2), b3(coeffB3) {}

double Model1_7::calculateNext(double input) {
    const double y_curr = getY();

    const double y_next = a * y_curr + b1 * input + b2 * u_prev1 + b3 * u_prev2;

    // Сдвиг истории управляющих воздействий
    u_prev2 = u_prev1;
    u_prev1 = input;

    setY(y_next);
    return y_next;
}

void Model1_7::reset() {
    setY(0.0);
    u_prev1 = 0.0;
    u_prev2 = 0.0;
}

bool Model1_7::isStable(std::string& message) const {
    std::ostringstream out;
    out.precision(4);
    out << std::fixed << "Characteristic equation: z - (" << a << ") = 0; |z| = "
        << std::fabs(a) << " (must be < 1)";
    message = out.str();
    return std::fabs(a) < 1.0;
}
