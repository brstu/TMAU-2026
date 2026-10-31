#ifndef MODEL_1_9_H
#define MODEL_1_9_H

#include "Model.h"

class Model1_9 : public Model {
public:
    Model1_9(double coeffA1, double coeffA2, double coeffA3, double coeffB1)
        : a1(coeffA1), a2(coeffA2), a3(coeffA3), b1(coeffB1) {}

    double calculateNext(double input) override {
        double current_y = getY();
        double next_y = a1 * current_y + a2 * y_prev1 + a3 * y_prev2 + b1 * input;
        y_prev2 = y_prev1;
        y_prev1 = current_y;
        setY(next_y);
        return next_y;
    }

    void reset() override {
        setY(0.0);
        y_prev1 = 0.0;
        y_prev2 = 0.0;
    }

private:
   double a1;
   double a2;
   double a3;
   double b1;
   double y_prev1 = 0.0;
   double y_prev2 = 0.0;
};

#endif // MODEL_1_9_H