#pragma once
#include "IModel.hpp"
#include "InputSignals.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Trace {
    std::vector<double> u;
    std::vector<double> y;
};

class Runner {
public:
    Runner(std::unique_ptr<IModel> model, int steps)
        : model_(std::move(model)), steps_(steps) {
    }

    Trace execute(const ISignal& sig) {
        model_->reset();
        Trace tr;
        tr.u.reserve(steps_);
        tr.y.reserve(steps_);
        for (int t = 0; t < steps_; ++t) {
            const double u = sig.at(t);
            tr.u.push_back(u);
            tr.y.push_back(model_->advance(u));
        }
        return tr;
    }

    std::string title() const { return model_->title(); }

    static void show(const Trace& tr, std::ostream& out = std::cout) {
        out << std::setw(6) << "tau"
            << std::setw(16) << "u(tau)"
            << std::setw(18) << "y(tau)" << "\n"
            << std::string(40, '-') << "\n";

        for (size_t t = 0; t < tr.y.size(); ++t) {
            out << std::setw(6) << t;

            out << std::setw(16) << std::scientific
                << std::setprecision(4) << tr.u[t];

            const double y = tr.y[t];
            if (std::isnan(y)) {
                out << std::setw(18) << "nan";
            }
            else if (std::isinf(y)) {
                out << std::setw(18) << (y > 0 ? "+inf" : "-inf");
            }
            else {
                out << std::setw(18) << y;
            }
            out << "\n";
        }
    }

    static void dumpCsv(const Trace& tr, const std::string& path) {
        std::ofstream f(path);
        f << "tau,u,y\n";
        f << std::scientific << std::setprecision(10);
        for (size_t t = 0; t < tr.y.size(); ++t) {
            f << t << "," << tr.u[t] << "," << tr.y[t] << "\n";
        }
    }

    static bool diverged(const Trace& tr) {
        for (double v : tr.y) {
            if (!std::isfinite(v)) return true;
        }
        return false;
    }

private:
    std::unique_ptr<IModel> model_;
    int steps_;
};