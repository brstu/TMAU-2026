#include "LinearModel.h"

LinearModel::LinearModel(double a, double b1, double b2, double b3)
    : a(a), b1(b1), b2(b2), b3(b3) {}

std::vector<double> LinearModel::calculate(const std::vector<double>& u) {
    std::vector<double> y;
    y.reserve(u.size() + 1);
    y.push_back(0.0);

    for (size_t j = 0; j < u.size(); ++j) {
        double u0 = u[j];
        double u1 = (j >= 1) ? u[j - 1] : 0.0;
        double u2 = (j >= 2) ? u[j - 2] : 0.0;
        y.push_back(a * y[j] + b1 * u0 + b2 * u1 + b3 * u2);
    }
    return y;
}