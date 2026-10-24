#ifndef MODEL_2_3_H
#define MODEL_2_3_H

#include "Model.h"

class Model2_3 : public Model {
public:
    Model2_3(double coeffA1, double coeffB1, double coeffC1)
        : a1(coeffA1), b1(coeffB1), c1(coeffC1) {}

    double calculateNext(double input) override {
        double current_y = getY();
        double next_y = a1 * current_y - b1 * (y_prev1 * y_prev1) + c1 * input;
        y_prev1 = current_y;
        setY(next_y);
        return next_y;
    }

    void reset() override {
        setY(0.0);
        y_prev1 = 0.0;
    }

private:
   double a1;
   double b1;
   double c1;
   double y_prev1 = 0.0;
};

#endif // MODEL_2_3_H