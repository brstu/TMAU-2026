#ifndef TASK03_AS06818_CSV_WRITER_HPP
#define TASK03_AS06818_CSV_WRITER_HPP

#include <string>
#include <vector>

#include "control_system.hpp"

/// @file csv_writer.hpp
/// Экспорт результатов симуляции в CSV.

/// Запись процесса регулирования в CSV-файл с заголовком "tau,w,e,u,y".
/// Возвращает true при успешной записи.
bool write_csv(const std::string& filename,
               const std::vector<ControllerPoint>& points);

/// Запись процесса без регулятора в CSV-файл с заголовком "tau,u,y".
bool write_csv(const std::string& filename,
               const std::vector<SimulationPoint>& points);

#endif
