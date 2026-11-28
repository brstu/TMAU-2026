#include <gtest/gtest.h>

#include "LinearModel13.h"
#include "SignalGenerator.h"
#include "Simulator.h"

#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

namespace {

struct SimulationParams {
    SignalType type;
    double amplitude;
    int steps;
    double expectedFirst;
};

class SimulatorParamTest : public ::testing::TestWithParam<SimulationParams> {};

// Симуляция всегда возвращает историю заданной длины и корректное первое значение.
TEST_P(SimulatorParamTest, SimulationProducesExpectedHistory) {
    const SimulationParams p = GetParam();
    auto model = std::make_unique<LinearModel13>(0.5, 0.1, 1.0);
    Simulator simulator(std::move(model), p.steps);

    const SignalGenerator signal(p.type, p.amplitude);
    const std::vector<double> history = simulator.run(signal);

    ASSERT_EQ(static_cast<size_t>(p.steps), history.size());
    EXPECT_NEAR(p.expectedFirst, history.front(), 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    ForDifferentSignals,
    SimulatorParamTest,
    ::testing::Values(
        // Ступенчатое воздействие: u = 1 => y(1) = b*u = 1.
        SimulationParams{SignalType::STEP, 1.0, 5, 1.0},
        SimulationParams{SignalType::STEP, 2.0, 5, 2.0},
        // Импульсное воздействие: u(0) = 1 => y(1) = 1.
        SimulationParams{SignalType::IMPULSE, 1.0, 5, 1.0},
        SimulationParams{SignalType::IMPULSE, 3.0, 5, 3.0},
        // Гармоническое воздействие: u(0) = 0 => y(1) = 0.
        SimulationParams{SignalType::HARMONIC, 1.0, 5, 0.0}));

}  // namespace

// Модель сбрасывается перед каждым прогоном — результаты повторяемы.
TEST(SimulatorTest, RepeatedRunsGiveSameResult) {
    auto model = std::make_unique<LinearModel13>(0.5, 0.1, 1.0);
    Simulator simulator(std::move(model), 10);
    const SignalGenerator signal(SignalType::STEP, 1.0);

    const std::vector<double> first = simulator.run(signal);
    const std::vector<double> second = simulator.run(signal);

    ASSERT_EQ(first.size(), second.size());
    for (size_t i = 0; i < first.size(); ++i) {
        EXPECT_DOUBLE_EQ(first[i], second[i]);
    }
}

// Экспорт в CSV: заголовок + одна строка на каждый шаг.
TEST(SimulatorTest, ExportCsvWritesHeaderAndAllRows) {
    auto model = std::make_unique<LinearModel13>(0.5, 0.1, 1.0);
    Simulator simulator(std::move(model), 7);
    const SignalGenerator signal(SignalType::STEP, 1.0);
    const std::vector<double> history = simulator.run(signal);

    const std::string path = "simulator_test_export.csv";
    simulator.exportCSV(history, signal, path);

    std::ifstream input(path);
    ASSERT_TRUE(input.is_open());

    std::string line;
    int lineCount = 0;
    bool headerOk = false;
    while (std::getline(input, line)) {
        if (lineCount == 0) {
            headerOk = (line == "tau,u,y");
        }
        ++lineCount;
    }
    input.close();
    std::remove(path.c_str());

    EXPECT_TRUE(headerOk);
    EXPECT_EQ(8, lineCount);  // заголовок + 7 шагов
}

// Экспорт в недоступный путь корректно обрабатывается (сообщение в stderr).
TEST(SimulatorTest, ExportCsvToInvalidPathReportsError) {
    auto model = std::make_unique<LinearModel13>(0.5, 0.1, 1.0);
    Simulator simulator(std::move(model), 3);
    const SignalGenerator signal(SignalType::STEP, 1.0);
    const std::vector<double> history = simulator.run(signal);

    ::testing::internal::CaptureStderr();
    simulator.exportCSV(history, signal, "no_such_directory_\\missing\\file.csv");
    const std::string err = ::testing::internal::GetCapturedStderr();

    EXPECT_NE(std::string::npos, err.find("Не удалось открыть файл"));
}

// Печать таблицы результатов не приводит к ошибкам.
TEST(SimulatorTest, PrintTableProducesOutput) {
    auto model = std::make_unique<LinearModel13>(0.5, 0.1, 1.0);
    Simulator simulator(std::move(model), 4);
    const SignalGenerator signal(SignalType::STEP, 1.0);
    const std::vector<double> history = simulator.run(signal);

    ::testing::internal::CaptureStdout();
    simulator.printTable(history, signal);
    const std::string out = ::testing::internal::GetCapturedStdout();

    EXPECT_NE(std::string::npos, out.find("Model 1.3"));
    EXPECT_NE(std::string::npos, out.find("Шаг (tau)"));
}

// Симуляция без модели возвращает пустую историю.
TEST(SimulatorTest, RunWithoutModelReturnsEmptyHistory) {
    Simulator simulator(nullptr, 10);
    const SignalGenerator signal(SignalType::STEP, 1.0);

    EXPECT_TRUE(simulator.run(signal).empty());
    EXPECT_FALSE(simulator.hasModel());
    EXPECT_EQ("N/A", simulator.modelName());
}

// Служебные геттеры симулятора.
TEST(SimulatorTest, AccessorsReturnModelInfo) {
    auto model = std::make_unique<LinearModel13>(0.5, 0.1, 1.0);
    Simulator simulator(std::move(model), 15);

    EXPECT_TRUE(simulator.hasModel());
    EXPECT_EQ(15, simulator.stepsCount());
    EXPECT_EQ("Model 1.3 (Linear 2nd order)", simulator.modelName());
}

// Ноль шагов даёт пустую историю.
TEST(SimulatorTest, ZeroStepsProduceEmptyHistory) {
    auto model = std::make_unique<LinearModel13>(0.5, 0.1, 1.0);
    Simulator simulator(std::move(model), 0);
    const SignalGenerator signal(SignalType::STEP, 1.0);

    EXPECT_TRUE(simulator.run(signal).empty());
}
