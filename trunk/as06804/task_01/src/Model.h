#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

class Model {
protected:
    double y;      // Текущее состояние
    double dt;     // Шаг времени
public:
    Model(double start_y, double step_size) : y(start_y), dt(step_size) {}
    virtual ~Model() = default;

    // Чисто виртуальный метод для расчета следующего шага
    virtual void step(double u) = 0;
    
    double getY() const { return y; }
    void setY(double val) { y = val; }
};

#endif