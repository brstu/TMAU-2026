#pragma once
#include <string>

enum class InputType {
    Step,
    Impulse,
    Harmonic
};

class Model {
protected:
    double alpha;
    std::string label;

public:
    Model(double alpha_val, const std::string& model_name)
        : alpha(alpha_val), label(model_name) {}

    virtual ~Model() = default;

    virtual double calculateStep(double u_current, double u_prev1, double u_prev2) = 0;
    virtual void reset() = 0;
    virtual void setInitial(double y0) = 0;

    std::string getName() const { return label; }
    double getAlpha() const { return alpha; }
};