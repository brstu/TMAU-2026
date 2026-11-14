#pragma once

#include "Simulator.h"

#include <string>
#include <vector>

class ResultWriter {
public:
    static bool saveCsv(const std::vector<ResultRow>& rows, const std::string& path);
};
