#pragma once
// Блок 2. Нелинейные дискретные модели.
#include <algorithm>
#include "model.hpp"

inline double sgn(double x) {
    if (x > 0) return 1.0;
    if (x < 0) return -1.0;
    return 0.0;
}

// Ограничение значения диапазоном [lo, hi].
inline double saturate(double u, double lo, double hi) { return std::min(hi, std::max(lo, u)); }

// Зона нечувствительности с половиной ширины d.
inline double deadZone(double u, double d) {
    if (u > d) return u - d;
    if (u < -d) return u + d;
    return 0.0;
}

// Модель 2.1 (уравнение — в readme.md)
struct Model21 : Model {
    double a;
    double b;
    double c;
    double d;
    explicit Model21(const Params& P) : Model("2.1", "Квадратичная ОС и гармоническое управление"),
        a(param(P, "a", .8)), b(param(P, "b", .1)), c(param(P, "c", .5)), d(param(P, "d", .2)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() - b * x.Y(1) * x.Y(1) + c * x.U() + d * std::sin(x.U(1)); }
};
// Модель 2.2 (уравнение — в readme.md)
struct Model22 : Model {
    double a;
    double b;
    double umax;
    double umin;
    explicit Model22(const Params& P) : Model("2.2", "Насыщение исполнительного органа"),
        a(param(P, "a", .8)), b(param(P, "b", .5)), umax(param(P, "Umax", 1)), umin(param(P, "Umin", -1)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() + b * saturate(x.U(), umin, umax); }
};
// Модель 2.3 (уравнение — в readme.md)
struct Model23 : Model {
    double a;
    double b;
    double dl;
    explicit Model23(const Params& P) : Model("2.3", "Зона нечувствительности"),
        a(param(P, "a", .8)), b(param(P, "b", .5)), dl(param(P, "delta", .3)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() + b * deadZone(x.U(), dl); }
};
// Модель 2.4 (уравнение — в readme.md)
struct Model24 : Model {
    double a;
    double b;
    explicit Model24(const Params& P) : Model("2.4", "Логарифмическое взаимодействие"),
        a(param(P, "a", .8)), b(param(P, "b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() * std::cos(x.Y()) + b * std::log(1 + std::fabs(x.U())); }
};
// Модель 2.5 (уравнение — в readme.md)
struct Model25 : Model {
    double a;
    double b;
    explicit Model25(const Params& P) : Model("2.5", "Сигнум-трение и экспоненциальный рост"),
        a(param(P, "a", .8)), b(param(P, "b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() + b * sgn(x.U()) * (1 - std::exp(-std::fabs(x.U()))); }
};
// Модель 2.6 (уравнение — в readme.md)
struct Model26 : Model {
    double a1;
    double a2;
    double b;
    explicit Model26(const Params& P) : Model("2.6", "Асимметричный полиномиальный люфт"),
        a1(param(P, "a1", .5)), a2(param(P, "a2", .2)), b(param(P, "b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a1 * std::pow(x.Y(), 3) - a2 * x.Y(1) + b * x.U() * x.U(); }
};
// Модель 2.7 (уравнение — в readme.md)
struct Model27 : Model {
    double a;
    double b;
    double eps;
    explicit Model27(const Params& P) : Model("2.7", "Реле с гистерезисом"),
        a(param(P, "a", .8)), b(param(P, "b", .5)), eps(param(P, "eps", .1)) {}
    double nextStep(const Ctx& x) const override {
        const double u = x.U();
        double r = x.Y() > 0 ? 1.0 : -1.0;  // внутри зоны гистерезиса реле сохраняет знак y
        if (u > eps) r = 1.0;
        else if (u < -eps) r = -1.0;
        return a * x.Y() + b * r; }
};
// Модель 2.8 (уравнение — в readme.md)
struct Model28 : Model {
    double a;
    double b;
    double c;
    explicit Model28(const Params& P) : Model("2.8", "Хаотическое логистическое отображение"),
        a(param(P, "a", 2.5)), b(param(P, "b", .1)), c(param(P, "c", .1)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() * (1 - x.Y()) + b * x.U() + c * std::sin(x.Y(1) * x.U()); }
};
// Модель 2.9 (уравнение — в readme.md)
struct Model29 : Model {
    double a;
    double b;
    explicit Model29(const Params& P) : Model("2.9", "Модуляция корнем"),
        a(param(P, "a", .8)), b(param(P, "b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() + b * std::sqrt(std::fabs(x.U())) * sgn(x.U()); }
};
// Модель 2.10 (уравнение — в readme.md)
struct Model210 : Model {
    double a;
    double b;
    explicit Model210(const Params& P) : Model("2.10", "Сглаживание гиперболическим тангенсом"),
        a(param(P, "a", .8)), b(param(P, "b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a * std::tanh(x.Y()) + b * std::pow(x.U(), 3); }
};