#include "SquareRootModel.h"
#include <cmath>

SquareRootModel::SquareRootModel(double a, double b) : a(a), b(b) {}

std::vector<double> SquareRootModel::calculate(const std::vector<double>& u)
{
    std::vector<double> y(u.size());
    for (std::size_t j = 0; j < u.size(); ++j)
    {
        double yPrev = (j >= 1) ? y[j - 1] : 0.0;
        double sign = (u[j] > 0) ? 1.0 : (u[j] < 0) ? -1.0 : 0.0;
        y[j] = a * yPrev + b * std::sqrt(std::abs(u[j])) * sign;
    }
    return y;
}