#pragma once

#include <string>
#include <vector>

class Model {
public:
    virtual ~Model() = default;
    virtual std::string name() const = 0;
    virtual std::string formula() const = 0;
    virtual double nextStep(int tau,
                            const std::vector<double>& y,
                            const std::vector<double>& u,
                            double dt) const = 0;
};
