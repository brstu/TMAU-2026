#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <sstream>
#include <cmath>
#include <memory>
#include <algorithm>
#include <functional>
#include <map>
#include <cstdlib>
#include <exception>

// Абстрактный базовый класс
class Model {
public:
    virtual ~Model() = default;
    virtual void        reset()        = 0;
    virtual double      step(double u) = 0;
    virtual std::string name() const   = 0;
};

class Model15 : public Model {
public:
    Model15(double a1, double a2, double b, int k)
    : a1_(a1), a2_(a2), b_(b), k_(k) {}

    void reset() override { y_.assign(1, 0.0); }

    double step(double u) override {
        const std::size_t t   = y_.size() - 1;
        const double      y_t = y_[t];
        const double      y_tk = (t >= (std::size_t)k_)
        ? y_[t - (std::size_t)k_] : 0.0;
        const double y_next = a1_ * y_t + a2_ * y_tk + b_ * u;
        y_.push_back(y_next);
        return y_next;
    }

    std::string name() const override { return "Model 1.5"; }

private:
    double a1_;
    double a2_;
    double b_;
    int k_;
    std::vector<double> y_{0.0};
};

class Model27 : public Model {
public:
    Model27(double a, double b, double eps)
    : a_(a), b_(b), eps_(eps) {}

    void reset() override { y_prev_ = 0.0; }

    double step(double u) override {
        double r;
        if      (u >  eps_ || (u >= -eps_ && y_prev_ >  0)) r =  1.0;
        else if (u < -eps_ || (u <=  eps_ && y_prev_ <= 0)) r = -1.0;
        else                                                r =  0.0;

        const double y_next = a_ * y_prev_ + b_ * r;
        y_prev_ = y_next;
        return y_next;
    }

    std::string name() const override { return "Model 2.7"; }

private:
    double a_;
    double b_;
    double eps_;
    double y_prev_ = 0.0;
};

class Model39 : public Model {
public:
    Model39(double b, double dt)
    : b_(b), dt_(dt) {}

    void reset() override { y_ = 0.0; }

    double step(double u) override {
        y_ += dt_ * b_ * std::tanh(u);
        return y_;
    }

    std::string name() const override { return "Model 3.9"; }

private:
    double b_;
    double dt_;
    double y_ = 0.0;
};

// Входные воздействия
enum class InputType { Step, Impulse, Harmonic };

const char* inputName(InputType t) {
    switch (t) {
        case InputType::Step:     return "step";
        case InputType::Impulse:  return "impulse";
        case InputType::Harmonic: return "harmonic";
    }
    return "?";
}

double genInput(InputType t, int tau) {
    switch (t) {
        case InputType::Step:     return 1.0;
        case InputType::Impulse:  return (tau == 0) ? 1.0 : 0.0;
        case InputType::Harmonic: return std::sin((double)tau);
    }
    return 0.0;
}

// Конфигурация
struct Config {
    std::set<std::string, std::less<>> models = {"15", "27", "39"};

    int         n       = 50;
    std::string inputs  = "all";        // step|impulse|harmonic|all
    std::string outFile = "output.csv";

    double m15_a1 = 0.6;
    double m15_a2 = 0.2;
    double m15_b  = 0.5;
    int    m15_k  = 2;

    double m27_a   = 0.7;
    double m27_b   = 0.4;
    double m27_eps = 0.1;

    double m39_b  = 1.0;
    double m39_dt = 0.1;
};

static void printHelp() {
    std::cout <<
    R"(Usage: app [options]

Common:
  --models 15,27,39          list of models to run (default: all)
  --n 50                     number of simulation steps
  --input step|impulse|harmonic|all   (default: all)
  --out output.csv           output CSV path

Model 1.5 parameters:
  --m15-a1  0.6
  --m15-a2  0.2
  --m15-b   0.5
  --m15-k   2

Model 2.7 parameters:
  --m27-a   0.7
  --m27-b   0.4
  --m27-eps 0.1

Model 3.9 parameters:
  --m39-b   1.0
  --m39-dt  0.1

  --help  -h                 show this help
)";
}

static void parseModelsArg(Config& c, std::string_view v) {
    c.models.clear();
    std::stringstream ss{std::string(v)};
    std::string tok;

    while (std::getline(ss, tok, ',')) {
        if (!tok.empty()) {
            c.models.insert(tok);
        }
    }
}

static Config parseArgs(int argc, char** argv) {
    Config c;

    using Setter = std::function<void(Config&, std::string_view)>;

    static const std::map<std::string, Setter, std::less<>> setters = {
        {"--n",       [](Config& c, std::string_view v){ c.n        = std::stoi(std::string(v)); }},
        {"--input",   [](Config& c, std::string_view v){ c.inputs   = std::string(v); }},
        {"--out",     [](Config& c, std::string_view v){ c.outFile  = std::string(v); }},
        {"--m15-a1",  [](Config& c, std::string_view v){ c.m15_a1   = std::stod(std::string(v)); }},
        {"--m15-a2",  [](Config& c, std::string_view v){ c.m15_a2   = std::stod(std::string(v)); }},
        {"--m15-b",   [](Config& c, std::string_view v){ c.m15_b    = std::stod(std::string(v)); }},
        {"--m15-k",   [](Config& c, std::string_view v){ c.m15_k    = std::stoi(std::string(v)); }},
        {"--m27-a",   [](Config& c, std::string_view v){ c.m27_a    = std::stod(std::string(v)); }},
        {"--m27-b",   [](Config& c, std::string_view v){ c.m27_b    = std::stod(std::string(v)); }},
        {"--m27-eps", [](Config& c, std::string_view v){ c.m27_eps  = std::stod(std::string(v)); }},
        {"--m39-b",   [](Config& c, std::string_view v){ c.m39_b    = std::stod(std::string(v)); }},
        {"--m39-dt",  [](Config& c, std::string_view v){ c.m39_dt   = std::stod(std::string(v)); }},
    };

    int i = 1;
    while (i < argc) {
        const std::string_view a = argv[i];

        if (a == "--help" || a == "-h") {
            printHelp();
            std::exit(0);
        }

        if (i + 1 >= argc) {
            std::cerr << "Missing value after " << a << "\n";
            std::exit(1);
        }

        const std::string_view v = argv[i + 1];

        try {
            if (a == "--models") {
                parseModelsArg(c, v);
            } else {
                const auto it = setters.find(a);
                if (it == setters.end()) {
                    std::cerr << "Unknown argument: " << a << "\n";
                    std::exit(1);
                }
                it->second(c, v);
            }
        } catch (const std::exception& e) {
            std::cerr << "Bad value for " << a << ": " << e.what() << "\n";
            std::exit(1);
        }

        i += 2;
    }

    if (c.n <= 0) {
        std::cerr << "--n must be > 0\n";
        std::exit(1);
    }

    return c;
}

// main
int main(int argc, char** argv) {
    Config cfg = parseArgs(argc, argv);

    std::vector<InputType> inputs;

    if      (cfg.inputs == "step")     inputs = {InputType::Step};
    else if (cfg.inputs == "impulse")  inputs = {InputType::Impulse};
    else if (cfg.inputs == "harmonic") inputs = {InputType::Harmonic};
    else if (cfg.inputs == "all")      inputs = {InputType::Step, InputType::Impulse, InputType::Harmonic};
    else {
        std::cerr << "Unknown --input: " << cfg.inputs << "\n";
        return 1;
    }

    std::ofstream fout(cfg.outFile);
    if (!fout) {
        std::cerr << "Cannot open " << cfg.outFile << "\n";
        return 1;
    }

    fout << "model,input,tau,u,y\n";
    fout << std::fixed << std::setprecision(6);

    for (InputType it : inputs) {
        // создаём только выбранные модели для каждого прохода
        std::vector<std::unique_ptr<Model>> models;

        if (cfg.models.count("15")) {
            models.push_back(std::make_unique<Model15>(cfg.m15_a1, cfg.m15_a2, cfg.m15_b, cfg.m15_k));
        }
        if (cfg.models.count("27")) {
            models.push_back(std::make_unique<Model27>(cfg.m27_a, cfg.m27_b, cfg.m27_eps));
        }
        if (cfg.models.count("39")) {
            models.push_back(std::make_unique<Model39>(cfg.m39_b, cfg.m39_dt));
        }

        for (const auto& m : models) {
            m->reset();

            for (int tau = 0; tau < cfg.n; ++tau) {
                const double u = genInput(it, tau);
                const double y = m->step(u);

                fout << m->name() << "," << inputName(it) << ","
                << (tau + 1) << "," << u << "," << y << "\n";
            }
        }
    }

    std::cerr << "Done. Models:";
    for (const auto& m : cfg.models) {
        std::cerr << " " << m;
    }
    std::cerr << " | n=" << cfg.n << " | inputs=" << cfg.inputs
    << " | out=" << cfg.outFile << "\n";

    return 0;
}
