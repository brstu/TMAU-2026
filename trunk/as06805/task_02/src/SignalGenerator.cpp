#include "SignalGenerator.h"

SignalGenerator::SignalGenerator(SignalType type, double amp)
    : currentType_(type), signalAmp_(amp) {}

double SignalGenerator::get(double tau) const {
    switch (currentType_) {
        case SignalType::STEP:
            return signalAmp_;
        case SignalType::IMPULSE:
            return (tau < 1e-9) ? signalAmp_ : 0.0;
        case SignalType::HARMONIC:
            return signalAmp_ * std::sin(tau);
    }
    return 0.0;
}

std::string SignalGenerator::typeName(SignalType type) {
    switch (type) {
        case SignalType::STEP:
            return "step";
        case SignalType::IMPULSE:
            return "impulse";
        case SignalType::HARMONIC:
            return "harmonic";
    }
    return "unknown";
}
