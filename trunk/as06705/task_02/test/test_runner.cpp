#include "InputSignals.hpp"
#include "Models.hpp"
#include "SimulationRunner.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

class MockSignal : public ISignal {
public:
    MOCK_METHOD(double, at, (int tau), (const, override));
    MOCK_METHOD(const char*, id, (), (const, override));
};

TEST(RunnerTest, ExecuteCallsSignalForEachStep) {
    MockSignal sig;
    EXPECT_CALL(sig, at(_)).Times(5).WillRepeatedly(Return(1.0));

    Runner r(std::make_unique<ThirdOrderLinear>(0.0, 0.0, 0.0, 1.0), 5);
    auto tr = r.execute(sig);

    ASSERT_EQ(tr.y.size(), 5u);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_DOUBLE_EQ(tr.y[i], 1.0);
        EXPECT_DOUBLE_EQ(tr.u[i], 1.0);
    }
}

TEST(RunnerTest, ExecuteWithPresetSequence) {
    MockSignal sig;
    {
        InSequence s;
        EXPECT_CALL(sig, at(0)).WillOnce(Return(1.0));
        EXPECT_CALL(sig, at(1)).WillOnce(Return(2.0));
        EXPECT_CALL(sig, at(2)).WillOnce(Return(3.0));
    }

    Runner r(std::make_unique<ThirdOrderLinear>(0.0, 0.0, 0.0, 1.0), 3);
    auto tr = r.execute(sig);

    ASSERT_EQ(tr.y.size(), 3u);
    EXPECT_DOUBLE_EQ(tr.y[0], 1.0);
    EXPECT_DOUBLE_EQ(tr.y[1], 2.0);
    EXPECT_DOUBLE_EQ(tr.y[2], 3.0);
}

TEST(RunnerTest, DivergedDetection) {
    Runner r(std::make_unique<ThirdOrderLinear>(2.0, 0.0, 0.0, 1.0), 2000);
    StepSignal s(1.0);
    auto tr = r.execute(s);
    EXPECT_TRUE(Runner::diverged(tr));
}

TEST(RunnerTest, NotDivergedForStable) {
    Runner r(std::make_unique<ThirdOrderLinear>(0.5, 0.0, 0.0, 1.0), 200);
    StepSignal s(1.0);
    auto tr = r.execute(s);
    EXPECT_FALSE(Runner::diverged(tr));
}

TEST(RunnerTest, DumpCsvCreatesValidFile) {
    Runner r(std::make_unique<ThirdOrderLinear>(0.5, 0.0, 0.0, 1.0), 3);
    StepSignal s(1.0);
    auto tr = r.execute(s);
    const std::string path = "test_dump.csv";
    Runner::dumpCsv(tr, path);

    std::ifstream f(path);
    ASSERT_TRUE(f.good());
    std::string header;
    std::getline(f, header);
    EXPECT_EQ(header, "tau,u,y");
    std::string line;
    std::getline(f, line);
    EXPECT_FALSE(line.empty());
    f.close();
    std::remove(path.c_str());
}