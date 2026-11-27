#pragma once
#include <complex>
#include <string>
#include <vector>

namespace stability {

    std::vector<std::complex<double>> findRoots(const std::vector<double>& coeffs,
        int maxIter = 5000,
        double tol = 1e-12);

    double spectralRadius(const std::vector<double>& coeffs);

    struct AnalysisResult {
        bool                              stable = true;
        double                            radius = 0.0;
        std::vector<std::complex<double>> roots;
        std::string                       message;
    };

    AnalysisResult analyzeModel16(double a1, double a2, double a3);

}