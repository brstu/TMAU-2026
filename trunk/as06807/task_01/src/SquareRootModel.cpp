#include "SquareRootModel.h"
#include <cmath>

SquareRootModel::SquareRootModel(double a, double b)
    : a(a), b(b) {}

std::vector<double> SquareRootModel::calculate(
    const std::vector<double>& u
) {
    std::vector<double> y;
    y.reserve(u.size() + 1);

    // Начальное значение выхода
    y.push_back(0.0);

    for (size_t j = 0; j < u.size(); ++j) {
        double u0 = u[j];

        // sign(u): 1, если u > 0;
        //          0, если u = 0;
        //         -1, если u < 0.
        double signU = (u0 > 0.0) ? 1.0
                     : (u0 < 0.0) ? -1.0
                     : 0.0;

        double nextY =
            a * y[j]
            + b * std::sqrt(std::abs(u0)) * signU;

        y.push_back(nextY);
    }

    return y;
}