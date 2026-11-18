#include "RelayModel27.h"
#include "gtest/gtest.h"

TEST(RelayModelTests, PositiveInputSwitchesRelayToPositiveState) {
    RelayModel27 model(0.5, 2.0, 0.1);
    EXPECT_DOUBLE_EQ(model.step(0.2, 0.1), 2.0);
}

TEST(RelayModelTests, NegativeInputSwitchesRelayToNegativeState) {
    RelayModel27 model(0.5, 2.0, 0.1);
    EXPECT_DOUBLE_EQ(model.step(-0.2, 0.1), -2.0);
}

TEST(RelayModelTests, PositiveBoundaryUsesHysteresisState) {
    RelayModel27 model(0.5, 2.0, 0.1, 0.0);
    EXPECT_DOUBLE_EQ(model.step(0.1, 0.1), -2.0);
}

TEST(RelayModelTests, NegativeBoundaryRetainsPositiveOutputSign) {
    RelayModel27 model(0.5, 2.0, 0.1, 1.0);
    EXPECT_DOUBLE_EQ(model.step(-0.1, 0.1), 2.5);
}
