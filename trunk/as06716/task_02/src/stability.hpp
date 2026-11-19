#pragma once

#include <complex>
#include <iosfwd>
#include <vector>


namespace stability {

using Complex = std::complex<double>;

struct Report {
    bool stable = false;
    double spectralRadius = 0.0;
    std::vector<Complex> roots;
};

std::vector<Complex> characteristicRoots(double a1, double a2, double a3);

bool isStable(double a1, double a2, double a3);

Report analyze(double a1, double a2, double a3);

bool warnIfUnstable(double a1, double a2, double a3, std::ostream& out);

}  // namespace stability
