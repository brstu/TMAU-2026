#include "pid.hpp"

PidController::PidController(double kp, double ki, double kd, double dt,
                             double u_min, double u_max)
    : kp(kp), ki(ki), kd(kd), dt(dt), u_min(u_min), u_max(u_max) {}

double PidController::compute(double e) {
    const double delta =
        kp * (e - e1) + ki * e * dt + kd / dt * (e - 2 * e1 + e2);

    u = clamp(u + delta);
    e2 = e1;
    e1 = e;

    return u;
}

void PidController::reset() {
    e1 = 0.0;
    e2 = 0.0;
    u = 0.0;
}

double PidController::get_u() const {
    return u;
}

double PidController::get_kp() const {
    return kp;
}

double PidController::get_ki() const {
    return ki;
}

double PidController::get_kd() const {
    return kd;
}

double PidController::clamp(double value) const {
    if (value < u_min) {
        return u_min;
    }
    if (value > u_max) {
        return u_max;
    }
    return value;
}
