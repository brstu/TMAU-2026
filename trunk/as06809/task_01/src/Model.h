#pragma once

class Model {
private:
    double y_curr = 0.0; // Теперь private
protected:
    // Специальные методы доступа для наследников
    double getY() const { return y_curr; }
    void setY(double val) { y_curr = val; }
public:
    virtual ~Model() = default;
    virtual double calculateNext(double u) = 0;
    virtual void reset() = 0;
};