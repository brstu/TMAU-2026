#pragma once

class Model {
public:
    virtual ~Model() = default;

    double step(double u, double dt);
    virtual void reset();

protected:
    virtual double calculateNext(double u, double dt) = 0;

    double currentY() const;
    double previousY() const;
    double previousU() const;
    double previousU2() const;

private:
    double y_ = 0.0;
    double previousY_ = 0.0;
    double previousU_ = 0.0;
    double previousU2_ = 0.0;
};
