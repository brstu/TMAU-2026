#include "stability_analyzer.hpp"
#include <cmath>
#include <limits>
StabilityAnalyzer::Roots StabilityAnalyzer::roots(double a1, double a2) {
    const std::complex<double> discriminant(a1*a1 + 4.0*a2, 0.0);
    const auto d = std::sqrt(discriminant);
    return { (a1 + d)/2.0, (a1 - d)/2.0 };
}
bool StabilityAnalyzer::is_asymptotically_stable(double a1, double a2) {
    if (!std::isfinite(a1) || !std::isfinite(a2)) return false;
    const auto r = roots(a1, a2);
    return std::abs(r.first) < 1.0 && std::abs(r.second) < 1.0;
}
