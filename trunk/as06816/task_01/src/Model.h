#ifndef MODEL_H
#define MODEL_H

#include <string>

// Базовый абстрактный класс объекта управления
class Model {
private:
    double y = 0.0;   // текущий выход

protected:
    // Наследники меняют выход только через этот метод
    void setOutput(double value) {
        y = value;
    }

public:
    virtual ~Model() = default;

    // Расчёт следующего шага: по текущему входу возвращает следующее значение выхода
    virtual double step(double u) = 0;

    // Сброс к начальным условиям
    virtual void reset() = 0;

    // Название модели для вывода
    virtual std::string name() const = 0;

    double output() const { return y; }
};

#endif
