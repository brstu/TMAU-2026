#include "model_realizations.hpp"

// Реализация методов линейной модели
LinearModel::LinearModel(double a1_, double a2_, double b1_, double b2_) :
    a1(a1_), a2(a2_), b1(b1_), b2(b2_) {}

double LinearModel::next_step(double yCurr, double yPrev, double u, double uPrev){ 
    return a1 * yCurr + a2 * yPrev + b1 * u + b2 * uPrev; 
}

// Реализация методов нелинейной модели 
NoneLinearModel::NoneLinearModel(double a_, double b_, double uMin_, double uMax_): 
    a(a_), b(b_), uMin(uMin_), uMax(uMax_) {}

double NoneLinearModel::next_step(double yCurr, double /*yPrev*/, double u, double /*uPrev*/){ 
    return a * yCurr + b * sat(u); 
}

double NoneLinearModel::sat(double u) const {
    if (u > uMax) return uMax;
    else if (u < uMin) return uMin;
    return u;
}

// Реализация методов дифферинциальной модели
SimpleDifferentialModel::SimpleDifferentialModel(double a_, double b_, double dt_):
    a(a_), b(b_), dt(dt_) {}

double SimpleDifferentialModel::next_step(double yCurr, double /*yPrev*/, double u, double /*uPrev*/){
    return yCurr + dt *(a * yCurr * yCurr * yCurr + b * u); 
}