#include "PureLinearDecayModel.h"

PureLinearDecayModel::PureLinearDecayModel(double a, double h)
    : a(a), h(h) {}

std::vector<double> PureLinearDecayModel::calculate(
    const std::vector<double>& u
) {
    std::vector<double> y;
    y.reserve(u.size() + 1);

    // Начальное значение выхода
    y.push_back(1.0);

    for (size_t j = 0; j < u.size(); ++j) {
        double currentY = y[j];

        // Метод Эйлера для dy/dt = -a*y
        double nextY = currentY - h * a * currentY;

        y.push_back(nextY);
    }

    return y;
}