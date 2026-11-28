#include <gtest/gtest.h>
#include "../src/models.h"
#include <string>
#include "../src/stability.h"
#include <limits>
#include <stdexcept>

TEST(StabilityTest, StableWhenRootInsideUnitCircle) {
    EXPECT_EQ(checkStability(0.5), StabilityStatus::Stable);
    EXPECT_EQ(checkStability(-0.8), StabilityStatus::Stable);
}

TEST(StabilityTest, BoundaryWhenRootOnUnitCircle) {
    EXPECT_EQ(checkStability(1.0), StabilityStatus::Boundary);
    EXPECT_EQ(checkStability(-1.0), StabilityStatus::Boundary);
}

TEST(StabilityTest, UnstableWhenRootOutsideUnitCircle) {
    EXPECT_EQ(checkStability(1.5), StabilityStatus::Unstable);
    EXPECT_EQ(checkStability(-1.2), StabilityStatus::Unstable);
}

TEST(StabilityTest, RejectsNaNCoefficient) {
    EXPECT_THROW(
        checkStability(std::numeric_limits<double>::quiet_NaN()),
        std::invalid_argument
    );
}

TEST(StabilityTest, RejectsInfiniteCoefficient) {
    EXPECT_THROW(
        checkStability(std::numeric_limits<double>::infinity()),
        std::invalid_argument
    );
}

TEST(StabilityIntegrationTest, StableModelCheckedBeforeSimulation) {
    Model110 model(0.5, 1.0, 0.5);

    testing::internal::CaptureStdout();
    auto status = checkModelBeforeSimulation(model);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(status, StabilityStatus::Stable);
    EXPECT_NE(output.find("asymptotically stable"), std::string::npos);
}

TEST(StabilityIntegrationTest, BoundaryModelCheckedBeforeSimulation) {
    Model110 model(1.0, 1.0, 0.5);

    testing::internal::CaptureStdout();
    auto status = checkModelBeforeSimulation(model);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(status, StabilityStatus::Boundary);
    EXPECT_NE(output.find("stability boundary"), std::string::npos);
}

TEST(StabilityIntegrationTest, UnstableModelWarnedBeforeSimulation) {
    Model110 model(1.2, 1.0, 0.5);

    testing::internal::CaptureStdout();
    auto status = checkModelBeforeSimulation(model);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(status, StabilityStatus::Unstable);
    EXPECT_NE(output.find("unstable"), std::string::npos);
}
