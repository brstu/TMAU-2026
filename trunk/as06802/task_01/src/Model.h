#pragma once

class Model {
public:
    virtual ~Model() = default;
    virtual double step(double u) = 0; // Виртуальный метод расчета
    virtual void reset() = 0;          // Сброс состояния
};