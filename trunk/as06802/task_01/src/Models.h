#pragma once
#include "Model.h"
#include <cmath>

class Model1_7 : public Model {
private:
    double a;
    double b1;
    double b2;
    double b3;
    double y = 0.0;
    double u1 = 0.0;
    double u2 = 0.0;

public:
    Model1_7(double a, double b1, double b2, double b3) 
        : a(a), b1(b1), b2(b2), b3(b3) {}
        
    double step(double u) override {
        double y_next = a * y + b1 * u + b2 * u1 + b3 * u2;
        u2 = u1;
        u1 = u;
        y = y_next;
        return y;
    }
    
    void reset() override { 
        y = 0.0; 
        u1 = 0.0; 
        u2 = 0.0; 
    }
};

class Model2_1 : public Model {
private:
    double a;
    double b;
    double c;
    double d;
    double y = 0.0;
    double y_prev = 0.0;
    double u_prev = 0.0;

public:
    Model2_1(double a, double b, double c, double d) 
        : a(a), b(b), c(c), d(d) {}
        
    double step(double u) override {
        double y_next = a * y - b * (y_prev * y_prev) + c * u + d * std::sin(u_prev);
        y_prev = y;
        y = y_next;
        u_prev = u;
        return y;
    }
    
    void reset() override { 
        y = 0.0; 
        y_prev = 0.0; 
        u_prev = 0.0; 
    }
};

class Model3_5 : public Model {
private:
    double b;
    double dt;
    double y = 0.0;

public:
    Model3_5(double b, double dt) 
        : b(b), dt(dt) {}
        
    double step(double u) override {
        y = y + dt * b * std::sin(u);
        return y;
    }
    
    void reset() override { 
        y = 0.0; 
    }
};