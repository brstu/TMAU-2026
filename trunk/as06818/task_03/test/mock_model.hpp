#ifndef TASK03_AS06818_MOCK_MODEL_HPP
#define TASK03_AS06818_MOCK_MODEL_HPP

#include <gmock/gmock.h>

#include "../src/model.hpp"

// Подменный объект управления для проверки взаимодействия
// замкнутой системы с моделью (значения u, порядок вызовов).
class MockModel : public Model {
public:
    MOCK_METHOD(double, step, (double u), (override));
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(double, get_y, (), (const, override));
};

#endif
