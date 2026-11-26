#ifndef INPUT_SIGNAL_H
#define INPUT_SIGNAL_H

#include <memory>
#include <string>
#include <vector>

// Абстрактное входное воздействие, подаваемое на модель
class InputSignal {
public:
    virtual ~InputSignal() = default;

    // Значение сигнала на заданном шаге моделирования
    virtual double valueAt(int step) const = 0;

    // Короткое имя сигнала, используется в именах CSV-файлов
    virtual std::string name() const = 0;
};

// Ступенчатое воздействие постоянной величины
class StepSignal : public InputSignal {
public:
    explicit StepSignal(double level = 1.0) : level_(level) {}
    double valueAt(int step) const override;
    std::string name() const override { return "Step"; }

private:
    double level_;
};

// Единичный импульс, отличный от нуля только на нулевом шаге
class ImpulseSignal : public InputSignal {
public:
    explicit ImpulseSignal(double height = 1.0) : height_(height) {}
    double valueAt(int step) const override;
    std::string name() const override { return "Impulse"; }

private:
    double height_;
};

// Гармоническое воздействие, синус от номера шага
class HarmonicSignal : public InputSignal {
public:
    explicit HarmonicSignal(double amplitude = 1.0) : amplitude_(amplitude) {}
    double valueAt(int step) const override;
    std::string name() const override { return "Harmonic"; }

private:
    double amplitude_;
};

// Набор сигналов по пункту меню: 1, 2, 3 выбирают один сигнал, 4 выбирает все
std::vector<std::unique_ptr<InputSignal>> makeSignals(int menuChoice);

#endif // INPUT_SIGNAL_H
