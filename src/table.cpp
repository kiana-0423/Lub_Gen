#include "lubgen/table.h"

#include <stdexcept>

namespace lubgen {

Table::Table(std::vector<std::string> header, std::vector<Row> rows)
    : header_(std::move(header)), rows_(std::move(rows)) {
    for (std::size_t index = 0; index < header_.size(); ++index) {
        columnIndex_.emplace(header_[index], index);
    }
}

std::size_t Table::rowCount() const noexcept { return rows_.size(); }

std::size_t Table::columnCount() const noexcept { return header_.size(); }

const std::vector<std::string>& Table::columns() const noexcept { return header_; }

const std::vector<Table::Row>& Table::rows() const noexcept { return rows_; }

bool Table::hasColumn(const std::string& name) const noexcept {
    return columnIndex_.find(name) != columnIndex_.end();
}

std::optional<std::size_t> Table::tryColumnIndex(const std::string& name) const noexcept {
    auto it = columnIndex_.find(name);
    if (it == columnIndex_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::size_t Table::columnIndex(const std::string& name) const {
    auto index = tryColumnIndex(name);
    if (!index.has_value()) {
        throw std::out_of_range("Unknown column: " + name);
    }
    return index.value();
}

const Table::Row& Table::row(std::size_t index) const {
    if (index >= rows_.size()) {
        throw std::out_of_range("Row index out of bounds");
    }
    return rows_[index];
}

const std::string& Table::value(std::size_t rowIndex, std::size_t columnIndex) const {
    const auto& r = row(rowIndex);
    if (columnIndex >= r.size()) {
        throw std::out_of_range("Column index out of bounds");
    }
    return r[columnIndex];
}

const std::string& Table::value(std::size_t rowIndex, const std::string& columnName) const {
    return value(rowIndex, columnIndex(columnName));
}

}  // namespace lubgen

