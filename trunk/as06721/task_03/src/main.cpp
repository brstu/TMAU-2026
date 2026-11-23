#include "control.hpp"

#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

using namespace lab03;

struct Preset {
    const char* key;
    PlantSettings plant;
    PidSettings pid;
};

const std::array<Preset, 3> presets{{
    {"model12", {PlantKind::delayedLinear, 0.8, 0.2, 2}, {1.2, 0.8, 0.1, -120, 120}},
    {"model27", {PlantKind::relay, 0.75, 15, 0, 0.2}, {0.25, 0.12, 0, -6, 6}},
    {"model35", {PlantKind::harmonic, 0, 12}, {0.09, 0.025, 0.02, -1.3, 1.3}},
}};

template <typename Value>
Value readValue(const char* prompt, Value minimum, Value maximum) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw std::runtime_error("Input ended.");
    }
    std::istringstream input(line);
    input.imbue(std::locale::classic());
    Value value{};
    if (!(input >> value) || !(input >> std::ws).eof() ||
        !std::isfinite(static_cast<double>(value)) || value < minimum || value > maximum) {
        throw std::invalid_argument("Input is outside the permitted range or has an invalid format.");
    }
    return value;
}

void writeCsv(const std::filesystem::path& path, const SimulationResult& result) {
    std::ofstream file(path);
    file.exceptions(std::ios::badbit | std::ios::failbit);
    file.imbue(std::locale::classic());
    file << "step,time,target,output,error,command\n" << std::setprecision(17);
    for (const auto& row : result.samples) {
        file << row.step << ',' << row.time << ',' << row.target << ',' << row.output
             << ',' << row.error << ',' << row.command << '\n';
    }
    file.close();
}

void customize(Preset& preset) {
    if (preset.plant.kind != PlantKind::harmonic) {
        preset.plant.a = readValue("Plant a: ", -1e6, 1e6);
    }
    preset.plant.b = readValue("Plant b: ", -1e6, 1e6);
    if (preset.plant.kind == PlantKind::delayedLinear) {
        preset.plant.delay = readValue("Delay in steps: ", 0, 1000000);
    } else if (preset.plant.kind == PlantKind::relay) {
        preset.plant.threshold = readValue("Relay threshold: ", 0.0, 1e6);
    }

    preset.pid.kp = readValue("Kp: ", 0.0, 1e6);
    preset.pid.ki = readValue("Ki: ", 0.0, 1e6);
    preset.pid.kd = readValue("Kd: ", 0.0, 1e6);
    preset.pid.minimum = readValue("Actuator minimum: ", -1e6, 0.0);
    preset.pid.maximum = readValue("Actuator maximum: ", 0.0, 1e6);
}

void runInteractive() {
    std::cout << "Lab 03 / Variant 21 / Closed-loop temperature control\n";
    auto preset = presets[readValue("Plant [1 = 1.2, 2 = 2.7, 3 = 3.5]: ", 1, 3) - 1];
    if (readValue("Coefficients [1 = preset, 2 = custom]: ", 1, 2) == 2) {
        customize(preset);
    }
    const auto mode = readValue("Mode [1 = fixed steps, 2 = error tolerance]: ", 1, 2) == 1
        ? StopMode::fixedSteps : StopMode::errorTolerance;
    const auto steps = static_cast<std::size_t>(
        readValue(mode == StopMode::fixedSteps ? "Number of steps: " : "Maximum steps: ", 1, 1000000));
    const double dt = readValue("Sample period (seconds): ", 1e-9, 1e6);
    const double initial = readValue("Initial temperature: ", -1e6, 1e6);
    const double target = readValue("Target temperature: ", -1e6, 1e6);
    const double tolerance = mode == StopMode::errorTolerance
        ? readValue("Absolute error tolerance: ", 0.0, 1e6) : 0.0;
    const auto result = simulate(preset.plant, preset.pid, {mode, steps, dt, initial, target, tolerance});
    writeCsv("results.csv", result);
    const auto& final = result.samples.back();
    std::cout << "step,time,target,output,error,command\n"
              << final.step << ',' << final.time << ',' << final.target << ',' << final.output
              << ',' << final.error << ',' << final.command << '\n';
    switch (result.reason) {
        case StopReason::completed: std::cout << "Fixed number of steps completed.\n"; break;
        case StopReason::toleranceReached: std::cout << "Error tolerance reached.\n"; break;
        case StopReason::stepLimit: std::cout << "Step limit reached before the error tolerance.\n"; break;
    }
    std::cout << "Trajectory saved to results.csv\n";
}

void runExamples() {
    const std::filesystem::path directory = "build/examples";
    std::filesystem::create_directories(directory);
    for (const auto& preset : presets) {
        for (const int target : {40, 60}) {
            const auto csv = std::string(preset.key) + "_" + std::to_string(target) + ".csv";
            const auto result = simulate(preset.plant, preset.pid,
                {StopMode::fixedSteps, 200, 0.2, 20.0, static_cast<double>(target)});
            writeCsv(directory / csv, result);
        }
    }

    const auto& reference = presets.front();
    const SimulationSettings tuning{StopMode::fixedSteps, 200, 0.2, 20, 60};
    auto gains = reference.pid;
    gains.ki = 0.0;
    gains.kd = 0.0;
    writeCsv(directory / "tuning_p.csv", simulate(reference.plant, gains, tuning));
    gains.ki = reference.pid.ki;
    writeCsv(directory / "tuning_pi.csv", simulate(reference.plant, gains, tuning));
    writeCsv(directory / "tuning_pid.csv", simulate(reference.plant, reference.pid, tuning));
    std::ofstream summary(directory / "modeb_summary.csv");
    summary.exceptions(std::ios::badbit | std::ios::failbit);
    summary.imbue(std::locale::classic());
    summary << "model,target,tolerance,step,time,output,error,reason\n" << std::setprecision(17);
    for (const auto& preset : presets) {
        for (const int target : {40, 60}) {
            for (const double tolerance : {0.5, 0.2, 0.05}) {
                const auto result = simulate(preset.plant, preset.pid,
                    {StopMode::errorTolerance, 200, 0.2, 20.0,
                     static_cast<double>(target), tolerance});
                const auto& last = result.samples.back();
                summary << preset.key << ',' << target << ',' << tolerance << ','
                        << last.step << ',' << last.time << ',' << last.output << ','
                        << last.error << ',' << (result.reason == StopReason::toleranceReached
                            ? "toleranceReached" : "stepLimit") << '\n';
                if (tolerance == 0.2) {
                    writeCsv(directory / (std::string("modeb_") + preset.key + "_" +
                        std::to_string(target) + ".csv"), result);
                }
            }
        }
    }
    summary.close();
    std::cout << "Example trajectories saved to build/examples\n";
}

} // namespace

/** Run the menu, or generate the report's CSV trajectories with --examples.
 *  @param argc Number of command-line arguments.
 *  @param argv Command-line argument values.
 *  @return Zero on success, one on invalid input or a runtime error.
 */
int main(int argc, char* argv[]) {
    try {
        if (argc == 1) {
            runInteractive();
        } else if (argc == 2 && std::string(argv[1]) == "--examples") {
            runExamples();
        } else {
            throw std::invalid_argument("Usage: task03 [--examples]");
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
