#pragma once

// Интерфейс генератора входного сигнала (нужен для Google Mock).
class ISignalGenerator {
public:
    virtual ~ISignalGenerator() = default;
    virtual double get(double tau) const = 0;
};
