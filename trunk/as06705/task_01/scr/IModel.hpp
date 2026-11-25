#pragma once
#include <string>
#include <vector>

class IModel {
public:
    virtual ~IModel() = default;

    virtual void        reset() = 0;
    virtual double      advance(double input) = 0;
    virtual const char* title() const = 0;

    virtual void setInitialState(const std::vector<double>&) {}
};