#include "model.hpp"

class LinearModel : private IModel{
    void next_step() override;
};

class NoneLinearModel : private IModel{
    void next_step() override;
};

class SimpleDifferentialModel : private IModel{
    void next_step() override;
};