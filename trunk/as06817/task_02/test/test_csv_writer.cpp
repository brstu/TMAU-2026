#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "CsvWriter.h"

namespace {

SimulationResult makeResult() {
    SimulationResult result;
    result.steps.push_back(SimulationStep{0, 1.0, 1.0});
    result.steps.push_back(SimulationStep{1, 0.5, 1.25});
    return result;
}

} // namespace

TEST(CsvWriterTest, FormatsHeaderAndRows) {
    const SimulationResult result = makeResult();

    EXPECT_EQ(CsvWriter::toCsv(result),
              "tau,u,y\n"
              "0,1,1\n"
              "1,0.5,1.25\n");
}

TEST(CsvWriterTest, EmptyResultContainsOnlyHeader) {
    const SimulationResult result;

    EXPECT_EQ(CsvWriter::toCsv(result), "tau,u,y\n");
}

TEST(CsvWriterTest, WritesFileThatCanBeReadBack) {
    const SimulationResult result = makeResult();
    const std::string fileName = "task02_csv_writer_test.csv";

    CsvWriter::write(fileName, result);

    std::ifstream input(fileName);
    ASSERT_TRUE(input.is_open());

    const std::string content((std::istreambuf_iterator<char>(input)),
                              std::istreambuf_iterator<char>());
    EXPECT_EQ(content, CsvWriter::toCsv(result));

    input.close();
    std::remove(fileName.c_str());
}

TEST(CsvWriterTest, ThrowsWhenFileCannotBeOpened) {
    const SimulationResult result = makeResult();

    EXPECT_THROW(
        CsvWriter::write("task02_missing_directory/output.csv", result),
        CsvWriterException);
}
