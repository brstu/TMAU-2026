#include "Model1_5.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <sstream>
#include <vector>

Model1_5::Model1_5(double coeffA1, double coeffA2, double coeffB, int delayK)
    : a1(coeffA1),
      a2(coeffA2),
      b(coeffB),
      k(delayK < 1 ? 1 : delayK),
      history(static_cast<size_t>(k) + 1, 0.0) {}

double Model1_5::calculateNext(double input) {
    const double y_curr = history.front();
    const double y_delay = history.back();

    const double y_next = a1 * y_curr + a2 * y_delay + b * input;

    // Новое значение помещается в начало буфера, самое старое удаляется
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

    using Complex = std::complex<double>;

    // Значение полинома в точке по схеме Горнера.
    // Коэффициенты перечислены от старшей степени к младшей.
    Complex evaluatePoly(const std::vector<double>& coeffs, Complex z) {
        Complex result = 0.0;
        for (double c : coeffs) {
            result = result * z + c;
        }
        return result;
    }

    // Произведение разностей между i-м корнем и всеми остальными
    Complex rootsDifference(const std::vector<Complex>& roots, size_t i) {
        Complex denom = 1.0;
        for (size_t j = 0; j < roots.size(); ++j) {
            if (j != i) {
                denom *= (roots[i] - roots[j]);
            }
        }
        return denom;
    }

    // Одна итерация метода Дюрана-Кернера, возвращает наибольший сдвиг корня
    double durandKernerStep(const std::vector<double>& coeffs, std::vector<Complex>& roots) {
        double maxShift = 0.0;
        for (size_t i = 0; i < roots.size(); ++i) {
            const Complex shift = evaluatePoly(coeffs, roots[i]) / rootsDifference(roots, i);
            roots[i] -= shift;
            maxShift = std::max(maxShift, std::abs(shift));
        }
        return maxShift;
    }

    // Поиск всех корней полинома со старшим коэффициентом, равным единице
    std::vector<Complex> polyRoots(const std::vector<double>& coeffs) {
        const size_t n = coeffs.size() - 1;
        std::vector<Complex> roots(n);
        const Complex seed(0.4, 0.9);
        for (size_t i = 0; i < n; ++i) {
            roots[i] = std::pow(seed, static_cast<int>(i));
        }

        const int maxIterations = 1000;
        const double tolerance = 1e-12;
        for (int iter = 0; iter < maxIterations; ++iter) {
            if (durandKernerStep(coeffs, roots) < tolerance) {
                break;
            }
        }
        return roots;
    }

} // namespace

bool Model1_5::isStable(std::string& message) const {
    // Коэффициенты характеристического уравнения степени k+1
    std::vector coeffs(static_cast<size_t>(k) + 2, 0.0);
    coeffs[0] = 1.0;
    coeffs[1] = -a1;
    coeffs[k + 1] -= a2;

    double maxModulus = 0.0;
    for (const auto& root : polyRoots(coeffs)) {
        maxModulus = std::max(maxModulus, std::abs(root));
    }

    std::ostringstream out;
    out.precision(4);
    out << std::fixed << "Characteristic equation: z^" << (k + 1)
        << " - (" << a1 << ")*z^" << k << " - (" << a2 << ") = 0; "
        << "max |z| = " << maxModulus;
    message = out.str();

    return maxModulus < 1.0;
}
