#pragma once

#include <cstddef>
#include <vector>

/** @mainpage Laboratory 03 вЂ” PID control, variant 21
 *
 * The simulator closes the loop around models 1.2, 2.7 and 3.5.
 * The controller receives the tracking error and returns a limited actuator command.
 * The simulator either runs a fixed number of steps or stops on the first sample
 * inside the requested error band, subject to a step limit.
 */

/** Discrete PID, mathematical plants and closed-loop simulation. */
namespace lab03 {

/** Selects the equation advanced at each simulation step. */
enum class PlantKind {
    delayedLinear,  ///< Model 1.2, pure delay in the input channel.
    relay,          ///< Model 2.7, relay with hysteresis determined by the output sign.
    harmonic        ///< Model 3.5, explicit Euler for dy/dt = b sin(u).
};

/** Parameters of the selected mathematical plant. */
struct PlantSettings {
    PlantKind kind;           ///< Equation to use.
    double a;                 ///< Discrete feedback coefficient in models 1.2 and 2.7.
    double b;                 ///< Input or forcing coefficient in all three models.
    std::size_t delay = 0;    ///< Input delay in steps for model 1.2.
    double threshold = 0.0;   ///< Nonnegative relay threshold for model 2.7.
};

/** Controller gains and actuator bounds. */
struct PidSettings {
    double kp;        ///< Proportional gain.
    double ki;        ///< Integral gain per time unit.
    double kd;        ///< Derivative gain times a time unit.
    double minimum;   ///< Lower actuator limit, at or below zero.
    double maximum;   ///< Upper actuator limit, at or above zero.
};

/** Incremental PID using the previous saturated command as its state. */
class Pid {
public:
    /** @param settings Gains and limits. @param dt Positive sampling period. */
    Pid(const PidSettings& settings, double dt);

    /** Advance the controller from two previous errors, initially zero.
     *  @param error Current target minus plant output.
     *  @return New command clipped to the actuator limits.
     */
    double advance(double error);

private:
    PidSettings settings_;
    double dt_;
    double previous_ = 0.0;
    double older_ = 0.0;
    double command_ = 0.0;
};

/** User-selectable stop rule. */
enum class StopMode {
    fixedSteps,       ///< Use exactly the requested number of updates.
    errorTolerance    ///< Stop at the first sample inside the error band.
};

/** Reason the simulation returned. */
enum class StopReason {
    completed,        ///< All fixed steps were completed.
    toleranceReached, ///< The initial or a later sample is in the error band.
    stepLimit         ///< The error band was not reached before the maximum.
};

/** Independent parameters of one simulation run. */
struct SimulationSettings {
    StopMode mode;           ///< Simulation stop rule.
    std::size_t steps;       ///< Step count in mode A or upper bound in mode B.
    double dt;               ///< Positive sampling period.
    double initial;          ///< Initial output.
    double target;           ///< Constant target temperature.
    double tolerance = 0.0;  ///< Absolute error bound in mode B.
};

/** State observed immediately after one update. */
struct Sample {
    std::size_t step;    ///< Sample index, zero before any update.
    double time;         ///< Timestamp in simulation time units.
    double target;       ///< Constant reference value.
    double output;       ///< Current output.
    double error;        ///< Current target minus output.
    double command;      ///< Command issued during the preceding step, initially zero.
};

/** Samples, including the initial state, and stop reason. */
struct SimulationResult {
    std::vector<Sample> samples;  ///< Samples in time order.
    StopReason reason;            ///< Why the algorithm stopped.
};

/** Run a fresh plant and controller in closed loop.
 *  @param plant Selected plant equation and coefficients.
 *  @param pid Controller gains and actuator bounds.
 *  @param settings Initial state, setpoint, timestep and stop rule.
 *  @return Recorded samples and termination reason.
 *  @throws std::invalid_argument For invalid or non-finite settings.
 *  @throws std::overflow_error If a computed value is not finite.
 */
SimulationResult simulate(const PlantSettings& plant, const PidSettings& pid,
                          const SimulationSettings& settings);

} // namespace lab03
