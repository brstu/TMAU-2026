#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "DiffEqModel.h"
#include "InputSignal.h"
#include "LinearModel.h"
#include "SimulationRunner.h"

using testing::_;
using testing::DoubleEq;
using testing::InSequence;
using testing::Return;

class MockSignal : public Signal {
public:
    MOCK_METHOD(double, value, (int step, double time), (const, override));
};

TEST(RunnerTest, RequestsEachSampleOnceInOrder) {
    MockSignal signal;
    {
        InSequence sequence;
        EXPECT_CALL(signal, value(0, DoubleEq(0.0))).WillOnce(Return(1.0));
        EXPECT_CALL(signal, value(1, DoubleEq(0.1))).WillOnce(Return(0.0));
        EXPECT_CALL(signal, value(2, DoubleEq(0.2))).WillOnce(Return(-1.0));
    }

    LinearModel model(0.5, 2.0, 1.0, 0.1);
    std::ostringstream table;
    const SimulationRecord record = runSimulation(model, signal, 3, 0.1, table);

    ASSERT_EQ(record.input.size(), 3u);
    EXPECT_DOUBLE_EQ(record.input[0], 1.0);
    EXPECT_DOUBLE_EQ(record.input[1], 0.0);
    EXPECT_DOUBLE_EQ(record.input[2], -1.0);

    ASSERT_EQ(record.output.size(), 3u);
    EXPECT_DOUBLE_EQ(record.output[0], 0.5);
    EXPECT_DOUBLE_EQ(record.output[1], -1.75);
    EXPECT_DOUBLE_EQ(record.output[2], -2.875);
    EXPECT_FALSE(record.diverged);

    EXPECT_NE(table.str().find("Output Y"), std::string::npos);
}

TEST(RunnerTest, WritesCsvWithHeaderAndSamples) {
    MockSignal signal;
    EXPECT_CALL(signal, value(_, _)).Times(2).WillRepeatedly(Return(1.0));

    DiffEqModel model(0.0, 0.0, 1.0, 0.0, 0.1);
    std::ostringstream table;
    const SimulationRecord record = runSimulation(model, signal, 2, 0.1, table);

    const std::string path = "runner_output.csv";
    std::remove(path.c_str());
    ASSERT_TRUE(writeCsv(path, record));

    std::ifstream input(path);
    ASSERT_TRUE(input.is_open());

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    input.close();
    std::remove(path.c_str());

    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], "Time,Input,Output");

    const auto parseRow = [](const std::string& row) {
        std::stringstream ss(row);
        std::string time;
        std::string input_value;
        std::string output_value;
        std::getline(ss, time, ',');
        std::getline(ss, input_value, ',');
        std::getline(ss, output_value, ',');
        return std::make_tuple(std::stod(time), std::stod(input_value), std::stod(output_value));
    };

    const auto [time0, input0, output0] = parseRow(lines[1]);
    EXPECT_NEAR(time0, 0.0, 1e-9);
    EXPECT_NEAR(input0, 1.0, 1e-9);
    EXPECT_NEAR(output0, 0.1, 1e-9);

    const auto [time1, input1, output1] = parseRow(lines[2]);
    EXPECT_NEAR(time1, 0.1, 1e-9);
    EXPECT_NEAR(input1, 1.0, 1e-9);
    EXPECT_NEAR(output1, 0.2, 1e-9);
}

TEST(RunnerTest, DetectsDivergentProcess) {
    LinearModel model(10.0, 1.0, 1.0, 0.1);
    StepSignal signal;
    std::ostringstream table;

    const SimulationRecord record = runSimulation(model, signal, 200, 0.1, table);
    EXPECT_TRUE(record.diverged);
}

TEST(RunnerTest, WriteCsvReturnsFalseWhenPathIsInvalid) {
    SimulationRecord record;
    record.time.push_back(0.0);
    record.input.push_back(1.0);
    record.output.push_back(0.1);

    EXPECT_FALSE(writeCsv("no_such_directory\\output.csv", record));
}

TEST(RunnerTest, StableProcessIsNotMarkedAsDivergent) {
    LinearModel model(0.5, 1.0, 1.0, 0.1);
    StepSignal signal;
    std::ostringstream table;

    const SimulationRecord record = runSimulation(model, signal, 50, 0.1, table);
    EXPECT_FALSE(record.diverged);
    EXPECT_TRUE(std::isfinite(record.output.back()));
}
