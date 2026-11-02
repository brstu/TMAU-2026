#pragma once
#include <vector>

class Model
{
public:
    virtual std::vector<double> calculate(const std :: vector<double>& u) = 0;

    virtual ~Model() = default;
};
