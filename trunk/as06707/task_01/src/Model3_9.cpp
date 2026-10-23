#include "Model3_9.h"
#include <cmath>

Model3_9::Model3_9(double b_val, double y0) : b(b_val), y_current(y0) {}

double Model3_9::step(double u_tau) {
    // Так как мы убрали dt из аргументов, считаем шаг интегрирования dt = 1
    y_current = y_current + b * std::tanh(u_tau);
    return y_current;
}

void Model3_9::reset(double y0) { 
    y_current = y0; 
}