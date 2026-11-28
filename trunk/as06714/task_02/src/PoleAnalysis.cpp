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

        PoleReport assemble(Plant plant, std::string polynomial, std::vector<double> poles) {
            PoleReport report;
            report.plant = plant;
            report.modelTag = plantTag(plant);
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
        return assemble(Plant::Linear1_7, "z^2 * (z - " + fmt(a) + ")", { 0.0, 0.0, a });
    }

    PoleReport inspectModel2_9(double a) {
        return assemble(Plant::Sqrt2_9, "z - " + fmt(a), { a });
    }

    PoleReport inspectModel3_1(double a, double dt) {
        const double pole = 1.0 - a * dt;
        return assemble(Plant::Euler3_1, "z - " + fmt(pole) + "  (1 - a*dt)", { pole });
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
        const std::streamsize oldPrecision = sink.precision(6);
        sink << "!!! Stability alarm for " << plantTag(report.plant) << ": denominator ";
        writeDenominator(report, sink);
        sink << ", spectral radius " << report.spectralRadius
             << " -> " << verdictText(report.verdict) << '\n';
        sink.precision(oldPrecision);
        return true;
    }

    const char* plantTag(Plant plant) {
        switch (plant) {
        case Plant::Linear1_7:
            return "Linear_1_7";
        case Plant::Sqrt2_9:
            return "Nonlinear_2_9";
        default:
            return "Euler_3_1";
        }
    }

    void writeDenominator(const PoleReport& report, std::ostream& sink) {
        const double pole = report.poles.empty() ? 0.0 : report.poles.back();
        const std::streamsize oldPrecision = sink.precision(6);
        if (report.plant == Plant::Linear1_7) {
            sink << "z^2 * (z - " << pole << ")";
        } else {
            sink << "z - " << pole;
            if (report.plant == Plant::Euler3_1) {
                sink << "  (1 - a*dt)";
            }
        }
        sink.precision(oldPrecision);
    }

    void enforceConvergence(const PoleReport& report) {
        if (!converges(report)) {
            throw DivergenceError(report.modelTag + " does not converge, spectral radius " +
                                  fmt(report.spectralRadius));
        }
    }

} // namespace stability
