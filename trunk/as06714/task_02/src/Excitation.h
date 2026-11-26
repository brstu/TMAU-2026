#pragma once

#include <string>

#include "InputSignal.h"

// Источник управляющего воздействия.
// В ЛР №1 воздействие задавалось свободной функцией getInputValue();
// здесь оно обёрнуто в интерфейс, чтобы в тестах подменять его mock-объектом.
class Excitation {
public:
    virtual ~Excitation() = default;
    virtual double sample(int tau) const = 0;
    virtual std::string title() const = 0;
};

// Воздействие из ЛР №1 (ступенька, импульс, синусоида) с настраиваемой амплитудой
class LabExcitation final : public Excitation {
public:
    explicit LabExcitation(InputType kind, double gain = 1.0);

    double sample(int tau) const override;
    std::string title() const override;
    InputType kind() const { return kind_; }

private:
    InputType kind_;
    double gain_;
};
