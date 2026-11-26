#ifndef TASK03_AS06818_MODEL_HPP
#define TASK03_AS06818_MODEL_HPP

/// @file model.hpp
/// Абстрактный объект управления.

/// Объект управления (Plant). Конкретные уравнения варианта 18
/// реализуются классами-наследниками.
class Model {
public:
    virtual ~Model() = default;

    /// Расчёт следующего состояния при входном воздействии u.
    /// Возвращает новое значение выходной величины y.
    virtual double step(double u) = 0;

    /// Возврат к начальным условиям.
    virtual void reset() = 0;

    /// Текущее значение выходной величины y.
    virtual double get_y() const = 0;
};

#endif
