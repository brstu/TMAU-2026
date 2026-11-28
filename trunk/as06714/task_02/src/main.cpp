// ЛР №2, вариант 7. Перед каждым экспериментом модель проходит анализ полюсов,
// и если процесс не сходится, пользователь видит предупреждение до начала расчёта.

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "Excitation.h"
#include "Model1_7.h"
#include "Model2_9.h"
#include "Model3_1.h"
#include "PoleAnalysis.h"
#include "Runner.h"

namespace {

    double askNumber(const std::string& label, double fallback) {
        std::cout << "  " << label << " <" << fallback << "> = ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            return fallback;
        }
        std::istringstream reader(line);
        double value = fallback;
        reader >> value;
        return reader.fail() ? fallback : value;
    }

    bool askYes(const std::string& question) {
        std::cout << question << " [y/N]: ";
        std::string line;
        std::getline(std::cin, line);
        return !line.empty() && (line[0] == 'y' || line[0] == 'Y');
    }

    using Experiment = std::pair<std::unique_ptr<Model>, stability::PoleReport>;

    Experiment prepareLinear() {
        const double a = askNumber("a", 0.6);
        const double b1 = askNumber("b1", 0.5);
        const double b2 = askNumber("b2", 0.3);
        const double b3 = askNumber("b3", 0.2);
        return { std::make_unique<Model1_7>(a, b1, b2, b3), stability::inspectModel1_7(a) };
    }

    Experiment prepareSqrt() {
        const double a = askNumber("a", 0.7);
        const double b = askNumber("b", 1.0);
        return { std::make_unique<Model2_9>(a, b), stability::inspectModel2_9(a) };
    }

    Experiment prepareDecay() {
        const double a = askNumber("a", 1.0);
        const double dt = askNumber("dt", 0.1);
        const double y0 = askNumber("y0", 1.0);
        return { std::make_unique<Model3_1>(a, dt, y0), stability::inspectModel3_1(a, dt) };
    }

    void showPoles(const stability::PoleReport& report) {
        std::cout << "\n  Transfer function denominator: "
                  << stability::sanitizeForLog(report.polynomial) << "\n  Poles:";
        for (double pole : report.poles) {
            std::cout << ' ' << pole;
        }
        std::cout << "\n  Spectral radius = " << report.spectralRadius
                  << ", verdict: " << stability::verdictText(report.verdict) << '\n';
    }

    InputType pickExcitation() {
        std::cout << "\n  Input: 1 - step, 2 - impulse, 3 - harmonic\n";
        const auto code = static_cast<int>(askNumber("input", 1.0));
        if (code == 2) {
            return InputType::IMPULSE;
        }
        return (code == 3) ? InputType::HARMONIC : InputType::STEP;
    }

    void runExperiment(Experiment& experiment) {
        const InputType kind = pickExcitation();
        const double gain = askNumber("amplitude", 1.0);
        const LabExcitation source(kind, gain);
        const auto horizon = static_cast<int>(askNumber("number of steps", 25.0));
        const Trace trace = simulateSteps(*experiment.first, source, horizon);

        std::ofstream table("as06714_trace.csv");
        table << "tau;u;y\n";
        std::cout << "\n  " << stability::sanitizeForLog(experiment.second.modelTag)
                  << " / " << stability::sanitizeForLog(source.title()) << '\n';
        for (size_t tau = 0; tau < trace.output.size(); ++tau) {
            std::cout << std::setw(6) << tau << std::setw(12) << std::setprecision(5)
                      << trace.input[tau] << std::setw(14) << trace.output[tau] << '\n';
            table << tau << ';' << trace.input[tau] << ';' << trace.output[tau] << '\n';
        }
        std::cout << "  Trace written to as06714_trace.csv\n";
    }

} // namespace

int main() {
    std::cout << "Variant 7: Model 1.7 (input history), Model 2.9 (square root), "
                 "Model 3.1 (linear decay)\n";

    while (true) {
        std::cout << "\nPlant: 1 - Model 1.7, 2 - Model 2.9, 3 - Model 3.1, 0 - quit\n";
        const auto choice = static_cast<int>(askNumber("plant", 0.0));
        if (choice < 1 || choice > 3) {
            break;
        }

        Experiment experiment;
        if (choice == 1) {
            experiment = prepareLinear();
        } else if (choice == 2) {
            experiment = prepareSqrt();
        } else {
            experiment = prepareDecay();
        }

        showPoles(experiment.second);
        if (stability::printAlarm(experiment.second, std::cout) &&
            !askYes("  The process will not settle. Simulate anyway?")) {
            continue;
        }
        runExperiment(experiment);
    }
    return 0;
}
