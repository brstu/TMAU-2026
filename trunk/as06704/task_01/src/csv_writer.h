#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "simulator.h"

#include <string>
#include <vector>

class CsvWriter {
public:
    static bool save(const std::string& filename,
                     const std::vector<Sample>& samples,
                     const std::string& modelName,
                     const std::string& signalName);
    static void printTable(const std::vector<Sample>& samples);
};

#endif
