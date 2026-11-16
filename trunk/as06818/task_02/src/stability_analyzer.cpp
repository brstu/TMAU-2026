#include "stability_analyzer.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

StabilityReport analyze_stability(double a1, double a2) {
    // Дискриминант D = a1^2 + 4*a2 может быть отрицательным,
    // поэтому корни ищем в комплексной арифметике.
    const std::complex<double> discriminant(a1 * a1 + 4.0 * a2, 0.0);
    const std::complex<double> root_part = std::sqrt(discriminant);
    const std::complex<double> z1 = (a1 + root_part) / 2.0;
    const std::complex<double> z2 = (a1 - root_part) / 2.0;

    const double radius_z1 = std::abs(z1);
    const double radius_z2 = std::abs(z2);
    const double radius = radius_z1 > radius_z2 ? radius_z1 : radius_z2;

    StabilityReport report;
    report.z1 = z1;
    report.z2 = z2;
    report.spectral_radius = radius;
    report.stable = radius_z1 < 1.0 && radius_z2 < 1.0;
    return report;
}

StabilityReport analyze_stability(const LinearModel18& model) {
    return analyze_stability(model.get_a1(), model.get_a2());
}

std::string stability_warning(const StabilityReport& report) {
    if (report.stable) {
        return std::string();
    }

    std::ostringstream result;
    result << std::fixed << std::setprecision(6)
           << "ПРЕДУПРЕЖДЕНИЕ: процесс расходится (неустойчив): "
              "спектральный радиус характеристического уравнения "
              "z^2 - a1*z - a2 = 0 равен "
           << report.spectral_radius << " >= 1.";
    return result.str();
}
