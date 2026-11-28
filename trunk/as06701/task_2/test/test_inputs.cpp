#include <gtest/gtest.h>
#include "models.h"
#include <cmath>

TEST(Input, StepAlwaysOne) {
    for (int t = 0; t < 10; ++t) EXPECT_DOUBLE_EQ(genInput(InputType::Step, t), 1.0);
}

TEST(Input, ImpulseOnlyFirst) {
    EXPECT_DOUBLE_EQ(genInput(InputType::Impulse, 0), 1.0);
    for (int t = 1; t < 10; ++t) EXPECT_DOUBLE_EQ(genInput(InputType::Impulse, t), 0.0);
}

TEST(Input, HarmonicIsSin) {
    for (int t = 0; t < 10; ++t)
        EXPECT_NEAR(genInput(InputType::Harmonic, t), std::sin(static_cast<double>(t)), 1e-9);
}

TEST(Input, NamesCorrect) {
    EXPECT_STREQ(inputName(InputType::Step),     "step");
    EXPECT_STREQ(inputName(InputType::Impulse),  "impulse");
    EXPECT_STREQ(inputName(InputType::Harmonic), "harmonic");
}
