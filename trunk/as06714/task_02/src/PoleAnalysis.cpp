#include "PoleAnalysis.h"

#include <cmath>
#include <limits>
#include <ostream>
#include <sstream>

namespace stability {

    namespace {

        std::string fmt(double value) {
            std::ostringstream text;
            text.precision(6);
            text << value;
            return text.str();
        }

        PoleReport assemble(std::string tag, std::string polynomial, std::vector<double> poles) {
            PoleReport report;
            report.modelTag = std::move(tag);
            report.polynomial = std::move(polynomial);
            report.poles = std::move(poles);
            report.spectralRadius = spectralRadius(report.poles);
            report.verdict = judge(report.spectralRadius);
            return report;
        }

    } // namespace

    double spectralRadius(const std::vector<double>& poles) {
        double radius = 0.0;
        for (double pole : poles) {
            if (!std::isfinite(pole)) {
                return std::numeric_limits<double>::infinity();
            }
            radius = std::fmax(radius, std::fabs(pole));
        }
        return radius;
    }

    Verdict judge(double radius) {
        if (std::isnan(radius)) {
            return Verdict::Diverging;
        }
        const double gap = radius - 1.0;
        if (gap < -kUnitCircleEps) {
            return Verdict::Converging;
        }
        return (gap > kUnitCircleEps) ? Verdict::Diverging : Verdict::Marginal;
    }

    PoleReport inspectModel1_7(double a) {
        // Двукратный нулевой полюс появляется из-за задержек входа на один и два такта;
        // он лежит внутри круга, поэтому сходимость определяет только полюс a
        return assemble("Linear_1_7", "z^2 * (z - " + fmt(a) + ")", { 0.0, 0.0, a });
    }

    PoleReport inspectModel2_9(double a) {
        return assemble("Nonlinear_2_9", "z - " + fmt(a), { a });
    }

    PoleReport inspectModel3_1(double a, double dt) {
        const double pole = 1.0 - a * dt;
        return assemble("Euler_3_1", "z - " + fmt(pole) + "  (1 - a*dt)", { pole });
    }

    bool converges(const PoleReport& report) {
        return report.verdict == Verdict::Converging;
    }

    std::string verdictText(Verdict verdict) {
        switch (verdict) {
        case Verdict::Converging:
            return "converging";
        case Verdict::Marginal:
            return "marginal (pole on the unit circle)";
        default:
            return "DIVERGING";
        }
    }

    bool printAlarm(const PoleReport& report, std::ostream& sink) {
        if (converges(report)) {
            return false;
        }
        sink << "!!! Stability alarm for " << report.modelTag
             << ": denominator " << report.polynomial
             << ", spectral radius " << fmt(report.spectralRadius)
             << " -> " << verdictText(report.verdict) << '\n';
        return true;
    }

    void enforceConvergence(const PoleReport& report) {
        if (!converges(report)) {
            throw DivergenceError(report.modelTag + " does not converge, spectral radius " +
                                  fmt(report.spectralRadius));
        }
    }

} // namespace stability
