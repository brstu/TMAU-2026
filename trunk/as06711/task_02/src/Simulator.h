#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "ISignalGenerator.h"
#include "Model.h"

// Драйвер моделирования: удерживает одну модель и прогоняет её по заданному
// горизонту, снимая выход на каждом такте. Источник входа передаётся снаружи,
// поэтому один и тот же экземпляр можно переиспользовать под разные сигналы.
class Simulator {
public:
    Simulator(std::unique_ptr<Model> target, int horizon)
        : target_(std::move(target)), horizon_(horizon) {}

    std::vector<double> run(const ISignalGenerator& feed) const {
        std::vector<double> trace;
        if (horizon_ <= 0) {
            return trace;
        }

        trace.reserve(static_cast<std::size_t>(horizon_));
        int tick = 0;
        while (tick < horizon_) {
            const double sample = feed.get(static_cast<double>(tick));
            trace.emplace_back(target_->step(sample));
            ++tick;
        }
        return trace;
    }

    std::size_t horizon() const noexcept {
        return horizon_ > 0 ? static_cast<std::size_t>(horizon_) : 0u;
    }

    Model* target() const noexcept { return target_.get(); }

private:
    std::unique_ptr<Model> target_;
    int horizon_;
};