#pragma once

class ISignalGenerator {
public:
    virtual ~ISignalGenerator() = default;
    virtual double get(double tau) const = 0;
};