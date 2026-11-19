#pragma once

#include <filesystem>
#include <istream>
#include <ostream>


constexpr int MODEL_COUNT = 3;
constexpr int INPUT_COUNT = 3;

int runApp(std::istream& in, std::ostream& out, const std::filesystem::path& docDir);
