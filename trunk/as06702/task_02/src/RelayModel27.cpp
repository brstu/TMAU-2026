#include "RelayModel27.h"

RelayModel27::RelayModel27(double a, double b, double epsilon, double initialY)
    : Model(initialY), a_(a), b_(b), epsilon_(epsilon < 0.0 ? 0.0 : epsilon) {}

double RelayModel27::relay(double u) const {
    if (u > epsilon_) return 1.0;
    if (u < -epsilon_) return -1.0;
    return y_ > 0.0 ? 1.0 : -1.0;
}

double RelayModel27::step(double u, double /*dt*/) {
    y_ = a_ * y_ + b_ * relay(u);
    return y_;
}

std::string RelayModel27::name() const {
    return "Model 2.7 - Relay";
}

double RelayModel27::epsilon() const { return epsilon_; }
