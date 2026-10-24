#include "model.h"

#include <cmath>
#include <sstream>

std::string Model::stabilityInfo() const {
    return {};
}

LinearModel12::LinearModel12(double a, double b, std::size_t delay)
    : a_(a), b_(b), delay_(delay) {}

std::string LinearModel12::name() const {
    return "Model 1.2 — система с запаздыванием управления";
}

double LinearModel12::nextStep(std::size_t tau,
                               const std::vector<double>& y,
                               const std::vector<double>& u,
                               double) const {
    const double delayedInput = tau >= delay_ ? u[tau - delay_] : 0.0;
    return a_ * y[tau] + b_ * delayedInput;
}

std::string LinearModel12::stabilityInfo() const {
    std::ostringstream out;
    out << "Проверка устойчивости Model 1.2: характеристическое уравнение z - a = 0, "
        << "корень z = " << a_ << ". Критерий устойчивости: |a| < 1.";
    if (std::abs(a_) >= 1.0) {
        out << "\nПРЕДУПРЕЖДЕНИЕ: |a| >= 1, свободная составляющая не затухает; "
            << "система неустойчива или находится на границе устойчивости.";
    } else {
        out << "\nУсловие |a| < 1 выполнено: свободная составляющая затухает.";
    }
    return out.str();
}

NonlinearModel26::NonlinearModel26(double a1, double a2, double b)
    : a1_(a1), a2_(a2), b_(b) {}

std::string NonlinearModel26::name() const {
    return "Model 2.6 — асимметричная полиномиальная модель";
}

double NonlinearModel26::nextStep(std::size_t tau,
                                  const std::vector<double>& y,
                                  const std::vector<double>& u,
                                  double) const {
    const double previousY = tau >= 1 ? y[tau - 1] : 0.0;
    return a1_ * y[tau] * y[tau] * y[tau]
           - a2_ * previousY
           + b_ * u[tau] * u[tau];
}

DifferentialModel310::DifferentialModel310(double a, double b)
    : a_(a), b_(b) {}

std::string DifferentialModel310::name() const {
    return "Model 3.10 — дифференциальная модель с постоянным смещением";
}

double DifferentialModel310::nextStep(std::size_t tau,
                                      const std::vector<double>& y,
                                      const std::vector<double>& u,
                                      double dt) const {
    const double derivative = -a_ * y[tau] + b_ + u[tau];
    return y[tau] + dt * derivative;
}
