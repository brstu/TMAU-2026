#include "Models.h"

#include <cmath>
#include <sstream>

StepSignal::StepSignal(double amplitude)
    : amplitude_(amplitude) {}

double StepSignal::value(int /*tau*/) const {
    return amplitude_;
}

std::string StepSignal::name() const {
    return "Ступенчатое воздействие";
}

double ImpulseSignal::value(int tau) const {
    return (tau == 0) ? 1.0 : 0.0;
}

std::string ImpulseSignal::name() const {
    return "Импульсное воздействие";
}

double HarmonicSignal::value(int tau) const {
    return std::sin(static_cast<double>(tau));
}

std::string HarmonicSignal::name() const {
    return "Гармоническое воздействие sin(tau)";
}

std::string Model::stabilityInfo() const {
    return "";
}

DelayedLinearModel::DelayedLinearModel(double a, double b, int delay)
    : a_(a), b_(b), delay_(delay) {}

std::string DelayedLinearModel::name() const {
    return "Линейная модель 1.2 (задержка управления)";
}

double DelayedLinearModel::calculateNext(
    int tau,
    double currentY,
    double /*currentInput*/,
    const std::vector<double>& inputHistory
) const {
    const int delayedIndex = tau - delay_;
    double delayedInput = 0.0;

    // Для отрицательных индексов вход считается равным нулю.
    if (delayedIndex >= 0 && delayedIndex < static_cast<int>(inputHistory.size())) {
        delayedInput = inputHistory[static_cast<std::size_t>(delayedIndex)];
    }

    return a_ * currentY + b_ * delayedInput;
}

std::string DelayedLinearModel::stabilityInfo() const {
    std::ostringstream message;
    message << "Проверка устойчивости модели 1.2: характеристическое уравнение "
            << "z - a = 0, корень z = " << a_ << ". ";

    if (std::abs(a_) < 1.0) {
        message << "Так как |a| < 1, свободная составляющая затухает: "
                << "линейная модель асимптотически устойчива.";
    } else {
        message << "Так как |a| >= 1, свободная составляющая не затухает: "
                << "ВНИМАНИЕ, модель неустойчива или находится на границе устойчивости.";
    }

    message << " Задержка входа k не меняет корень характеристического "
            << "уравнения, поскольку относится к внешнему воздействию.";
    return message.str();
}

NonlinearLogModel::NonlinearLogModel(double a, double b)
    : a_(a), b_(b) {}

std::string NonlinearLogModel::name() const {
    return "Нелинейная модель 2.4 (косинус и логарифм)";
}

double NonlinearLogModel::calculateNext(
    int /*tau*/,
    double currentY,
    double currentInput,
    const std::vector<double>& /*inputHistory*/
) const {
    return a_ * currentY * std::cos(currentY)
         + b_ * std::log1p(std::abs(currentInput));
}

DifferentialCubicModel::DifferentialCubicModel(double a, double b, double deltaT)
    : a_(a), b_(b), deltaT_(deltaT) {}

std::string DifferentialCubicModel::name() const {
    return "Дифференциальная модель 3.6 (кубический рост и управление)";
}

double DifferentialCubicModel::calculateNext(
    int /*tau*/,
    double currentY,
    double currentInput,
    const std::vector<double>& /*inputHistory*/
) const {
    const double derivative = a_ * currentY * currentY * currentY
                            + b_ * currentInput;
    return currentY + deltaT_ * derivative;
}
