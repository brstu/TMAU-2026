#ifndef MODEL_H
#define MODEL_H

class Model {
public:
    virtual ~Model() = default;

    virtual double calculateNext(
        double y,
        double yPrev,
        double u,
        double uPrev,
        double uPrev2
    ) = 0;
};

#endif