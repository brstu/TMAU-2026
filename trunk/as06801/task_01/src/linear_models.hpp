#pragma once
// Блок 1. Линейные дискретные модели.
// y_{t+1} = sum_j yc[j]*y_{t-j} + sum_j uc[j]*u_{t-j}
#include <algorithm>
#include <complex>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "model.hpp"

namespace detail {

using Complex = std::complex<double>;

// Значение многочлена p[0]*x^m + ... + p[m] по схеме Горнера.
inline Complex polyValue(const std::vector<Complex>& p, const Complex& x) {
    Complex val = p[0];
    for (size_t j = 1; j < p.size(); ++j) val = val * x + p[j];
    return val;
}

// Произведение (z[i] - z[j]) по всем j != i.
inline Complex rootsProduct(const std::vector<Complex>& z, size_t i) {
    Complex den = 1;
    for (size_t j = 0; j < z.size(); ++j) {
        if (j != i) den *= (z[i] - z[j]);
    }
    return den;
}

// Одна итерация метода Дюрана–Кернера; возвращает максимальную поправку.
inline double durandKernerStep(const std::vector<Complex>& p, std::vector<Complex>& z) {
    double delta = 0;
    for (size_t i = 0; i < z.size(); ++i) {
        Complex den = rootsProduct(z, i);
        if (std::abs(den) < 1e-14) den = 1e-14;
        const Complex d = polyValue(p, z[i]) / den;
        z[i] -= d;
        delta = std::max(delta, std::abs(d));
    }
    return delta;
}

// Текст "zN = re+imi (|z|=модуль)" для вывода корня.
inline std::string formatRoot(size_t index, const Complex& z, bool last) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(4) << "z" << index << " = " << z.real()
       << std::showpos << z.imag() << std::noshowpos << "i (|z|=" << std::abs(z) << ")";
    if (!last) os << "; ";
    return os.str();
}

// Все корни многочлена p (p[0] = 1).
inline std::vector<Complex> findRoots(const std::vector<Complex>& p) {
    std::vector<Complex> z(p.size() - 1);
    const Complex seed(0.4, 0.9);
    for (size_t i = 0; i < z.size(); ++i) z[i] = std::pow(seed, static_cast<int>(i));
    for (int it = 0; it < 2000; ++it) {
        if (durandKernerStep(p, z) < 1e-13) break;
    }
    return z;
}

}  // namespace detail

class LinearModel : public Model {
public:
    LinearModel(const std::string& id, const std::string& title) : Model(id, title) {}

    double nextStep(const Ctx& c) const override {
        double r = 0;
        for (size_t j = 0; j < yc.size(); ++j) r += yc[j] * c.Y(static_cast<int>(j));
        for (size_t j = 0; j < uc.size(); ++j) r += uc[j] * c.U(static_cast<int>(j));
        return r;
    }

    // Характеристическое уравнение: z^m - yc0*z^(m-1) - ... - yc_{m-1} = 0.
    // Устойчивость: все корни внутри единичной окружности |z| < 1.
    bool isStable(std::string& info) const override {
        using detail::Complex;
        using detail::findRoots;
        using detail::formatRoot;
        std::vector<Complex> p(yc.size() + 1);  // p[0]=1 (старший), p[j] = -yc[j-1]
        p[0] = 1;
        for (size_t j = 1; j < p.size(); ++j) p[j] = -yc[j - 1];
        const std::vector<Complex> z = findRoots(p);

        double rmax = 0;
        info = "корни характеристического уравнения: ";
        for (size_t i = 0; i < z.size(); ++i) {
            info += formatRoot(i + 1, z[i], i + 1 == z.size());
            rmax = std::max(rmax, std::abs(z[i]));
        }
        return rmax < 1.0 - 1e-9;
    }

protected:
    // Наследники задают коэффициенты: ycoef[j] при y_{t-j}, ucoef[j] при u_{t-j}.
    void setCoefficients(std::vector<double> ycoef, std::vector<double> ucoef) {
        yc = std::move(ycoef);
        uc = std::move(ucoef);
    }

private:
    std::vector<double> yc;
    std::vector<double> uc;
};

// Модель 1.1 (уравнение — в readme.md)
struct Model11 : LinearModel {
    explicit Model11(const Params& P) : LinearModel("1.1", "Инерционное звено 1-го порядка") {
        setCoefficients({P.get("a", .8)}, {P.get("b", .5)}); }
};
// Модель 1.2 (уравнение — в readme.md)
struct Model12 : LinearModel {
    explicit Model12(const Params& P) : LinearModel("1.2", "Чистое запаздывание по управлению") {
        const auto k = static_cast<size_t>(std::max(1, static_cast<int>(P.get("k", 2))));
        std::vector<double> u(k + 1, 0.0);
        u[k] = P.get("b", .5);
        setCoefficients({P.get("a", .8)}, u); }
};
// Модель 1.3 (уравнение — в readme.md)
struct Model13 : LinearModel {
    explicit Model13(const Params& P) : LinearModel("1.3", "Система 2-го порядка") {
        setCoefficients({P.get("a1", .5), P.get("a2", .2)}, {P.get("b", .5)}); }
};
// Модель 1.4 (уравнение — в readme.md)
struct Model14 : LinearModel {
    explicit Model14(const Params& P) : LinearModel("1.4", "Интегратор с потерями") {
        double b = P.get("b", .5);
        setCoefficients({P.get("a", .8)}, {b, -b}); }
};
// Модель 1.5 (уравнение — в readme.md)
struct Model15 : LinearModel {
    explicit Model15(const Params& P) : LinearModel("1.5", "Запаздывание по состоянию") {
        const auto k = static_cast<size_t>(std::max(1, static_cast<int>(P.get("k", 2))));
        std::vector<double> y(k + 1, 0.0);
        y[0] = P.get("a1", .5);
        y[k] += P.get("a2", .2);
        setCoefficients(y, {P.get("b", .5)}); }
};
// Модель 1.6 (уравнение — в readme.md)
struct Model16 : LinearModel {
    explicit Model16(const Params& P) : LinearModel("1.6", "Система 3-го порядка") {
        setCoefficients({P.get("a1", .5), P.get("a2", .2), P.get("a3", .1)}, {P.get("b", .5)}); }
};
// Модель 1.7 (уравнение — в readme.md)
struct Model17 : LinearModel {
    explicit Model17(const Params& P) : LinearModel("1.7", "История управления (3 шага)") {
        setCoefficients({P.get("a", .8)}, {P.get("b1", .5), P.get("b2", .3), P.get("b3", .2)}); }
};
// Модель 1.8 (уравнение — в readme.md)
struct Model18 : LinearModel {
    explicit Model18(const Params& P) : LinearModel("1.8", "Обобщённая авторегрессионная модель") {
        setCoefficients({P.get("a1", .5), P.get("a2", .2)}, {P.get("b1", .5), P.get("b2", .3)}); }
};
// Модель 1.9 (уравнение — в readme.md)
struct Model19 : LinearModel {
    explicit Model19(const Params& P) : LinearModel("1.9", "Трёхшаговая модель состояния") {
        setCoefficients({P.get("a1", .5), P.get("a2", .2), P.get("a3", .1)}, {P.get("b1", .5)}); }
};
// Модель 1.10 (уравнение — в readme.md)
struct Model110 : LinearModel {
    explicit Model110(const Params& P) : LinearModel("1.10", "Распределённое окно управления") {
        setCoefficients({P.get("a1", .8)}, {P.get("b1", .5), 0.0, P.get("b2", .3)}); }
};