#include "Model1_5.h"
#include <cmath>
#include <complex>
#include <sstream>
#include <vector>

Model1_5::Model1_5(double coeffA1, double coeffA2, double coeffB, int delayK)
    : a1(coeffA1), a2(coeffA2), b(coeffB), k(delayK < 1 ? 1 : delayK) {
    reset();
}

double Model1_5::calculateNext(double input) {
    const double y_curr = history.front();   // y(t)
    const double y_delay = history.back();   // y(t-k)

    const double y_next = a1 * y_curr + a2 * y_delay + b * input;

    // Сдвигаем историю: новое значение в начало, самое старое удаляем
    history.push_front(y_next);
    history.pop_back();

    setY(y_next);
    return y_next;
}

void Model1_5::reset() {
    setY(0.0);
    history.assign(static_cast<size_t>(k) + 1, 0.0);
}

namespace {

    // Поиск всех корней полинома методом Дюрана–Кернера.
    // coeffs — коэффициенты при z^n, z^(n-1), ..., z^0 (старший = 1)
    std::vector<std::complex<double>> polyRoots(const std::vector<double>& coeffs) {
        const int n = static_cast<int>(coeffs.size()) - 1;
        std::vector<std::complex<double>> roots(n);
        const std::complex<double> seed(0.4, 0.9);
        for (int i = 0; i < n; ++i) roots[i] = std::pow(seed, i);

        auto evaluate = [&](std::complex<double> z) {
            std::complex<double> result = 0.0;
            for (double c : coeffs) result = result * z + c;
            return result;
        };

        for (int iter = 0; iter < 1000; ++iter) {
            double maxShift = 0.0;
            for (int i = 0; i < n; ++i) {
                std::complex<double> denom = 1.0;
                for (int j = 0; j < n; ++j)
                    if (j != i) denom *= (roots[i] - roots[j]);
                const std::complex<double> shift = evaluate(roots[i]) / denom;
                roots[i] -= shift;
                maxShift = std::max(maxShift, std::abs(shift));
            }
            if (maxShift < 1e-12) break;
        }
        return roots;
    }

} // namespace

bool Model1_5::isStable(std::string& message) const {
    // Характеристическое уравнение: z^(k+1) - a1*z^k - a2 = 0
    std::vector<double> coeffs(static_cast<size_t>(k) + 2, 0.0);
    coeffs[0] = 1.0;
    coeffs[1] = -a1;
    coeffs[k + 1] -= a2;

    const auto roots = polyRoots(coeffs);
    double maxModulus = 0.0;
    for (const auto& r : roots) maxModulus = std::max(maxModulus, std::abs(r));

    std::ostringstream out;
    out.precision(4);
    out << std::fixed << "Characteristic equation: z^" << (k + 1)
        << " - (" << a1 << ")*z^" << k << " - (" << a2 << ") = 0; "
        << "max |z| = " << maxModulus;
    message = out.str();

    return maxModulus < 1.0;
}
