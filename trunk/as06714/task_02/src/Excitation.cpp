#include "Excitation.h"

LabExcitation::LabExcitation(InputType kind, double gain) : kind_(kind), gain_(gain) {}

double LabExcitation::sample(int tau) const {
    return gain_ * getInputValue(kind_, tau);
}

std::string LabExcitation::title() const {
    switch (kind_) {
    case InputType::STEP:
        return "step";
    case InputType::IMPULSE:
        return "impulse";
    case InputType::HARMONIC:
        return "harmonic";
    default:
        return "unknown";
    }
}
