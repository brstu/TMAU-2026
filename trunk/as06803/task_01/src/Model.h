#ifndef MODEL_H
#define MODEL_H

#include <complex>
#include <string>

enum class InputSignal {
    Step = 1,
    Impulse = 2,
    Harmonic = 3
};

class Model {
public:
    virtual ~Model() = default;

    virtual double nextStep(double yCurrent,
                            double yPrevious,
                            double uCurrent,
                            double uPrevious,
                            double deltaT) const = 0;

    virtual std::string name() const = 0;
    virtual bool isStable() const;
    virtual std::string stabilityInfo() const;
};

class LinearModel18 : public Model {
private:
    double a1;
    double a2;
    double b1;
    double b2;

public:
    LinearModel18(double a1, double a2, double b1, double b2);

    double nextStep(double yCurrent,
                    double yPrevious,
                    double uCurrent,
                    double uPrevious,
                    double deltaT) const override;

    std::string name() const override;
    bool isStable() const override;
    std::string stabilityInfo() const override;
};

class NonlinearModel210 : public Model {
private:
    double a;
    double b;

public:
    NonlinearModel210(double a, double b);

    double nextStep(double yCurrent,
                    double yPrevious,
                    double uCurrent,
                    double uPrevious,
                    double deltaT) const override;

    std::string name() const override;
};

class DifferentialModel32 : public Model {
private:
    double b;

public:
    explicit DifferentialModel32(double b);

    double nextStep(double yCurrent,
                    double yPrevious,
                    double uCurrent,
                    double uPrevious,
                    double deltaT) const override;

    std::string name() const override;
};

double getInputValue(InputSignal signal, int tau, double stepAmplitude);
std::string getInputSignalName(InputSignal signal);

#endif
