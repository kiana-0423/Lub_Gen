#include "lubgen/query_engine.h"

#include <algorithm>
#include <stdexcept>

namespace lubgen {

QueryResult::QueryResult(const Table& table, std::vector<std::size_t> rowIndices)
    : table_(&table), rowIndices_(std::move(rowIndices)) {}

std::size_t QueryResult::size() const noexcept { return rowIndices_.size(); }

const std::vector<std::size_t>& QueryResult::rowIndices() const noexcept { return rowIndices_; }

std::vector<Table::Row> QueryResult::materialize() const {
    std::vector<Table::Row> result;
    result.reserve(rowIndices_.size());
    for (auto index : rowIndices_) {
        result.push_back(table_->row(index));
    }
    return result;
}

std::vector<std::string> QueryResult::column(const std::string& columnName) const {
    std::vector<std::string> values;
    auto columnIndex = table_->columnIndex(columnName);
    values.reserve(rowIndices_.size());
    for (auto index : rowIndices_) {
        values.push_back(table_->value(index, columnIndex));
    }
    return values;
}

std::vector<double> QueryResult::numericColumn(const std::string& columnName) const {
    auto raw = column(columnName);
    std::vector<double> values;
    values.reserve(raw.size());
    for (const auto& token : raw) {
        try {
            values.push_back(std::stod(token));
        } catch (const std::exception&) {
            throw std::runtime_error("Unable to convert value '" + token + "' to double");
        }
    }
    return values;
}

void QueryResult::writeCSV(std::ostream& output, char delimiter) const {
    const auto& header = table_->columns();
    for (std::size_t i = 0; i < header.size(); ++i) {
        if (i > 0) {
            output << delimiter;
        }
        output << header[i];
    }
    output << '\n';

    for (auto index : rowIndices_) {
        const auto& row = table_->row(index);
        for (std::size_t i = 0; i < row.size(); ++i) {
            if (i > 0) {
                output << delimiter;
            }
            bool needsQuotes = row[i].find_first_of(",\"\n") != std::string::npos;
            if (needsQuotes) {
                output << '"';
                for (char ch : row[i]) {
                    if (ch == '"') {
                        output << "\"\"";
                    } else {
                        output << ch;
                    }
                }
                output << '"';
            } else {
                output << row[i];
            }
        }
        output << '\n';
    }
}

QueryEngine::QueryEngine(const Table& table) noexcept : table_(&table) {}

QueryResult QueryEngine::head(std::size_t count) const {
    count = std::min<std::size_t>(count, table_->rowCount());
    std::vector<std::size_t> indices(count);
    for (std::size_t i = 0; i < count; ++i) {
        indices[i] = i;
    }
    return QueryResult(*table_, std::move(indices));
}

QueryResult QueryEngine::whereEquals(const std::string& columnName, const std::string& value) const {
    auto columnIndex = table_->columnIndex(columnName);
    std::vector<std::size_t> indices;
    indices.reserve(table_->rowCount());
    for (std::size_t rowIndex = 0; rowIndex < table_->rowCount(); ++rowIndex) {
        if (table_->value(rowIndex, columnIndex) == value) {
            indices.push_back(rowIndex);
        }
    }
    return QueryResult(*table_, std::move(indices));
}

}  // namespace lubgen
