#ifndef MODELS_H
#define MODELS_H

#include <vector>
#include <cmath>

class Model {
public:
    virtual ~Model() = default;
    virtual double calculateNext(double u) = 0;
    virtual void reset() = 0;

    std::vector<double> simulate(const std::vector<double>& inputSignal) {
        std::vector<double> outputSignal;
        outputSignal.reserve(inputSignal.size());
        for (double u : inputSignal) {
            outputSignal.push_back(calculateNext(u));
        }
        return outputSignal;
    }
};

class LinearModel1_4 : public Model {
private:
    double a;
    double b;
    double y_curr = 0.0;
    double u_prev = 0.0;

public:
    LinearModel1_4(double a_val, double b_val)
        : a(a_val), b(b_val) {}

    double calculateNext(double u) override {
        double y_next = a * y_curr + b * (u - u_prev);
        u_prev = u;
        y_curr = y_next;
        return y_next;
    }

    void reset() override {
        y_curr = 0.0;
        u_prev = 0.0;
    }
};

class NonlinearModel2_8 : public Model {
private:
    double a;
    double b;
    double c;
    double y_curr = 0.0;
    double y_prev = 0.0;

public:
    NonlinearModel2_8(double a_val, double b_val, double c_val)
        : a(a_val), b(b_val), c(c_val) {}

    double calculateNext(double u) override {
        double y_next = a * y_curr * (1.0 - y_curr) + b * u + c * std::sin(y_prev * u);
        y_prev = y_curr;
        y_curr = y_next;
        return y_next;
    }

    void reset() override {
        y_curr = 0.0;
        y_prev = 0.0;
    }
};

class DiffModel3_2 : public Model {
private:
    double b;
    double dt;
    double y_curr = 0.0;

public:
    explicit DiffModel3_2(double b_val, double dt_val = 0.1)
        : b(b_val), dt(dt_val) {}

    double calculateNext(double u) override {
        double y_next = y_curr + dt * b * u;
        y_curr = y_next;
        return y_next;
    }

    void reset() override {
        y_curr = 0.0;
    }
};

#endif