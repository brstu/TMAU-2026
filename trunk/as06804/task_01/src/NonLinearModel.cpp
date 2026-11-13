#include "NonLinearModel.h"
#include <cmath>

NonLinearModel::NonLinearModel(double a1, double a2, double b, double start_y, double dt)
    : Model(start_y, dt), a1(a1), a2(a2), b(b), y_prev(start_y), first_step(true) {}

void NonLinearModel::step(double u) {
    double current_y = getY(); // Получаем текущее значение y

    if (first_step) {
        y_prev = current_y; // Запоминаем начальное состояние
        first_step = false;
    }

    // Формула: y(t+1) = a1*y^3 - a2*y(t-1) + b*u^2
    // (Используем std::pow для возведения в куб и квадрат)
    double y_next = a1 * std::pow(current_y, 3) - a2 * y_prev + b * std::pow(u, 2);
    
    setY(y_next); // Устанавливаем новое значение через метод базового класса
    y_prev = current_y; // Обновляем предыдущее значение для следующего шага
}
