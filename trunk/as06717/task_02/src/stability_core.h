#pragma once
#include <complex>
#include <ostream>

enum class Stability
{
    Stable,
    Boundary,
    Unstable
};

// Корни z^2 - a1*z - a2 = 0 для ArModel
struct Roots
{
    std::complex<double> z1;
    std::complex<double> z2;
};

Roots findRoots(double a1, double a2);

// Устойчиво, если оба корня строго внутри единичного круга
Stability checkStability(double a1, double a2);

bool isStable(double a1, double a2);

bool warnIfUnstable(double a1, double a2, std::ostream& out);