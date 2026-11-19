#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string>

#include "app.hpp"

TEST(App, WarningIsPrintedBeforeSimulationStarts) {
    const auto dir = std::filesystem::temp_directory_path() / "task02_app_unstable";
    std::istringstream in("1\n1\n1.5\n0\n0\n1\n5\n");
    std::ostringstream out;

    EXPECT_EQ(runApp(in, out, dir), 0);

    const std::string text = out.str();
    const auto warning = text.find("ВНИМАНИЕ");
    const auto table = text.find("tau");
    ASSERT_NE(warning, std::string::npos);
    ASSERT_NE(table, std::string::npos);
    EXPECT_LT(warning, table);
    std::filesystem::remove_all(dir);
}

TEST(App, StableModelRunsWithoutWarning) {
    const auto dir = std::filesystem::temp_directory_path() / "task02_app_stable";
    std::istringstream in("1\n1\n\n\n\n\n5\n");
    std::ostringstream out;

    EXPECT_EQ(runApp(in, out, dir), 0);

    EXPECT_EQ(out.str().find("ВНИМАНИЕ"), std::string::npos);
    std::filesystem::remove_all(dir);
}