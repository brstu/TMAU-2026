#pragma once
class IModel{
    public:
        virtual double next_step(double yCurr, double yPrev, double u, double uPrev) = 0;
        virtual ~IModel() = default;
};