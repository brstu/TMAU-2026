#pragma once

#include <cmath>
#include <stdexcept>

enum class SignalType {
    STEP,
    IMPULSE,
    HARMONIC
};

class SignalGenerator {
private:
    SignalType currentType_;
    double signalAmp_;

public:
    explicit SignalGenerator(SignalType type = SignalType::STEP, double amp = 1.0)
        : currentType_(type), signalAmp_(amp) {}

    double get(double tauStep) const {
        switch (currentType_) {
            case SignalType::STEP:
                return signalAmp_;
            case SignalType::IMPULSE:
                return (tauStep < 1e-9) ? signalAmp_ : 0.0;
            case SignalType::HARMONIC:
                return signalAmp_ * std::sin(tauStep);
            default:
                return 0.0;
        }
    }

    SignalType getType() const { return currentType_; }
    double getAmplitude() const { return signalAmp_; }
};