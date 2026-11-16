#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../src/csv_writer.hpp"

// Вспомогательная функция: чтение всего файла в строку.
std::string read_file(const std::string& filename) {
    std::ifstream file(filename.c_str());
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Корректный экспорт: заголовок и строки с разделителями-запятыми.
TEST(CsvWriterTest, WritesHeaderAndRows) {
    std::vector<SimulationPoint> points;
    points.push_back(SimulationPoint{0, 1.0, 0.3});
    points.push_back(SimulationPoint{1, 0.0, 0.25});

    const std::string filename = "task02_test_output.csv";
    ASSERT_TRUE(write_csv(filename, points));

    EXPECT_EQ(read_file(filename), "tau,u,y\n0,1,0.3\n1,0,0.25\n");

    std::remove(filename.c_str());
}

// Пустой список точек: записывается только заголовок.
TEST(CsvWriterTest, EmptyTrajectoryWritesHeaderOnly) {
    const std::vector<SimulationPoint> points;

    const std::string filename = "task02_test_empty.csv";
    ASSERT_TRUE(write_csv(filename, points));

    EXPECT_EQ(read_file(filename), "tau,u,y\n");

    std::remove(filename.c_str());
}

// Некорректное имя файла (несуществующий каталог) => false.
TEST(CsvWriterTest, FailsForInvalidPath) {
    const std::vector<SimulationPoint> points;
    EXPECT_FALSE(write_csv("/nonexistent_directory_as06818/out.csv", points));
}

// Пустое имя файла отклоняется.
TEST(CsvWriterTest, FailsForEmptyFilename) {
    const std::vector<SimulationPoint> points;
    EXPECT_FALSE(write_csv("", points));
}
