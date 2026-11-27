#include "StabilityAnalyzer.hpp"

#include <cmath>
#include <sstream>

namespace stability {

namespace {
constexpr double kEps = 1e-9;
}

AnalysisResult analyzeLinearModel14(double a, double) {
    AnalysisResult result;

    result.root = a;
    result.radius = std::fabs(a);
    result.stable = result.radius < 1.0 - kEps; // |z| = 1 is not asymptotic stability

    std::ostringstream out;
    out << "Model 1.4: z - (" << a << ") = 0, root z = " << a
        << ", |z|max = " << result.radius;
    if (result.stable) {
        out << " -> STABLE";
    } else if (result.radius <= 1.0 + kEps) {
        out << " -> MARGINAL (|z| = 1): NOT asymptotically stable";
    } else {
        out << " -> UNSTABLE (divergent)";
    }
    result.message = out.str();

    return result;
}

bool isStable(double a, double eps) {
    return std::fabs(a) < 1.0 - eps;
}

} // namespace stability
