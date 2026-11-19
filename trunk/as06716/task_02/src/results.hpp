#pragma once

#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

#include "simulator.hpp"


void printTable(std::ostream& out, const std::string& model, const std::string& input, const std::vector<Sample>& samples);

void writeCsv(std::ostream& out, const std::string& model, const std::string& input, const std::vector<Sample>& samples);

bool saveCsv(const std::filesystem::path& file, const std::string& model, const std::string& input, const std::vector<Sample>& samples);
