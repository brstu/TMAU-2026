#ifndef MODEL_H
#define MODEL_H

#include <cstddef>
#include <string>
#include <vector>

class Model {
public:
    virtual ~Model() = default;
    virtual std::string name() const = 0;
    virtual double nextStep(std::size_t tau,
                            const std::vector<double>& y,
                            const std::vector<double>& u,
                            double dt) const = 0;
    virtual std::string stabilityInfo() const;
};

class LinearModel12 final : public Model {
private:
    double a_;
    double b_;
    std::size_t delay_;

public:
    LinearModel12(double a, double b, std::size_t delay);
    std::string name() const override;
    double nextStep(std::size_t tau,
                    const std::vector<double>& y,
                    const std::vector<double>& u,
                    double dt) const override;
    std::string stabilityInfo() const override;
};

class NonlinearModel26 final : public Model {
private:
    double a1_;
    double a2_;
    double b_;

public:
    NonlinearModel26(double a1, double a2, double b);
    std::string name() const override;
    double nextStep(std::size_t tau,
                    const std::vector<double>& y,
                    const std::vector<double>& u,
                    double dt) const override;
};

class DifferentialModel310 final : public Model {
private:
    double a_;
    double b_;

public:
    DifferentialModel310(double a, double b);
    std::string name() const override;
    double nextStep(std::size_t tau,
                    const std::vector<double>& y,
                    const std::vector<double>& u,
                    double dt) const override;
};

#endif
