#include "InputSignal.h"

#include <array>
#include <cmath>

namespace {

using SignalLaw = double (*)(int);

// Законы изменения входа; порядок элементов совпадает с порядком в InputType
constexpr std::array<SignalLaw, kInputTypeCount> kLaws{
    [](int) { return 1.0; },                                     // u(τ) = 1
    [](int tau) { return tau == 0 ? 1.0 : 0.0; },                // u(0) = 1, далее 0
    [](int tau) { return std::sin(static_cast<double>(tau)); }   // u(τ) = sin τ
};

} // namespace

double getInputValue(InputType type, int tau) {
    const auto index = static_cast<std::size_t>(type);
    return index < kLaws.size() ? kLaws[index](tau) : 0.0;
}
