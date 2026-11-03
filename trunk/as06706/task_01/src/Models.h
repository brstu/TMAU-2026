#pragma once

#include <string>
#include <vector>

// Абстрактный источник входного воздействия u_tau.
class InputSignal {
public:
    virtual ~InputSignal() = default;
    virtual double value(int tau) const = 0;
    virtual std::string name() const = 0;
};

class StepSignal final : public InputSignal {
public:
    explicit StepSignal(double amplitude);
    double value(int tau) const override;
    std::string name() const override;

private:
    double amplitude_;
};

class ImpulseSignal final : public InputSignal {
public:
    double value(int tau) const override;
    std::string name() const override;
};

class HarmonicSignal final : public InputSignal {
public:
    double value(int tau) const override;
    std::string name() const override;
};

// Базовый абстрактный класс математической модели.
class Model {
public:
    virtual ~Model() = default;

    virtual std::string name() const = 0;
    virtual double calculateNext(
        int tau,
        double currentY,
        double currentInput,
        const std::vector<double>& inputHistory
    ) const = 0;

    // Пустая строка означает, что для модели проверка устойчивости не задана.
    virtual std::string stabilityInfo() const;
};

// Вариант 2, модель 1.2: y_(tau+1) = a*y_tau + b*u_(tau-k).
class DelayedLinearModel final : public Model {
public:
    DelayedLinearModel(double a, double b, int delay);

    std::string name() const override;
    double calculateNext(
        int tau,
        double currentY,
        double currentInput,
        const std::vector<double>& inputHistory
    ) const override;
    std::string stabilityInfo() const override;

private:
    double a_;
    double b_;
    int delay_;
};

// Вариант 2, модель 2.4: y_(tau+1) = a*y_tau*cos(y_tau)
//                         + b*ln(1 + |u_tau|).
class NonlinearLogModel final : public Model {
public:
    NonlinearLogModel(double a, double b);

    std::string name() const override;
    double calculateNext(
        int tau,
        double currentY,
        double currentInput,
        const std::vector<double>& inputHistory
    ) const override;

private:
    double a_;
    double b_;
};

// Вариант 2, модель 3.6: dy/dt = a*y^3 + b*u.
// Численное интегрирование выполняется методом Эйлера.
class DifferentialCubicModel final : public Model {
public:
    DifferentialCubicModel(double a, double b, double deltaT);

    std::string name() const override;
    double calculateNext(
        int tau,
        double currentY,
        double currentInput,
        const std::vector<double>& inputHistory
    ) const override;

private:
    double a_;
    double b_;
    double deltaT_;
};
