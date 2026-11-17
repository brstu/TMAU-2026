#pragma once

#include <string>

class Model {
public:
    virtual ~Model() = default;

    virtual double step(double u) = 0;
    virtual void reset() = 0;
    virtual std::string name() const = 0;
};