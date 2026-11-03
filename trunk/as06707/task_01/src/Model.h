#pragma once
#include <string>

class Model {
public:
    virtual ~Model() = default;
    
    // Виртуальный метод для расчета следующего шага симуляции
    virtual double step(double u_tau) = 0;
    
    // Получение текущего значения выхода
    virtual double getY() const = 0;
    
    // Сброс состояния модели в начальное (передаем y0)
    virtual void reset(double y0) = 0;
    
    virtual std::string getName() const = 0;
};