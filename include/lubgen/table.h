#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace lubgen {

/**
 * Represents an in-memory table loaded from a CSV file.
 */
class Table {
public:
    using Row = std::vector<std::string>;

    Table() = default;
    Table(std::vector<std::string> header, std::vector<Row> rows);

    std::size_t rowCount() const noexcept;
    std::size_t columnCount() const noexcept;

    const std::vector<std::string>& columns() const noexcept;
    const std::vector<Row>& rows() const noexcept;

    bool hasColumn(const std::string& name) const noexcept;
    std::optional<std::size_t> tryColumnIndex(const std::string& name) const noexcept;
    std::size_t columnIndex(const std::string& name) const;

    const Row& row(std::size_t index) const;
    const std::string& value(std::size_t rowIndex, std::size_t columnIndex) const;
    const std::string& value(std::size_t rowIndex, const std::string& columnName) const;

private:
    std::vector<std::string> header_;
    std::vector<Row> rows_;
    std::unordered_map<std::string, std::size_t> columnIndex_;
};

}  // namespace lubgen

