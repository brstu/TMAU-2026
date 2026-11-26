#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

#include "../src/control_system.hpp"
#include "../src/csv_writer.hpp"

// Чтение файла в список строк.
std::vector<std::string> read_lines(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

TEST(CsvWriterTest, ControllerPointsHeaderAndRows) {
    const std::vector<ControllerPoint> points = {
        {0, 25.0, 25.0, 0.8, 0.3},
        {1, 25.0, 24.7, 1.1, 0.55},
    };

    ASSERT_TRUE(write_csv("test_ctrl.csv", points));

    const std::vector<std::string> lines = read_lines("test_ctrl.csv");
    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], "tau,w,e,u,y");
    EXPECT_EQ(lines[1], "0,25,25,0.8,0.3");
    EXPECT_EQ(lines[2], "1,25,24.7,1.1,0.55");
}

TEST(CsvWriterTest, SimulationPointsHeaderAndRows) {
    const std::vector<SimulationPoint> points = {
        {0, 1.0, 0.3},
        {1, 1.0, 0.55},
    };

    ASSERT_TRUE(write_csv("test_sim.csv", points));

    const std::vector<std::string> lines = read_lines("test_sim.csv");
    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], "tau,u,y");
    EXPECT_EQ(lines[1], "0,1,0.3");
    EXPECT_EQ(lines[2], "1,1,0.55");
}

TEST(CsvWriterTest, EmptyTrajectoryWritesHeaderOnly) {
    const std::vector<ControllerPoint> points;

    ASSERT_TRUE(write_csv("test_empty.csv", points));
    EXPECT_EQ(read_lines("test_empty.csv").size(), 1u);
}

// Повторная запись в тот же файл перезаписывает его.
TEST(CsvWriterTest, OverwritesExistingFile) {
    const std::vector<ControllerPoint> one = {{0, 1.0, 1.0, 0.5, 0.2}};
    const std::vector<ControllerPoint> two = {
        {0, 1.0, 1.0, 0.5, 0.2},
        {1, 1.0, 0.8, 0.6, 0.4},
    };

    ASSERT_TRUE(write_csv("test_overwrite.csv", one));
    ASSERT_TRUE(write_csv("test_overwrite.csv", two));
    EXPECT_EQ(read_lines("test_overwrite.csv").size(), 3u);
}

TEST(CsvWriterTest, UnwritablePathReturnsFalse) {
    const std::vector<ControllerPoint> points = {{0, 1.0, 1.0, 0.5, 0.2}};
    EXPECT_FALSE(write_csv("/nonexistent_directory_xyz/out.csv", points));
}
