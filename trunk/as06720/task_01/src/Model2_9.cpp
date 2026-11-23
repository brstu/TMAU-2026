#include "Model2_9.h"
#include <cmath>
#include <sstream>

Model2_9::Model2_9(double coeffA, double coeffB)
    : a(coeffA), b(coeffB) {}

double Model2_9::sign(double value) {
    if (value > 0.0) return 1.0;
    if (value < 0.0) return -1.0;
    return 0.0;
}

double Model2_9::calculateNext(double input) {
    const double y_curr = output();

    // Нелинейное воздействие: корень из модуля входа со знаком входа
    const double y_next = a * y_curr + b * std::sqrt(std::fabs(input)) * sign(input);

    storeOutput(y_next);
    return y_next;
}

void Model2_9::reset() {
    storeOutput(0.0);
}

bool Model2_9::isStable(std::string& message) const {
    std::ostringstream out;
    out.precision(4);
    out << std::fixed << "Homogeneous part y(t+1) = a*y(t): |a| = " << std::fabs(a)
        << " (must be < 1)";
    message = out.str();
    return std::fabs(a) < 1.0;
}
