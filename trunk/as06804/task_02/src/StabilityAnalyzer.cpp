#include "StabilityAnalyzer.hpp"

#include <cmath>
#include <sstream>

namespace stability {

AnalysisResult analyzeLinearModel14(double a, double) {
    AnalysisResult result;

    result.root = a;
    result.radius = std::fabs(a);
    result.stable = result.radius < 1.0; // |z| = 1 is not asymptotic stability

    std::ostringstream out;
    out << "Model 1.4: z - (" << a << ") = 0, root z = " << a
        << ", |z|max = " << result.radius;
    if (result.stable) {
        out << " -> STABLE";
    } else if (result.radius <= 1.0) {
        out << " -> MARGINAL (|z| = 1): NOT asymptotically stable";
    } else {
        out << " -> UNSTABLE (divergent)";
    }
    result.message = out.str();

    return result;
}

bool isStable(double a) {
    return std::fabs(a) < 1.0;
}

} // namespace stability
