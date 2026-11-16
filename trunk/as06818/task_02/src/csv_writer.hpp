#ifndef TASK02_AS06818_CSV_WRITER_HPP
#define TASK02_AS06818_CSV_WRITER_HPP

#include <string>
#include <vector>

#include "simulator.hpp"

// Экспорт траектории симуляции в CSV-файл с заголовком "tau,u,y".
// Возвращает true при успешной записи.
bool write_csv(const std::string& filename,
               const std::vector<SimulationPoint>& points);

#endif
