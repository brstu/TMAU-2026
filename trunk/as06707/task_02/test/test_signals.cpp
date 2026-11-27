#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Model.h"

// Использование Mock-объектов для подмены входных сигналов
class MockModel : public Model {
public:
    MOCK_METHOD(double, step, (double u_tau), (override));
    MOCK_METHOD(double, getY, (), (const, override));
    MOCK_METHOD(void, reset, (double y0), (override));
    MOCK_METHOD(std::string, getName, (), (const, override));
};

TEST(MockTests, SimulationLoopWithMock) {
    MockModel mockModel;
    
    // Ожидаем 3 вызова метода step
    EXPECT_CALL(mockModel, step(::testing::_))
        .Times(3)
        .WillRepeatedly(::testing::Return(42.0));

    // Имитация цикла симуляции
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(mockModel.step(1.0), 42.0);
    }
}