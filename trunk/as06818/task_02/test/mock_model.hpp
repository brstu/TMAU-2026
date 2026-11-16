#ifndef TASK02_AS06818_MOCK_MODEL_HPP
#define TASK02_AS06818_MOCK_MODEL_HPP

#include <gmock/gmock.h>

#include "../src/model.hpp"

// Подмена реальной модели при тестировании симулятора:
// gmock проверяет вызовы step()/reset() и значения подаваемых воздействий.
class MockModel : public Model {
public:
    MOCK_METHOD(double, step, (double u), (override));
    MOCK_METHOD(void, reset, (), (override));
};

#endif
