#pragma once
#include <complex>
#include <cmath>

class StabilityAnalyzer {
public:
    static bool isStableLinear13(double a1, double a2) {
        std::complex<double> disc(a1 * a1 + 4.0 * a2, 0.0);
        std::complex<double> sqrt_disc = std::sqrt(disc);
        std::complex<double> z1 = (a1 + sqrt_disc) / 2.0;
        std::complex<double> z2 = (a1 - sqrt_disc) / 2.0;
        
        return std::abs(z1) < 1.0 && std::abs(z2) < 1.0;
    }
};