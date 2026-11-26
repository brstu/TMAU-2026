#pragma once

#include <complex>
#include <string>
#include <vector>

// Интерфейс источника входного сигнала u(tau).
// Выделен в отдельный интерфейс, чтобы в тестах можно было подменять
// реальные сигналы mock-объектами (Google Mock).
class ISignalSource {
public:
    virtual ~ISignalSource() = default;

    // Значение входного сигнала в момент дискретного времени tau.
    virtual double get(double tau) const = 0;
};

// Абстрактный базовый класс модели управляемого объекта.
// Определяет единый интерфейс расчёта шагов симуляции.
class Model {
public:
    virtual ~Model() = default;

    // Расчёт очередного шага модели при входном воздействии u.
    virtual double step(double u) = 0;

    // Сброс внутреннего состояния модели в нулевое.
    virtual void reset() = 0;

    // Имя модели (для вывода в консоль и отчётов).
    virtual std::string name() const = 0;

    // Полюса модели = корни характеристического уравнения на Z-плоскости.
    // Используются модулем анализа устойчивости (StabilityAnalyzer).
    virtual std::vector<std::complex<double>> poles() const = 0;

    // Текстовое описание разностного уравнения модели.
    virtual std::string equation() const = 0;

    // Установка начального состояния (y[0], y[-1], ...).
    virtual void setInitialState(const std::vector<double>& init) = 0;
};
