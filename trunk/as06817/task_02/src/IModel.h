#pragma once

#include <string>
#include <vector>

// Общий интерфейс дискретной модели.
// step(u) выполняет один шаг моделирования и возвращает y(tau + 1).
class IModel {
public:
    virtual ~IModel() = default;

    virtual double step(double u) = 0;
    virtual void reset() = 0;
    virtual std::string name() const = 0;

    // Для нелинейных моделей анализ на Z-плоскости неприменим.
    virtual bool isLinear() const { return false; }

    // Коэффициенты характеристического уравнения a0*z^n + ... + an
    // (по убыванию степени). Пустой вектор означает, что уравнение не задано.
    virtual std::vector<double> characteristicPolynomial() const { return {}; }
};
