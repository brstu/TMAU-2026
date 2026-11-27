#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <complex>
#include <cstddef>
#include <string>

// Classification of the homogeneous (unforced) discrete-time system.
enum class StabilityStatus {
    AsymptoticallyStable,
    MarginallyStable,
    Unstable,
    InvalidParameters
};

struct StabilityReport {
    double coefficientA{};
    std::size_t inputDelay{};
    std::complex<double> characteristicRoot{};
    StabilityStatus status{StabilityStatus::InvalidParameters};

    bool isAsymptoticallyStable() const;
    std::string message() const;
};

class StabilityAnalyzer {
public:
    // Model 1.2: y(tau+1) = a*y(tau) + b*u(tau-k).
    // The input delay k does not change the characteristic root of
    // the homogeneous equation: z - a = 0, therefore z = a.
    static StabilityReport analyzeModel12(double a, std::size_t inputDelay);
};

#endif
