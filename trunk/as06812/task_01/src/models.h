#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

enum class SignalType {
    Step = 1,
    Impulse = 2,
    Harmonic = 3
};

inline double signalValue(SignalType type, int tau) {
    switch (type) {
         case SignalType::Step:
            return 1.0;
        case SignalType::Impulse:
            return (tau == 0) ? 1.0 : 0.0;
        case SignalType::Harmonic:
            return std::sin(static_cast<double>(tau));
    }
    return 0.0;
}

inline const char* signalName(SignalType type) {
    switch (type) {
        case SignalType::Step:
            return "Step";
        case SignalType::Impulse:
            return "Impulse";
        case SignalType::Harmonic:
            return "Harmonic";
    }
    return "Unknown";
}

// Базовый абстрактный класс: конкретное уравнение задаёт наследник.
class Model {
public:
    Model(std::string title, std::string csvName)
        : title_(std::move(title)), csvName_(std::move(csvName)) {}
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    virtual ~Model() = default;

    const std::string& title() const { return title_; }
    const std::string& csvName() const { return csvName_; }

    // Принимает u(tau), возвращает y(tau+1).
    virtual double nextValue(double u) = 0;

private:
    std::string title_;
    std::string csvName_;
};

// Модель 1.10: y(t+1) = a1*y(t) + b1*u(t) + b2*u(t-2)
class Model1_10 final : public Model {
public:
    Model1_10(double a1, double b1, double b2)
        : Model("Модель 1.10", "Model 1.10"), a1_(a1), b1_(b1), b2_(b2) {}

    double nextValue(double u) override {
        const double yNext = a1_ * y_ + b1_ * u + b2_ * uPrev2_;
        uPrev2_ = uPrev1_;
        uPrev1_ = u;
        y_ = yNext;
        return yNext;
    }

private:
    double a1_;
    double b1_;
    double b2_;
    double y_ = 0.0;
    double uPrev1_ = 0.0;
    double uPrev2_ = 0.0;
};

// Модель 2.2: y(t+1) = a*y(t) + b*sat(u(t)), sat ограничивает u в [uMin, uMax]
class Model2_2 final : public Model {
public:
    Model2_2(double a, double b, double uMin, double uMax)
        : Model("Модель 2.2", "Model 2.2"), a_(a), b_(b), uMin_(uMin), uMax_(uMax) {}

    double nextValue(double u) override {
        y_ = a_ * y_ + b_ * std::clamp(u, uMin_, uMax_);
        return y_;
    }

private:
    double a_;
    double b_;
    double uMin_;
    double uMax_;
    double y_ = 0.0;
};

// Модель 3.4: dy/dt = -a*y^2, метод Эйлера: y(t+1) = y(t) - dt*a*y(t)^2
// Вход u в уравнение не входит.
class Model3_4 final : public Model {
public:
    Model3_4(double a, double deltaT, double y0)
        : Model("Модель 3.4", "Model 3.4"), a_(a), deltaT_(deltaT), y_(y0) {}

    double nextValue(double /*u*/) override {
        y_ -= deltaT_ * a_ * y_ * y_;
        return y_;
    }

private:
    double a_;
    double deltaT_;
    double y_;
};