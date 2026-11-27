#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <string>

namespace stability {

struct AnalysisResult {
    bool stable = false;
    double root = 0.0;      // root of the characteristic equation on the Z-plane
    double radius = 0.0;    // |z|
    std::string message;
};

// Analytical stability check for variant 4, model 1.4.
// Free process: y[k+1] = a*y[k]  =>  z - a = 0  =>  z = a.
// The system is asymptotically stable iff |a| < 1.
// `b` is accepted for API completeness: it cancels out in the free process.
AnalysisResult analyzeLinearModel14(double a, double b = 0.0);

// Convenience predicate used by main.cpp and unit tests.
bool isStable(double a, double eps = 1e-9);

} // namespace stability

#endif // STABILITY_ANALYZER_H
