#pragma once

class Model 
{
private:
    double y_curr = 0.0;
    
public:
    virtual ~Model() = default;
    virtual double processStep(double u) = 0;
    virtual void resetState() = 0;

    double getY() const { return y_curr; }
    void setY(double value) { y_curr = value; }
};