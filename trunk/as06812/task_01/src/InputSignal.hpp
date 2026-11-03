#pragma once
#include <string>

class InputSignal 
{
public:
    enum class Type { STEP, IMPULSE, HARMONIC };

    explicit InputSignal(Type type) : type_(type) {}
    
    double getValue(int t) const;
    std::string toString() const;
    Type getType() const { return type_; }

private:
    Type type_;
};