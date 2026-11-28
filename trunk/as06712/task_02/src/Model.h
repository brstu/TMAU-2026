#pragma once
#include <vector>

class Model {
public:
    virtual ~Model() = default;
    virtual double step(double u) = 0;
    virtual void reset() = 0;
    virtual const char* name() const = 0;
    virtual void setInitialState(const std::vector<double>& init) {}
};