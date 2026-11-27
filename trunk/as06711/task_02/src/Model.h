#pragma once
#include <vector>

// Абстрактный базовый класс для всех моделей варианта 19.
class Model {
public:
    virtual ~Model() = default;

    // Один шаг моделирования: получить вход u, вернуть новое значение y
    virtual double step(double u) = 0;

    // Привести внутреннее состояние к начального (нулевому)
    virtual void reset() = 0;

    // Человекочитаемое имя модели
    virtual const char* name() const = 0;

    // Необязательная установка начальных значений (y[k], y[k-1], y[k-2])
    virtual void setInitialState(const std::vector<double>& /*init*/) {}
};
