#pragma once

// Интерфейс источника входного воздействия.
// Позволяет подменять реальный сигнал mock-объектом в тестах.
class ISignalSource {
public:
    virtual ~ISignalSource() = default;

    virtual double value(int tau) const = 0;
};
