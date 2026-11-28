#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <complex>
#include <memory>
#include <string>
#include <vector>

#include "Model.h"
#include "Simulator.h"

namespace {

using ::testing::Return;

// Mock-модель управляемого объекта (Google Mock).
class MockModel : public Model {
public:
    MOCK_METHOD(double, step, (double u), (override));
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(std::string, name, (), (const, override));
    MOCK_METHOD(std::vector<std::complex<double>>, poles, (), (const, override));
    MOCK_METHOD(std::string, equation, (), (const, override));
    MOCK_METHOD(void, setInitialState, (const std::vector<double>& init), (override));
};

// Mock-источник входного сигнала (подмена входных воздействий).
class MockSignalSource : public ISignalSource {
public:
    MOCK_METHOD(double, get, (double tau), (const, override));
};

}  // namespace

// Симулятор вызывает step() модели ровно по одному разу на каждый шаг.
TEST(MockModelTest, SimulatorCallsStepForEveryStep) {
    auto model = std::make_unique<MockModel>();
    MockModel* rawModel = model.get();

    EXPECT_CALL(*rawModel, reset()).Times(1);
    EXPECT_CALL(*rawModel, step(::testing::_))
        .Times(3)
        .WillOnce(Return(1.0))
        .WillOnce(Return(2.0))
        .WillOnce(Return(3.0));

    Simulator simulator(std::move(model), 3);
    const std::vector<double> history = simulator.run(::testing::NiceMock<MockSignalSource>());

    ASSERT_EQ(3u, history.size());
    EXPECT_DOUBLE_EQ(1.0, history[0]);
    EXPECT_DOUBLE_EQ(2.0, history[1]);
    EXPECT_DOUBLE_EQ(3.0, history[2]);
}

// Модель сбрасывается перед началом симуляции.
TEST(MockModelTest, SimulatorResetsModelBeforeSimulation) {
    auto model = std::make_unique<MockModel>();
    MockModel* rawModel = model.get();

    EXPECT_CALL(*rawModel, reset()).Times(1);
    EXPECT_CALL(*rawModel, step(::testing::_)).Times(2).WillRepeatedly(Return(0.0));

    Simulator simulator(std::move(model), 2);
    simulator.run(::testing::NiceMock<MockSignalSource>());
}

// Симулятор запрашивает входной сигнал для каждого шага (начиная с tau = 0).
TEST(MockModelTest, SimulatorQueriesSignalForEveryStep) {
    auto model = std::make_unique<MockModel>();
    EXPECT_CALL(*model, reset()).Times(1);
    EXPECT_CALL(*model, step(::testing::_)).Times(3).WillRepeatedly(Return(0.0));

    MockSignalSource signal;
    EXPECT_CALL(signal, get(::testing::_))
        .Times(3)
        .WillOnce(Return(0.5))
        .WillOnce(Return(1.5))
        .WillOnce(Return(2.5));

    Simulator simulator(std::move(model), 3);
    simulator.run(signal);
}

// Подмена входного сигнала mock-объектом: значения сигнала передаются в модель.
TEST(MockModelTest, MockSignalValuesArePassedToModel) {
    auto model = std::make_unique<MockModel>();
    EXPECT_CALL(*model, reset()).Times(1);
    EXPECT_CALL(*model, step(0.5)).Times(1);
    EXPECT_CALL(*model, step(1.5)).Times(1);
    EXPECT_CALL(*model, step(2.5)).Times(1);

    MockSignalSource signal;
    EXPECT_CALL(signal, get(0.0)).WillOnce(Return(0.5));
    EXPECT_CALL(signal, get(1.0)).WillOnce(Return(1.5));
    EXPECT_CALL(signal, get(2.0)).WillOnce(Return(2.5));

    Simulator simulator(std::move(model), 3);
    const std::vector<double> history = simulator.run(signal);

    ASSERT_EQ(3u, history.size());
}

// Порядок вызовов: сначала reset, затем шаги.
TEST(MockModelTest, ResetIsCalledBeforeFirstStep) {
    auto model = std::make_unique<MockModel>();
    MockModel* rawModel = model.get();

    {
        ::testing::InSequence sequence;
        EXPECT_CALL(*rawModel, reset()).Times(1);
        EXPECT_CALL(*rawModel, step(::testing::_)).Times(1);
    }

    Simulator simulator(std::move(model), 1);
    simulator.run(::testing::NiceMock<MockSignalSource>());
}

// Mock-модель позволяет проверить получение имени и полюсов.
TEST(MockModelTest, MockModelProvidesNameAndPoles) {
    MockModel model;
    EXPECT_CALL(model, name()).WillOnce(Return("Mocked model"));
    EXPECT_CALL(model, poles())
        .WillOnce(Return(std::vector<std::complex<double>>{std::complex<double>(0.5, 0.0)}));

    EXPECT_EQ("Mocked model", model.name());
    ASSERT_EQ(1u, model.poles().size());
}
