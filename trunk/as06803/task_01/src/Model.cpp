#include "Model.h"

#include <cmath>
#include <iomanip>
#include <sstream>

bool Model::isStable() const {
    return true;
}

std::string Model::stabilityInfo() const {
    return "Для выбранной модели проверка устойчивости не предусмотрена.";
}

LinearModel18::LinearModel18(double a1Value, double a2Value,
                             double b1Value, double b2Value)
    : a1(a1Value), a2(a2Value), b1(b1Value), b2(b2Value) {}

double LinearModel18::nextStep(double yCurrent,
                               double yPrevious,
                               double uCurrent,
                               double uPrevious,
                               double) const {
    return a1 * yCurrent + a2 * yPrevious
         + b1 * uCurrent + b2 * uPrevious;
}

std::string LinearModel18::name() const {
    return "Model 1.8 — линейная авторегрессионная модель";
}

bool LinearModel18::isStable() const {
    const std::complex<double> discriminant(a1 * a1 + 4.0 * a2, 0.0);
    const std::complex<double> rootPart = std::sqrt(discriminant);
    const std::complex<double> z1 = (a1 + rootPart) / 2.0;
    const std::complex<double> z2 = (a1 - rootPart) / 2.0;
    return std::abs(z1) < 1.0 && std::abs(z2) < 1.0;
}

std::string LinearModel18::stabilityInfo() const {
    const std::complex<double> discriminant(a1 * a1 + 4.0 * a2, 0.0);
    const std::complex<double> rootPart = std::sqrt(discriminant);
    const std::complex<double> z1 = (a1 + rootPart) / 2.0;
    const std::complex<double> z2 = (a1 - rootPart) / 2.0;

    std::ostringstream result;
    result << std::fixed << std::setprecision(4);
    result << "Корни характеристического уравнения: z1 = " << z1
           << ", |z1| = " << std::abs(z1)
           << "; z2 = " << z2
           << ", |z2| = " << std::abs(z2);
    return result.str();
}

NonlinearModel210::NonlinearModel210(double aValue, double bValue)
    : a(aValue), b(bValue) {}

double NonlinearModel210::nextStep(double yCurrent,
                                   double,
                                   double uCurrent,
                                   double,
                                   double) const {
    return a * std::tanh(yCurrent) + b * uCurrent * uCurrent * uCurrent;
}

std::string NonlinearModel210::name() const {
    return "Model 2.10 — нелинейная модель с tanh";
}

DifferentialModel32::DifferentialModel32(double bValue) : b(bValue) {}

double DifferentialModel32::nextStep(double yCurrent,
                                      double,
                                      double uCurrent,
                                      double,
                                      double deltaT) const {
    return yCurrent + deltaT * b * uCurrent;
}

std::string DifferentialModel32::name() const {
    return "Model 3.2 — дифференциальное уравнение с постоянным входом";
}

double getInputValue(InputSignal signal, int tau, double stepAmplitude) {
    switch (signal) {
        case InputSignal::Step:
            return stepAmplitude;
        case InputSignal::Impulse:
            return tau == 0 ? 1.0 : 0.0;
        case InputSignal::Harmonic:
            return std::sin(static_cast<double>(tau));
    }
    return 0.0;
}

std::string getInputSignalName(InputSignal signal) {
    switch (signal) {
        case InputSignal::Step:
            return "Ступенчатое воздействие";
        case InputSignal::Impulse:
            return "Импульсное воздействие";
        case InputSignal::Harmonic:
            return "Гармоническое воздействие";
    }
    return "Неизвестное воздействие";
}
