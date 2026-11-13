#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

class Model {
private:
    double y;      // Текущее состояние
    double dt;     // Шаг времени

protected:
    // Защищенный геттер, чтобы наследники могли узнать шаг dt
    double getDt() const { return dt; }

public:
    Model(double start_y, double step_size) : y(start_y), dt(step_size) {}
    virtual ~Model() = default;

    // Чисто виртуальный метод для расчета следующего шага
    virtual void step(double u) = 0;
    
    double getY() const { return y; }
    void setY(double val) { y = val; }
};

#endif
