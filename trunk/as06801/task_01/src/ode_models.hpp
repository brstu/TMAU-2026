#pragma once
// Блок 3. Дифференциальные уравнения, метод Эйлера: y+ = y + dt * f(y,u).
#include "model.hpp"

class OdeModel : public Model {
public:
    OdeModel(const std::string& id, const std::string& title, const Params& P)
        : Model(id, title), dt(P.get("dt", 0.1)) {}
    double nextStep(const Ctx& x) const override { return x.Y() + dt * rhs(x.Y(), x.U()); }
protected:
    virtual double rhs(double y, double u) const = 0;  // правая часть dy/dt

private:
    double dt;
};

struct Model31 : OdeModel {
    double a;
    explicit Model31(const Params& P) : OdeModel("3.1", "Линейный распад", P), a(P.get("a", 1)) {}
    double rhs(double y, double) const override { return -a * y; }
};
struct Model32 : OdeModel {
    double b;
    explicit Model32(const Params& P) : OdeModel("3.2", "Интегрирование входа", P), b(P.get("b", 1)) {}
    double rhs(double, double u) const override { return b * u; }
};
struct Model33 : OdeModel {
    double a;
    double b;
    explicit Model33(const Params& P) : OdeModel("3.3", "Процесс 1-го порядка", P),
        a(P.get("a", 1)), b(P.get("b", 1)) {}
    double rhs(double y, double u) const override { return -a * y + b * u; }
};
struct Model34 : OdeModel {
    double a;
    explicit Model34(const Params& P) : OdeModel("3.4", "Квадратичный распад", P), a(P.get("a", 1)) {}
    double rhs(double y, double) const override { return -a * y * y; }
};
struct Model35 : OdeModel {
    double b;
    explicit Model35(const Params& P) : OdeModel("3.5", "Гармоническое воздействие", P), b(P.get("b", 1)) {}
    double rhs(double, double u) const override { return b * std::sin(u); }
};
struct Model36 : OdeModel {
    double a;
    double b;
    explicit Model36(const Params& P) : OdeModel("3.6", "Кубический рост и управление", P),
        a(P.get("a", -1)), b(P.get("b", 1)) {}
    double rhs(double y, double u) const override { return a * y * y * y + b * u; }
};
struct Model37 : OdeModel {
    double a;
    double b;
    explicit Model37(const Params& P) : OdeModel("3.7", "Экспоненциальное масштабирование", P),
        a(P.get("a", 0)), b(P.get("b", 1)) {}
    double rhs(double y, double u) const override { return -std::exp(a) * y + b * u; }
};
struct Model38 : OdeModel {
    double a1;
    double a2;
    double b;
    explicit Model38(const Params& P) : OdeModel("3.8", "Линейно-квадратичный распад", P),
        a1(P.get("a1", 1)), a2(P.get("a2", .5)), b(P.get("b", 1)) {}
    double rhs(double y, double u) const override { return -a1 * y - a2 * y * y + b * u; }
};
struct Model39 : OdeModel {
    double b;
    explicit Model39(const Params& P) : OdeModel("3.9", "Ограниченная скорость", P), b(P.get("b", 1)) {}
    double rhs(double, double u) const override { return b * std::tanh(u); }
};
struct Model310 : OdeModel {
    double a;
    double b;
    explicit Model310(const Params& P) : OdeModel("3.10", "Постоянное смещение", P),
        a(P.get("a", 1)), b(P.get("b", .5)) {}
    double rhs(double y, double u) const override { return -a * y + b + u; }
};