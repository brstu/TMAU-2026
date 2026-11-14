#pragma once

#include <complex>
#include <string>

// Characteristic polynomial for Model 1.3: z^2 - a1*z - a2 = 0.
enum class StabilityStatus {
    AsymptoticallyStable,
    MarginallyStable,
    Unstable
};

struct StabilityReport {
    std::complex<double> root1;
    std::complex<double> root2;
    StabilityStatus status;

    bool isAsymptoticallyStable() const;
};

class StabilityAnalyzer {
public:
    static StabilityReport analyze(double a1, double a2, double tolerance = 1e-9);
    static std::string statusText(StabilityStatus status);
    static std::string formatReport(const StabilityReport& report);
};
