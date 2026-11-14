#pragma once

#include <string>

class Model {
protected:
    double y_;
    double initialY_;

public:
    explicit Model(double initialY);
    virtual ~Model() = default;

    virtual double step(double u, double dt) = 0;
    virtual std::string name() const = 0;
    virtual void reset();

    double value() const;
};
