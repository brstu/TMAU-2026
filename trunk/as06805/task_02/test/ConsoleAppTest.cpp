#include <gtest/gtest.h>

#include <cstdio>
#include <sstream>
#include <string>

#include "ConsoleApp.h"

namespace {

// Прогон интерактивного сценария с подготовленным вводом.
struct RunResult {
    int exitCode = 0;
    std::string output;
};

RunResult runWithInput(const std::string& input, const AppOptions& options = AppOptions()) {
    std::istringstream in(input);
    std::ostringstream out;

    RunResult result;
    result.exitCode = runApplication(in, out, options);
    result.output = out.str();
    return result;
}

}  // namespace

// Сценарий с устойчивой моделью завершается успешно и печатает таблицу.
TEST(ConsoleAppTest, StableModelRunsSimulationSuccessfully) {
    // Модель 2 (LinearModel13, a1=0.5, a2=0.1, b=1.0), сигнал STEP, 5 шагов.
    const RunResult result = runWithInput("2\n1\n1.0\n5\n0.5\n0.1\n1.0\n");

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("Model 1.3"));
    EXPECT_NE(std::string::npos, result.output.find("Шаг (tau)"));
    EXPECT_NE(std::string::npos, result.output.find("устойчива"));
}

// Для неустойчивой модели выводится предупреждение перед началом симуляции.
TEST(ConsoleAppTest, UnstableModelProducesWarning) {
    // Модель 1 (LinearModel11) с a = 1.5 => полюс вне единичной окружности.
    const RunResult result = runWithInput("1\n1\n1.0\n5\n1.5\n1.0\n1\n");

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("ВНИМАНИЕ"));
    EXPECT_NE(std::string::npos, result.output.find("Продолжить симуляцию"));
}

// Пользователь может отказаться от симуляции неустойчивого процесса.
TEST(ConsoleAppTest, UnstableSimulationCanBeCancelledByUser) {
    const RunResult result = runWithInput("1\n1\n1.0\n5\n1.5\n1.0\n0\n");

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("ВНИМАНИЕ"));
    EXPECT_NE(std::string::npos, result.output.find("Симуляция прервана пользователем"));
    EXPECT_EQ(std::string::npos, result.output.find("Шаг (tau)"));
}

// Неверный выбор модели приводит к запросу корректного значения.
TEST(ConsoleAppTest, InvalidModelChoiceIsRejected) {
    const RunResult result = runWithInput("9\n2\n1\n1.0\n5\n0.5\n0.1\n1.0\n");

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("Ошибка: введите целое число"));
    EXPECT_NE(std::string::npos, result.output.find("Model 1.3"));
}

// Неверный числовой параметр приводит к запросу корректного значения.
TEST(ConsoleAppTest, InvalidNumberIsRejected) {
    const RunResult result = runWithInput("1\n1\n1.0\n5\nabc\n0.5\n1.0\n");

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("Ошибка: введите корректное число"));
    EXPECT_NE(std::string::npos, result.output.find("Model 1.1"));
}

// Поддерживается ввод десятичной запятой.
TEST(ConsoleAppTest, CommaIsAcceptedAsDecimalSeparator) {
    const RunResult result = runWithInput("1\n1\n1.0\n3\n0,5\n1,0\n");

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("Model 1.1"));
}

// Модель 3.7 (метод Эйлера) выбирается и симулируется.
TEST(ConsoleAppTest, ODESimulationIsSupported) {
    const RunResult result = runWithInput("6\n1\n1.0\n4\n0.1\n1.0\n0.1\n");

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("Model 3.7"));
    EXPECT_NE(std::string::npos, result.output.find("устойчива"));
}

// Нелинейные модели также доступны в интерактивном сценарии.
TEST(ConsoleAppTest, NonlinearModelsAreSupported) {
    const RunResult sat = runWithInput("3\n1\n1.0\n3\n0.7\n0.3\n1.0\n-1.0\n");
    EXPECT_EQ(0, sat.exitCode);
    EXPECT_NE(std::string::npos, sat.output.find("Model 2.2"));

    const RunResult dz = runWithInput("4\n1\n1.0\n3\n0.7\n0.3\n0.1\n");
    EXPECT_EQ(0, dz.exitCode);
    EXPECT_NE(std::string::npos, dz.output.find("Model 2.3"));

    const RunResult fric = runWithInput("5\n1\n1.0\n3\n0.8\n0.5\n");
    EXPECT_EQ(0, fric.exitCode);
    EXPECT_NE(std::string::npos, fric.output.find("Model 2.5"));
}

// Гармонический сигнал выбирается и попадает в таблицу результатов.
TEST(ConsoleAppTest, HarmonicSignalIsSupported) {
    const RunResult result = runWithInput("2\n3\n2.0\n5\n0.5\n0.1\n1.0\n");

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("Model 1.3"));
}

// При пустых именах файлов экспорт результатов не выполняется.
TEST(ConsoleAppTest, ExportIsSkippedForEmptyFileNames) {
    AppOptions options;
    options.csvFileName.clear();
    options.txtFileName.clear();

    const RunResult result = runWithInput("1\n1\n1.0\n3\n0.5\n1.0\n", options);

    EXPECT_EQ(0, result.exitCode);
    EXPECT_NE(std::string::npos, result.output.find("Model 1.1"));
}
