#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <vector>

#include "../src/control_system.hpp"
#include "../src/linear_model.hpp"
#include "../src/pid.hpp"
#include "mock_model.hpp"

using ::testing::_;
using ::testing::Return;

// Объект с заранее заданной последовательностью выходов: step(u)
// возвращает очередной элемент списка независимо от u. Нужен для
// управляемых проверок режима Б (сходимость/несходимость ошибки).
class ScriptedModel : public Model {
public:
    explicit ScriptedModel(std::vector<double> outputs)
        : outputs(std::move(outputs)) {}

    double step(double /*u*/) override {
        const double y = outputs[index];
        index = (index + 1) % outputs.size();
        y_curr = y;
        return y;
    }

    void reset() override {
        index = 0;
        y_curr = 0.0;
    }

    double get_y() const override {
        return y_curr;
    }

private:
    std::vector<double> outputs;
    std::size_t index = 0;
    double y_curr = 0.0;
};

// Такт контура: e = w - y, u рассчитано регулятором и передано модели,
// точка содержит согласованное состояние (w=2.5, y0=0 -> e=2.5 -> u=2.5).
TEST(ControlSystemTest, SingleStepPassesPidOutputToPlant) {
    MockModel plant;
    PidController pid(1.0, 0.0, 0.0, 1.0, -10.0, 10.0);

    EXPECT_CALL(plant, get_y()).WillOnce(Return(0.0));
    EXPECT_CALL(plant, step(2.5)).WillOnce(Return(0.3));

    ControlSystem system(plant, pid);
    const ControllerPoint point = system.step(2.5);

    EXPECT_EQ(point.tau, 0);
    EXPECT_DOUBLE_EQ(point.w, 2.5);
    EXPECT_DOUBLE_EQ(point.e, 2.5);
    EXPECT_DOUBLE_EQ(point.u, 2.5);
    EXPECT_DOUBLE_EQ(point.y, 0.3);
}

// Номера тактов идут подряд с нуля.
TEST(ControlSystemTest, TauIncrements) {
    MockModel plant;
    PidController pid(0.1, 0.0, 0.0, 1.0);

    EXPECT_CALL(plant, get_y()).WillOnce(Return(0.0));
    EXPECT_CALL(plant, step(_)).Times(5).WillRepeatedly(Return(1.0));

    ControlSystem system(plant, pid);
    for (int i = 0; i < 5; ++i) {
        const ControllerPoint point = system.step(2.0);
        EXPECT_EQ(point.tau, i);
    }
}

// Режим А возвращает ровно n точек.
TEST(RunFixedStepsTest, ReturnsExactCount) {
    MockModel plant;
    PidController pid(0.5, 0.0, 0.0, 1.0);

    EXPECT_CALL(plant, get_y()).WillOnce(Return(0.0));
    EXPECT_CALL(plant, step(_)).Times(25).WillRepeatedly(Return(1.0));

    ControlSystem system(plant, pid);
    const std::vector<ControllerPoint> points =
        run_fixed_steps(system, 2.0, 25);

    ASSERT_EQ(points.size(), 25u);
    EXPECT_EQ(points.front().tau, 0);
    EXPECT_EQ(points.back().tau, 24);
}

// Замкнутый контур на линейной модели 1.8: итоговые настройки ЛР3
// выводят y на задание w = 25.
TEST(RunFixedStepsTest, ClosedLoopReachesSetpoint) {
    LinearModel18 plant;
    PidController pid(1.0, 0.5, 1.0, 1.0, -100.0, 100.0);
    ControlSystem system(plant, pid);

    const std::vector<ControllerPoint> points =
        run_fixed_steps(system, 25.0, 60);

    ASSERT_FALSE(points.empty());
    EXPECT_NEAR(points.back().y, 25.0, 0.01);
    EXPECT_NEAR(points.back().e, 0.0, 0.01);
}

// Режим Б: процесс сходится — симуляция заканчивается раньше
// ограничения, последние ошибки ниже eps.
TEST(RunUntilSteadyTest, StopsWhenErrorBelowEps) {
    LinearModel18 plant;
    PidController pid(1.0, 0.5, 1.0, 1.0, -100.0, 100.0);
    ControlSystem system(plant, pid);

    const std::vector<ControllerPoint> points =
        run_until_steady(system, 25.0, 0.5);

    EXPECT_LT(points.size(), 100u);
    EXPECT_GE(points.size(), 10u);
    for (std::size_t i = points.size() - 3; i < points.size(); ++i) {
        EXPECT_LT(std::abs(points[i].e), 0.5);
    }
}

// Режим Б с высокой точностью: финальные ошибки ниже eps = 0.01,
// симуляция завершается до ограничения по max_steps.
TEST(RunUntilSteadyTest, TightEpsilonEndsBelowEps) {
    LinearModel18 plant;
    PidController pid(1.0, 0.5, 1.0, 1.0, -100.0, 100.0);
    ControlSystem system(plant, pid);

    const std::vector<ControllerPoint> points =
        run_until_steady(system, 25.0, 0.01);

    EXPECT_LT(points.size(), 100u);
    EXPECT_LT(std::abs(points.back().e), 0.01);
}

// Режим Б при несходящемся процессе останавливается по max_steps.
TEST(RunUntilSteadyTest, MaxStepsLimitProtectsFromInfiniteLoop) {
    ScriptedModel plant({0.0, 0.0, 0.0});
    PidController pid(1.0, 0.0, 0.0, 1.0);
    ControlSystem system(plant, pid);

    const std::vector<ControllerPoint> points =
        run_until_steady(system, 2.5, 0.1, 3, 50);

    ASSERT_EQ(points.size(), 50u);
}

// Окно устойчивости: при window = 3 трёх тактов ниже eps не набрать
// (ошибка периодически возвращается к 2.5), а при window = 2 процесс
// признаётся установившимся на 4-м такте.
TEST(RunUntilSteadyTest, WindowRequiresConsecutiveSteps) {
    // Выходы объекта: 0, 2.4, 2.5 (циклически). Ошибка e = w - y при
    // w = 2.5: 2.5, 2.5, 0.1, 0.0, 2.5, ... (y берётся с запаздыванием).
    ScriptedModel plant({0.0, 2.4, 2.5});

    PidController pid(0.1, 0.0, 0.0, 1.0);
    ControlSystem system(plant, pid);
    const std::vector<ControllerPoint> points =
        run_until_steady(system, 2.5, 0.2, 3, 12);
    ASSERT_EQ(points.size(), 12u);

    plant.reset();
    PidController pid2(0.1, 0.0, 0.0, 1.0);
    ControlSystem system2(plant, pid2);
    const std::vector<ControllerPoint> points2 =
        run_until_steady(system2, 2.5, 0.2, 2, 12);
    ASSERT_EQ(points2.size(), 4u);
}

// reset возвращает контур в исходное состояние: повторный прогон
// повторяет траекторию первого.
TEST(ControlSystemTest, ResetRepeatsTrajectory) {
    LinearModel18 plant;
    PidController pid(1.0, 0.5, 1.0, 1.0, -100.0, 100.0);
    ControlSystem system(plant, pid);

    const std::vector<ControllerPoint> first =
        run_fixed_steps(system, 25.0, 40);
    system.reset();
    const std::vector<ControllerPoint> second =
        run_fixed_steps(system, 25.0, 40);

    ASSERT_EQ(first.size(), second.size());
    for (std::size_t i = 0; i < first.size(); ++i) {
        EXPECT_DOUBLE_EQ(first[i].e, second[i].e);
        EXPECT_DOUBLE_EQ(first[i].u, second[i].u);
        EXPECT_DOUBLE_EQ(first[i].y, second[i].y);
    }
}

// Открытый контур: модель 1.8 при u = 1 повторяет ряд из тестов модели.
TEST(RunOpenLoopTest, LinearModelTrajectory) {
    LinearModel18 plant;
    const std::vector<SimulationPoint> points = run_open_loop(plant, 1.0, 3);

    ASSERT_EQ(points.size(), 3u);
    EXPECT_NEAR(points[0].y, 0.3, 1e-12);
    EXPECT_NEAR(points[1].y, 0.55, 1e-12);
    EXPECT_NEAR(points[2].y, 0.735, 1e-12);
    for (const SimulationPoint& point : points) {
        EXPECT_DOUBLE_EQ(point.u, 1.0);
    }
}
