#ifndef CSV_READER_HPP_
#define CSV_READER_HPP_

#include <string>
#include <vector>

#include "Types.hpp"

class CSVReader {
public:
    static auto loadPlayersFromCSVFile(const std::string &filename) -> std::vector<PlayerData>;

    static auto loadAdminsFromCSVFile(const std::string &filename) -> std::vector<AdminData>;

private:
    static auto parseCSVLine(const std::string &line) -> std::vector<std::string>;
};

#endif // CSV_READER_HPP_