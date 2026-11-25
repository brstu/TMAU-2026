#include "stability_core.h"
#include <cmath>

Roots findRoots(double a1, double a2)
{
    const std::complex<double> disc(a1 * a1 + 4.0 * a2, 0.0);
    const std::complex<double> sq = std::sqrt(disc);
    return { (a1 + sq) / 2.0, (a1 - sq) / 2.0 };
}

static Stability verdictFor(double mod)
{
    if (!std::isfinite(mod)) return Stability::Unstable;
    return (mod < 1.0) ? Stability::Stable
         : (mod == 1.0) ? Stability::Boundary
                        : Stability::Unstable;
}

Stability checkStability(double a1, double a2)
{
    const Roots r = findRoots(a1, a2);
    const Stability s1 = verdictFor(std::abs(r.z1));
    const Stability s2 = verdictFor(std::abs(r.z2));

    // берём худший из двух
    if (s1 == Stability::Unstable || s2 == Stability::Unstable)
        return Stability::Unstable;
    if (s1 == Stability::Boundary || s2 == Stability::Boundary)
        return Stability::Boundary;
    return Stability::Stable;
}

bool isStable(double a1, double a2)
{
    return checkStability(a1, a2) == Stability::Stable;
}

bool warnIfUnstable(double a1, double a2, std::ostream& out)
{
    const Stability v = checkStability(a1, a2);
    if (v == Stability::Stable)
        return false;

    const Roots r = findRoots(a1, a2);
    out << "ПРЕДУПРЕЖДЕНИЕ: корни z1 = " << r.z1
        << ", z2 = " << r.z2 << ". ";
    if (v == Stability::Boundary)
        out << "Система на границе устойчивости, процесс не затухает.\n";
    else
        out << "Система неустойчива, процесс будет расходиться.\n";
    return true;
}