#include "PureLinearDecayModel.h"

PureLinearDecayModel::PureLinearDecayModel(double a, double h, double y0)
    : a(a), h(h), y0(y0) {}

std::vector<double> PureLinearDecayModel::calculate(const std::vector<double>& u)
{
    std::vector<double> y(u.size());
    if (y.empty()) return y;

    y[0] = y0;
    for (std::size_t j = 1; j < u.size(); ++j)
        y[j] = y[j - 1] - h * a * y[j - 1];   // Эйлер: dy/dt = -a*y
    return y;
}