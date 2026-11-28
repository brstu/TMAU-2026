#include "models.h"

#include <algorithm>
#include <stdexcept>

// ---------------- Model 1.10 ----------------

Model110::Model110(double a1, double b1, double b2)
    : a1(a1), b1(b1), b2(b2) {
}

std::vector<double> Model110::calculate(
    const std::vector<double>& u,
    int N
) const {
    if (N < 0) {
        throw std::invalid_argument("N must be non-negative");
    }

    std::vector y(N + 1, 0.0);

    for (int t = 0; t < N; t++) {
        double currentInput =
            (t < static_cast<int>(u.size())) ? u[t] : 0.0;

        double delayedInput =
            (t >= 2 && t - 2 < static_cast<int>(u.size()))
            ? u[t - 2] : 0.0;

        y[t + 1] =
            a1 * y[t] +
            b1 * currentInput +
            b2 * delayedInput;
    }

    return y;
}

double Model110::getA1() const {
    return a1;
}

// ---------------- Model 2.2 ----------------

Model22::Model22(
    double a,
    double b,
    double Umin,
    double Umax
) : a(a), b(b), Umin(Umin), Umax(Umax) {

    if (Umin > Umax) {
        throw std::invalid_argument("Umin must be <= Umax");
    }
}

std::vector<double> Model22::calculate(
    const std::vector<double>& u,
    int N
) const {
    if (N < 0) {
        throw std::invalid_argument("N must be non-negative");
    }

    std::vector y(N + 1, 0.0);

    for (int t = 0; t < N; t++) {
        double input =
            (t < static_cast<int>(u.size())) ? u[t] : 0.0;

        // Ограничение входного воздействия
        double saturatedInput =
            std::max(Umin, std::min(Umax, input));

        y[t + 1] = a * y[t] + b * saturatedInput;
    }

    return y;
}

// ---------------- Model 3.4 ----------------

Model34::Model34(double a, double dt)
    : a(a), dt(dt) {

    if (dt <= 0.0) {
        throw std::invalid_argument("dt must be positive");
    }
}

std::vector<double> Model34::calculate(
    const std::vector<double>& u,
    int N
) const {
    if (N < 0) {
        throw std::invalid_argument("N must be non-negative");
    }

    std::vector y(N + 1, 0.0);

    // Начальное условие из ЛР №1
    y[0] = 1.0;

    // Метод Эйлера
    for (int t = 0; t < N; t++) {
        y[t + 1] =
            y[t] - dt * a * y[t] * y[t];
    }

    return y;
}