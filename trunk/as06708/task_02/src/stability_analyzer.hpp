#pragma once
#include <complex>
#include <utility>

// y[k] = a1*y[k-1] + a2*y[k-2] + ...
// Характеристический полином z^2 - a1*z - a2.
class StabilityAnalyzer {
public:
    using Roots = std::pair<std::complex<double>, std::complex<double>>;
    static Roots roots(double a1, double a2);
    // True только при строгом |z_i| < 1 (асимптотическая устойчивость).
    static bool is_asymptotically_stable(double a1, double a2);
};
