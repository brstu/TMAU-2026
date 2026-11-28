#include "StabilityAnalyzer.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace stability {

    namespace {

        void normalizeCoefficients(const std::vector<double>& input, std::vector<double>& p, int& zerosAtOrigin, int& n) {
            std::vector<double> coeffs = input;
            size_t start = 0;
            while (start < coeffs.size() && std::abs(coeffs[start]) < 1e-15) {
                ++start;
            }

            zerosAtOrigin = 0;
            while (coeffs.size() - start > 1 && std::abs(coeffs.back()) < 1e-15) {
                coeffs.pop_back();
                ++zerosAtOrigin;
            }

            n = (start >= coeffs.size()) ? 0 : static_cast<int>(coeffs.size() - start) - 1;
            p.clear();
            if (n > 0) {
                const double lead = coeffs[start];
                for (int i = 0; i <= n; ++i) {
                    p.push_back(coeffs[start + i] / lead);
                }
            }
        }

        std::complex<double> evaluatePolynomial(const std::vector<double>& p, std::complex<double> z) {
            std::complex<double> r = p[0];
            for (size_t i = 1; i < p.size(); ++i) {
                r = r * z + p[i];
            }
            return r;
        }

        double performIteration(std::vector<std::complex<double>>& dk, const std::vector<double>& p) {
            double maxDelta = 0.0;
            const size_t n = dk.size();
            for (size_t i = 0; i < n; ++i) {
                std::complex<double> num = evaluatePolynomial(p, dk[i]);
                std::complex<double> den(1.0, 0.0);

                for (size_t j = 0; j < n; ++j) {
                    if (i != j) {
                        den *= (dk[i] - dk[j]);
                    }
                }

                if (std::abs(den) > 1e-30) {
                    std::complex<double> delta = num / den;
                    dk[i] -= delta;
                    maxDelta = std::max(maxDelta, std::abs(delta));
                }
            }
            return maxDelta;
        }

    }
    std::vector<std::complex<double>> findRoots(const std::vector<double>& c, int maxIter, double tol) {
        if (c.empty()) return {};

        std::vector<double> p;
        int zerosAtOrigin = 0;
        int n = 0;
        normalizeCoefficients(c, p, zerosAtOrigin, n);

        std::vector<std::complex<double>> roots;

        if (n > 0) {
            std::vector<std::complex<double>> dk(n);
            std::complex<double> seed(0.4, 0.9);
            std::complex<double> acc(1.0, 0.0);
            for (int i = 0; i < n; ++i) {
                dk[i] = acc;
                acc *= seed;
            }

            for (int it = 0; it < maxIter; ++it) {
                if (performIteration(dk, p) < tol) {
                    break;
                }
            }
            roots = dk;
        }

        for (int i = 0; i < zerosAtOrigin; ++i) {
            roots.emplace_back(0.0, 0.0);
        }
        return roots;
    }

    double spectralRadius(const std::vector<double>& c) {
        auto roots = findRoots(c);
        double r = 0.0;
        for (const auto& z : roots) {
            r = std::max(r, std::abs(z));
        }
        return r;
    }

    AnalysisResult analyzeModel16(double a1, double a2, double a3) {
        AnalysisResult res;
        const std::vector coeffs = { 1.0, -a1, -a2, -a3 };

        res.roots = findRoots(coeffs);
        for (const auto& z : res.roots) {
            res.radius = std::max(res.radius, std::abs(z));
        }

        const double eps = 1e-6;
        res.stable = res.radius < 1.0 - eps;

        std::ostringstream os;
        os << "Model 1.6 spectral radius |z|max = " << res.radius;
        if (res.stable) {
            os << " -> STABLE";
        }
        else if (std::abs(res.radius - 1.0) <= eps) {
            os << " -> MARGINAL (unit circle)";
        }
        else {
            os << " -> UNSTABLE (divergent)";
        }
        res.message = os.str();
        return res;
    }

}