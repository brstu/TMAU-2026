#include "StabilityAnalyzer.h"
#include <cmath>

Stability checkStability(double a)
{
    const double r = std::fabs(a);   //модуль корня z = a

    if (r < 1.0)
        return Stability::Stable;
    if (r == 1.0)
        return Stability::Boundary;
    return Stability::Unstable;      //|a| > 1, а также NaN и бесконечность
}

bool isStable(double a)
{
    return checkStability(a) == Stability::Stable;
}

bool warnIfUnstable(double a, std::ostream& out)
{
    const Stability s = checkStability(a);

    if (s == Stability::Stable)
        return false;

    out << "ПРЕДУПРЕЖДЕНИЕ: корень z = " << a << ", |z| = " << std::fabs(a) << ". ";

    if (s == Stability::Boundary)
        out << "Система на границе устойчивости, процесс не затухает.\n";
    else
        out << "Система неустойчива, процесс будет расходиться.\n";

    return true;
}
