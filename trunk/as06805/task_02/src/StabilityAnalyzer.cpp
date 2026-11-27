#include "StabilityAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iterator>
#include <ostream>
#include <sstream>

namespace {

// Перевод статуса в текстовую форму.
const char* statusName(StabilityAnalyzer::Status status) {
    switch (status) {
        case StabilityAnalyzer::Status::Stable:
            return "устойчива";
        case StabilityAnalyzer::Status::Marginal:
            return "на границе устойчивости";
        case StabilityAnalyzer::Status::Unstable:
            return "неустойчива";
    }
    return "неизвестна";
}

// Форматирование комплексного числа.
std::string formatRoot(const std::complex<double>& z) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4) << z.real();
    if (std::abs(z.imag()) > 1e-12) {
        out << (z.imag() >= 0.0 ? " + " : " - ") << std::abs(z.imag()) << "j";
    }
    return out.str();
}

}  // namespace

double StabilityAnalyzer::Result::maxModulus() const noexcept {
    if (moduli.empty()) {
        return 0.0;
    }
    return *std::max_element(moduli.begin(), moduli.end());
}

std::string StabilityAnalyzer::Result::summary() const {
    std::ostringstream out;
    out << "Процесс " << statusName(status) << "; ";
    if (roots.empty()) {
        out << "полюса не определены";
        return out.str();
    }
    out << "полюса: ";
    for (size_t i = 0; i < roots.size(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        out << "z" << (i + 1) << " = " << formatRoot(roots[i]) << " (|z| = "
            << std::fixed << std::setprecision(4) << moduli[i] << ")";
    }
    return out.str();
}

StabilityAnalyzer::Result StabilityAnalyzer::analyze(const Model& model) {
    Result result;
    result.roots = model.poles();

    result.moduli.reserve(result.roots.size());
    std::transform(result.roots.begin(), result.roots.end(), std::back_inserter(result.moduli),
                   [](const std::complex<double>& root) { return std::abs(root); });

    if (const double maxMod = result.maxModulus(); maxMod < 1.0 - kEps) {
        result.status = Status::Stable;
    } else if (maxMod <= 1.0 + kEps) {
        result.status = Status::Marginal;
    } else {
        result.status = Status::Unstable;
    }

    return result;
}

StabilityAnalyzer::Result StabilityAnalyzer::analyzeFirstOrder(double a) {
    Result result;
    result.roots = {std::complex<double>(a, 0.0)};
    result.moduli = {std::abs(a)};

    if (std::abs(a) < 1.0 - kEps) {
        result.status = Status::Stable;
    } else if (std::abs(a) <= 1.0 + kEps) {
        result.status = Status::Marginal;
    } else {
        result.status = Status::Unstable;
    }

    return result;
}

StabilityAnalyzer::Result StabilityAnalyzer::analyzeSecondOrder(double a1, double a2) {
    // Корни характеристического уравнения (см. StabilityAnalyzer.h).
    const std::complex<double> disc(a1 * a1 + 4.0 * a2, 0.0);
    const auto sq = std::sqrt(disc);
    const std::complex<double> half(0.5, 0.0);

    Result result;
    result.roots = {half * (std::complex<double>(a1, 0.0) + sq),
                    half * (std::complex<double>(a1, 0.0) - sq)};

    result.moduli.reserve(result.roots.size());
    std::transform(result.roots.begin(), result.roots.end(), std::back_inserter(result.moduli),
                   [](const std::complex<double>& root) { return std::abs(root); });

    if (const double maxMod = result.maxModulus(); maxMod < 1.0 - kEps) {
        result.status = Status::Stable;
    } else if (maxMod <= 1.0 + kEps) {
        result.status = Status::Marginal;
    } else {
        result.status = Status::Unstable;
    }

    return result;
}

StabilityAnalyzer::Result StabilityAnalyzer::analyzeEuler(double a, double dt) {
    // Дискретный полюс метода Эйлера (см. StabilityAnalyzer.h).
    const double pole = 1.0 - dt * std::exp(a);
    return analyzeFirstOrder(pole);
}

bool StabilityAnalyzer::jurySecondOrder(double p1, double p0, double eps) {
    // Аналитический критерий Джури (см. StabilityAnalyzer.h).
    return (std::abs(p0) < 1.0 - eps) && (1.0 + p1 + p0 > eps) &&
           (1.0 - p1 + p0 > eps);
}

bool StabilityAnalyzer::allInsideUnitCircle(const std::vector<double>& moduli, double eps) {
    return std::all_of(moduli.begin(), moduli.end(),
                       [eps](double m) { return m < 1.0 - eps; });
}

bool StabilityAnalyzer::allOnOrInsideUnitCircle(const std::vector<double>& moduli, double eps) {
    return std::none_of(moduli.begin(), moduli.end(),
                         [eps](double m) { return m > 1.0 + eps; });
}

std::string StabilityAnalyzer::warningMessage(const Result& result) {
    std::ostringstream out;
    if (result.status == Status::Marginal) {
        out << "ВНИМАНИЕ: процесс находится на границе устойчивости (|z| = 1)!\n";
    } else {
        out << "ВНИМАНИЕ: обнаружен неустойчивый (расходящийся) процесс!\n";
    }
    out << "  " << result.summary() << "\n";
    out << "  Процесс признан " << statusName(result.status)
        << ". Симуляция может давать расходящиеся значения.\n";
    return out.str();
}

bool StabilityAnalyzer::warnIfUnstable(const Model& model, std::ostream& os) {
    const Result result = analyze(model);
    if (result.isStable()) {
        return false;
    }
    os << warningMessage(result);
    os.flush();
    return true;
}

std::string StabilityAnalyzer::formatReport(const Model& model) {
    const Result result = analyze(model);

    std::ostringstream out;
    out << "--- Анализ устойчивости ---\n";
    out << "Модель: " << model.name() << "\n";
    out << "Уравнение: " << model.equation() << "\n";
    out << "Процесс: " << statusName(result.status) << "\n";
    for (size_t i = 0; i < result.roots.size(); ++i) {
        out << "  z" << (i + 1) << " = " << formatRoot(result.roots[i])
            << ", |z| = " << std::fixed << std::setprecision(6) << result.moduli[i] << "\n";
    }
    out << "Вывод: " << (result.isStable()
                             ? "все полюса внутри единичной окружности, процесс сходится"
                             : "не все полюса внутри единичной окружности, процесс может расходиться")
        << "\n";
    return out.str();
}
