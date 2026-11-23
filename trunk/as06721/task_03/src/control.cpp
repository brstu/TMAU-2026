#include "control.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace lab03 {
namespace {

constexpr std::size_t maxSteps = 1000000;

class Plant {
public:
    Plant(const PlantSettings& settings, double initial)
        : settings_(settings), output_(initial) {
        if (!std::isfinite(settings.b) ||
            (settings.kind != PlantKind::harmonic && !std::isfinite(settings.a))) {
            throw std::invalid_argument("Plant coefficients must be finite.");
        }
        switch (settings.kind) {
            case PlantKind::delayedLinear:
                if (settings.delay > maxSteps) {
                    throw std::invalid_argument("Input delay is too large.");
                }
                pending_.resize(settings.delay, 0.0);
                break;
            case PlantKind::relay:
                if (!std::isfinite(settings.threshold) || settings.threshold < 0.0) {
                    throw std::invalid_argument("Relay threshold must be finite and nonnegative.");
                }
                break;
            case PlantKind::harmonic:
                break;
            default:
                throw std::invalid_argument("Unknown plant.");
        }
    }

    double advance(double command, double dt) {
        switch (settings_.kind) {
            case PlantKind::delayedLinear: {
                double delayed = command;
                if (!pending_.empty()) {
                    delayed = pending_[cursor_];
                    pending_[cursor_] = command;
                    cursor_ = (cursor_ + 1) % pending_.size();
                }
                output_ = settings_.a * output_ + settings_.b * delayed;
                break;
            }
            case PlantKind::relay: {
                const double threshold = settings_.threshold;
                const bool positive = command > threshold ||
                    (command >= -threshold && output_ > 0.0);
                output_ = settings_.a * output_ + settings_.b * (positive ? 1.0 : -1.0);
                break;
            }
            case PlantKind::harmonic:
                output_ += dt * settings_.b * std::sin(command);
                break;
        }
        if (!std::isfinite(output_)) {
            throw std::overflow_error("Plant output is not finite.");
        }
        return output_;
    }

private:
    PlantSettings settings_;
    double output_;
    std::vector<double> pending_;
    std::size_t cursor_ = 0;
};

double trackingError(double target, double output) {
    const double error = target - output;
    if (!std::isfinite(error)) {
        throw std::overflow_error("Tracking error is not finite.");
    }
    return error;
}

} // namespace

Pid::Pid(PidSettings settings, double dt) : settings_(settings), dt_(dt) {
    if (!std::isfinite(dt) || dt <= 0.0 ||
        !std::isfinite(settings.kp) || !std::isfinite(settings.ki) ||
        !std::isfinite(settings.kd) || settings.kp < 0.0 ||
        settings.ki < 0.0 || settings.kd < 0.0 ||
        !std::isfinite(settings.minimum) || !std::isfinite(settings.maximum) ||
        settings.minimum > 0.0 || settings.maximum < 0.0 ||
        settings.minimum >= settings.maximum) {
        throw std::invalid_argument("PID needs nonnegative gains, a positive period and ordered limits containing zero.");
    }
}

double Pid::advance(double error) {
    if (!std::isfinite(error)) {
        throw std::invalid_argument("PID error must be finite.");
    }
    const double difference = error - previous_;
    const double change = settings_.kp * difference + settings_.ki * error * dt_ +
        settings_.kd / dt_ * (difference - (previous_ - older_));
    const double raw = command_ + change;
    if (!std::isfinite(raw)) {
        throw std::overflow_error("PID command is not finite.");
    }
    command_ = std::clamp(raw, settings_.minimum, settings_.maximum);
    older_ = previous_;
    previous_ = error;
    return command_;
}

SimulationResult simulate(const PlantSettings& plantSettings, const PidSettings& pidSettings,
                          const SimulationSettings& settings) {
    const bool byError = settings.mode == StopMode::errorTolerance;
    if ((!byError && settings.mode != StopMode::fixedSteps) ||
        settings.steps > maxSteps || !std::isfinite(settings.dt) || settings.dt <= 0.0 ||
        !std::isfinite(settings.dt * static_cast<double>(settings.steps)) ||
        !std::isfinite(settings.initial) || !std::isfinite(settings.target) ||
        (byError && (!std::isfinite(settings.tolerance) || settings.tolerance < 0.0))) {
        throw std::invalid_argument("Invalid simulation settings.");
    }
    Plant plant(plantSettings, settings.initial);
    Pid pid(pidSettings, settings.dt);
    SimulationResult result{{}, byError ? StopReason::stepLimit : StopReason::completed};
    if (!byError) {
        result.samples.reserve(settings.steps + 1);
    }

    double output = settings.initial;
    double error = trackingError(settings.target, output);
    double command = 0.0;
    for (std::size_t step = 0; ; ++step) {
        result.samples.push_back({step, settings.dt * static_cast<double>(step),
                                  settings.target, output, error, command});
        if (byError && std::abs(error) <= settings.tolerance) {
            result.reason = StopReason::toleranceReached;
            break;
        }
        if (step == settings.steps) {
            break;
        }
        command = pid.advance(error);
        output = plant.advance(command, settings.dt);
        error = trackingError(settings.target, output);
    }
    return result;
}

} // namespace lab03
