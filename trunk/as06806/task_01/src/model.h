
#pragma once

#include <string>
#include <utility>
#include <cmath>

enum class InputSignals {
    Step = 1,
    Impulse = 2,
    Harmonic = 3
};

inline double calculateInput(int tau, InputSignals type) {
    switch (type) {
    case InputSignals::Step:
        return 1.0;

    case InputSignals::Impulse:
        return (tau == 0) ? 1.0 : 0.0;

    case InputSignals::Harmonic:
        return std::sin(static_cast<double>(tau));
    }

    return 0.0;
}

class Model {
private:
    std::string name_;

public:
    explicit Model(std::string modelName)
        : name_(std::move(modelName)) {
    }

    virtual ~Model() = default;

    const std::string& getName() const {
        return name_;
    }

    virtual double calculateNextStep(double uTau) = 0;
};

class Model1_9 : public Model {
private:
    double a1_;
    double a2_;
    double a3_;
    double b1_;

    double yTau_;
    double yTauMinus1_;
    double yTauMinus2_;

public:
    Model1_9(
        double a1,
        double a2,
        double a3,
        double b1,
        double y0 = 0.0,
        double yMinus1 = 0.0,
        double yMinus2 = 0.0
    )
        : Model("Модель 1.9"),
          a1_(a1),
          a2_(a2),
          a3_(a3),
          b1_(b1),
          yTau_(y0),
          yTauMinus1_(yMinus1),
          yTauMinus2_(yMinus2) {
    }

    double calculateNextStep(double uTau) override {
        const double yNext =
            a1_ * yTau_
            + a2_ * yTauMinus1_
            + a3_ * yTauMinus2_
            + b1_ * uTau;

        yTauMinus2_ = yTauMinus1_;
        yTauMinus1_ = yTau_;
        yTau_ = yNext;

        return yNext;
    }
};

class Model2_1 : public Model {
private:
    double a_;
    double b_;
    double c_;
    double d_;

    double yTau_;
    double yTauMinus1_;
    double uTauMinus1_;

public:
    Model2_1(
        double a,
        double b,
        double c,
        double d,
        double y0 = 0.0,
        double yMinus1 = 0.0,
        double uMinus1 = 0.0
    )
        : Model("Модель 2.1"),
          a_(a),
          b_(b),
          c_(c),
          d_(d),
          yTau_(y0),
          yTauMinus1_(yMinus1),
          uTauMinus1_(uMinus1) {
    }

    double calculateNextStep(double uTau) override {
        const double yNext =
            a_ * yTau_
            - b_ * yTauMinus1_ * yTauMinus1_
            + c_ * uTau
            + d_ * std::sin(uTauMinus1_);

        yTauMinus1_ = yTau_;
        yTau_ = yNext;
        uTauMinus1_ = uTau;

        return yNext;
    }
};

class Model3_3 : public Model {
private:
    double a_;
    double b_;
    double deltaT_;
    double yTau_;

public:
    Model3_3(
        double a,
        double b,
        double deltaT,
        double y0 = 0.0
    )
        : Model("Модель 3.3"),
          a_(a),
          b_(b),
          deltaT_(deltaT),
          yTau_(y0) {
    }

    double calculateNextStep(double uTau) override {
        const double yNext =
            yTau_ + deltaT_ * (-a_ * yTau_ + b_ * uTau);

        yTau_ = yNext;

        return yNext;
    }
};
