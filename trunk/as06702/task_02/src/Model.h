#pragma once

#include <string>

class Model {
private:
    double y_;
    double initialY_;

protected:
    // Добавляем методы для доступа наследников
    double getY() const { return y_; }
    void setY(double val) { y_ = val; }

public:
    explicit Model(double initialY);
    virtual ~Model() = default;

    virtual double step(double u, double dt) = 0;
    virtual std::string name() const = 0;
    virtual void reset();

    double value() const;
};
