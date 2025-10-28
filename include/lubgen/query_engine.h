#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

#include "lubgen/table.h"

namespace lubgen {

class QueryResult {
public:
    QueryResult(const Table& table, std::vector<std::size_t> rowIndices);

    std::size_t size() const noexcept;
    const std::vector<std::size_t>& rowIndices() const noexcept;

    std::vector<Table::Row> materialize() const;
    std::vector<std::string> column(const std::string& columnName) const;
    std::vector<double> numericColumn(const std::string& columnName) const;
    void writeCSV(std::ostream& output, char delimiter = ',') const;

private:
    const Table* table_;
    std::vector<std::size_t> rowIndices_;
};

class QueryEngine {
public:
    explicit QueryEngine(const Table& table) noexcept;

    QueryResult head(std::size_t count) const;
    QueryResult whereEquals(const std::string& columnName, const std::string& value) const;

private:
    const Table* table_;
};

}  // namespace lubgen
