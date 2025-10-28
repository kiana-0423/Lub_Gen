#pragma once

#include <string>

#include "lubgen/table.h"

namespace lubgen {

struct CSVReaderOptions {
    char delimiter = ',';
    bool hasHeader = true;
};

/**
 * Reads CSV files into in-memory tables.
 */
class CSVReader {
public:
    static Table read(const std::string& filePath, const CSVReaderOptions& options = {});
};

}  // namespace lubgen

