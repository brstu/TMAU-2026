#include "csv_writer.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

TEST(CsvWriterTest, SavesSamplesWithModelAndSignalMetadata) {
    const auto filePath = std::filesystem::temp_directory_path()
        / "timay_variant12_task02_test.csv";
    const std::vector<Sample> samples{{0, 1.0, 0.0}, {1, 0.0, 0.5}};

    ASSERT_TRUE(CsvWriter::save(
        filePath.string(), samples,
        "Model 1.2", "Impulse signal"
    ));

    std::ifstream file(filePath, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    const std::string content(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    file.close();
    std::error_code error;
    std::filesystem::remove(filePath, error);

    EXPECT_NE(content.find("tau,u_tau,y_tau,model,signal"), std::string::npos);
    EXPECT_NE(content.find("Model 1.2"), std::string::npos);
    EXPECT_NE(content.find("Impulse signal"), std::string::npos);
    EXPECT_NE(content.find("0,1,0"), std::string::npos);
}


TEST(CsvWriterTest, EscapesQuotesAndCommasInMetadata) {
    const auto filePath = std::filesystem::temp_directory_path()
        / "timay_variant12_csv_escape_test.csv";
    const std::vector<Sample> samples{{2, 0.5, 1.25}};

    ASSERT_TRUE(CsvWriter::save(
        filePath.string(), samples,
        "Model \"1.2\", test", "Impulse, signal"
    ));

    std::ifstream file(filePath, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    const std::string content(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    file.close();
    std::error_code error;
    std::filesystem::remove(filePath, error);

    EXPECT_NE(content.find("\"Model \"\"1.2\"\", test\""), std::string::npos);
    EXPECT_NE(content.find("\"Impulse, signal\""), std::string::npos);
}

TEST(CsvWriterTest, ReturnsFalseWhenOutputFileCannotBeOpened) {
    const auto invalidPath = std::filesystem::temp_directory_path()
        / "timay_missing_directory_for_csv_test"
        / "results.csv";
    std::error_code error;
    std::filesystem::remove_all(invalidPath.parent_path(), error);

    EXPECT_FALSE(CsvWriter::save(
        invalidPath.string(), {}, "Model 1.2", "Step signal"
    ));
}

TEST(CsvWriterTest, PrintsTableWithSampleValues) {
    const std::vector<Sample> samples{{1, 0.5, 1.25}};
    std::ostringstream captured;
    std::streambuf* originalBuffer = std::cout.rdbuf(captured.rdbuf());
    const auto originalFlags = std::cout.flags();
    const auto originalPrecision = std::cout.precision();

    CsvWriter::printTable(samples);

    std::cout.rdbuf(originalBuffer);
    std::cout.flags(originalFlags);
    std::cout.precision(originalPrecision);
    const std::string output = captured.str();

    EXPECT_NE(output.find("Результаты моделирования"), std::string::npos);
    EXPECT_NE(output.find("u_tau"), std::string::npos);
    EXPECT_NE(output.find("y_tau"), std::string::npos);
    EXPECT_NE(output.find("1.250000"), std::string::npos);
}

TEST(CsvWriterTest, PrintsEmptyTableWithoutRows) {
    std::ostringstream captured;
    std::streambuf* originalBuffer = std::cout.rdbuf(captured.rdbuf());
    const auto originalFlags = std::cout.flags();
    const auto originalPrecision = std::cout.precision();

    CsvWriter::printTable({});

    std::cout.rdbuf(originalBuffer);
    std::cout.flags(originalFlags);
    std::cout.precision(originalPrecision);
    const std::string output = captured.str();

    EXPECT_NE(output.find("Результаты моделирования"), std::string::npos);
    EXPECT_NE(output.find("tau"), std::string::npos);
}
