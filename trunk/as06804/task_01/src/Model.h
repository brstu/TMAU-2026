#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

class Model {
private:
    double y;
    double dt;

protected:
    double getDt() const { return dt; }

public:
    Model(double start_y, double step_size) : y(start_y), dt(step_size) {}
    virtual ~Model() = default;

    virtual void step(double u) = 0;
    
    double getY() const { return y; }
    void setY(double val) { y = val; }
};

#endif
