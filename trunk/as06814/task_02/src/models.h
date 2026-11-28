#ifndef MODELS_H
#define MODELS_H

#include <vector>
#include <cmath>

// Интерфейс для mock-объектов генераторов сигналов
class ISignalProvider {
public:
    virtual ~ISignalProvider() = default;
    virtual double getSignal(int t) const = 0;
};

class Model {
public:
    virtual ~Model() = default;
    virtual double calculateNext(double u) = 0;
    virtual void reset() = 0;

    std::vector<double> simulate(const std::vector<double>& inputSignal) {
        std::vector outputSignal(inputSignal.size(), 0.0);
        for (size_t i = 0; i < inputSignal.size(); ++i) {
            outputSignal[i] = calculateNext(inputSignal[i]);
        }
        return outputSignal;
    }

    std::vector<double> simulateWithProvider(const ISignalProvider& provider, int steps) {
        std::vector inputSignal(steps, 0.0);
        for (int t = 0; t < steps; ++t) {
            inputSignal[t] = provider.getSignal(t);
        }
        return simulate(inputSignal);
    }
};

class LinearModel1_4 : public Model {
private:
    double a;
    double b;
    double y_curr = 0.0;
    double u_prev = 0.0;

public:
    LinearModel1_4(double a_val, double b_val);
    ~LinearModel1_4() override = default;

    double calculateNext(double u) override;
    void reset() override;
    double getA() const { return a; }
    double getB() const { return b; }
};

class NonlinearModel2_8 : public Model {
private:
    double a;
    double b;
    double c;
    double y_curr = 0.0;
    double y_prev = 0.0;

public:
    NonlinearModel2_8(double a_val, double b_val, double c_val);
    ~NonlinearModel2_8() override = default;

    double calculateNext(double u) override;
    void reset() override;
};

class DiffModel3_2 : public Model {
private:
    double b;
    double dt;
    double y_curr = 0.0;

public:
    explicit DiffModel3_2(double b_val, double dt_val = 0.1);
    ~DiffModel3_2() override = default;

    double calculateNext(double u) override;
    void reset() override;
};

#endif // MODELS_H