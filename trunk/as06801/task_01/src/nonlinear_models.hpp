#pragma once
// Блок 2. Нелинейные дискретные модели.
#include "model.hpp"

inline double sgn(double x) { return x > 0 ? 1.0 : (x < 0 ? -1.0 : 0.0); }

// 2.1  y+ = a y - b y_{t-1}^2 + c u + d sin(u_{t-1})
struct Model21 : Model {
    double a, b, c, d;
    Model21(const Params& P) : Model("2.1", "Квадратичная ОС и гармоническое управление"),
        a(P.get("a", .8)), b(P.get("b", .1)), c(P.get("c", .5)), d(P.get("d", .2)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() - b * x.Y(1) * x.Y(1) + c * x.U() + d * std::sin(x.U(1)); }
};
// 2.2  y+ = a y + b sat(u)
struct Model22 : Model {
    double a, b, umax, umin;
    Model22(const Params& P) : Model("2.2", "Насыщение исполнительного органа"),
        a(P.get("a", .8)), b(P.get("b", .5)), umax(P.get("Umax", 1)), umin(P.get("Umin", -1)) {}
    double nextStep(const Ctx& x) const override {
        double u = x.U(); u = u > umax ? umax : (u < umin ? umin : u);
        return a * x.Y() + b * u; }
};
// 2.3  y+ = a y + b deadzone(u)
struct Model23 : Model {
    double a, b, dl;
    Model23(const Params& P) : Model("2.3", "Зона нечувствительности"),
        a(P.get("a", .8)), b(P.get("b", .5)), dl(P.get("delta", .3)) {}
    double nextStep(const Ctx& x) const override {
        double u = x.U(), dz = u > dl ? u - dl : (u < -dl ? u + dl : 0.0);
        return a * x.Y() + b * dz; }
};
// 2.4  y+ = a y cos(y) + b ln(1+|u|)
struct Model24 : Model {
    double a, b;
    Model24(const Params& P) : Model("2.4", "Логарифмическое взаимодействие"),
        a(P.get("a", .8)), b(P.get("b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() * std::cos(x.Y()) + b * std::log(1 + std::fabs(x.U())); }
};
// 2.5  y+ = a y + b sign(u) (1 - e^{-|u|})
struct Model25 : Model {
    double a, b;
    Model25(const Params& P) : Model("2.5", "Сигнум-трение и экспоненциальный рост"),
        a(P.get("a", .8)), b(P.get("b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() + b * sgn(x.U()) * (1 - std::exp(-std::fabs(x.U()))); }
};
// 2.6  y+ = a1 y^3 - a2 y_{t-1} + b u^2
struct Model26 : Model {
    double a1, a2, b;
    Model26(const Params& P) : Model("2.6", "Асимметричный полиномиальный люфт"),
        a1(P.get("a1", .5)), a2(P.get("a2", .2)), b(P.get("b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a1 * std::pow(x.Y(), 3) - a2 * x.Y(1) + b * x.U() * x.U(); }
};
// 2.7  y+ = a y + b relay(u, y)
struct Model27 : Model {
    double a, b, eps;
    Model27(const Params& P) : Model("2.7", "Реле с гистерезисом"),
        a(P.get("a", .8)), b(P.get("b", .5)), eps(P.get("eps", .1)) {}
    double nextStep(const Ctx& x) const override {
        double u = x.U(), r;
        if (u > eps) r = 1; else if (u < -eps) r = -1; else r = x.Y() > 0 ? 1 : -1;
        return a * x.Y() + b * r; }
};
// 2.8  y+ = a y (1-y) + b u + c sin(y_{t-1} u)
struct Model28 : Model {
    double a, b, c;
    Model28(const Params& P) : Model("2.8", "Хаотическое логистическое отображение"),
        a(P.get("a", 2.5)), b(P.get("b", .1)), c(P.get("c", .1)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() * (1 - x.Y()) + b * x.U() + c * std::sin(x.Y(1) * x.U()); }
};
// 2.9  y+ = a y + b sqrt(|u|) sign(u)
struct Model29 : Model {
    double a, b;
    Model29(const Params& P) : Model("2.9", "Модуляция корнем"),
        a(P.get("a", .8)), b(P.get("b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a * x.Y() + b * std::sqrt(std::fabs(x.U())) * sgn(x.U()); }
};
// 2.10  y+ = a tanh(y) + b u^3
struct Model210 : Model {
    double a, b;
    Model210(const Params& P) : Model("2.10", "Сглаживание гиперболическим тангенсом"),
        a(P.get("a", .8)), b(P.get("b", .5)) {}
    double nextStep(const Ctx& x) const override {
        return a * std::tanh(x.Y()) + b * std::pow(x.U(), 3); }
};
