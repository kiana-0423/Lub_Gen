#include "lubgen/csv_reader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {

std::vector<std::string> parseRow(std::istream& input, char delimiter) {
    std::vector<std::string> row;
    std::string field;
    bool inQuotes = false;
    bool anyCharacter = false;

    while (true) {
        int next = input.get();
        if (next == std::char_traits<char>::eof()) {
            if (inQuotes) {
                throw std::runtime_error("Unterminated quoted field in CSV input");
            }
            if (!anyCharacter && field.empty() && row.empty()) {
                return {};
            }
            row.push_back(std::move(field));
            return row;
        }

        anyCharacter = true;
        char ch = static_cast<char>(next);
        if (inQuotes) {
            if (ch == '"') {
                if (input.peek() == '"') {
                    field.push_back('"');
                    input.get();
                } else {
                    inQuotes = false;
                }
            } else {
                field.push_back(ch);
            }
            continue;
        }

        if (ch == '"') {
            inQuotes = true;
            continue;
        }
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            row.push_back(std::move(field));
            return row;
        }
        if (ch == delimiter) {
            row.push_back(std::move(field));
            field.clear();
            continue;
        }

        field.push_back(ch);
    }
}

}  // namespace

namespace lubgen {

Table CSVReader::read(const std::string& filePath, const CSVReaderOptions& options) {
    std::ifstream input(filePath, std::ios::binary);
    if (!input.is_open()) {
        throw std::runtime_error("Unable to open CSV file: " + filePath);
    }

    std::vector<std::string> header;
    std::vector<Table::Row> rows;

    auto readNextRow = [&]() -> std::vector<std::string> {
        return parseRow(input, options.delimiter);
    };

    if (options.hasHeader) {
        header = readNextRow();
        if (header.empty()) {
            throw std::runtime_error("CSV file does not contain a header row");
        }
    }

    while (input.good()) {
        auto parsed = readNextRow();
        if (parsed.empty()) {
            break;
        }
        rows.push_back(std::move(parsed));
    }

    if (!options.hasHeader && !rows.empty()) {
        header.resize(rows.front().size());
        for (std::size_t i = 0; i < header.size(); ++i) {
            header[i] = "column_" + std::to_string(i);
        }
    }

    if (!header.empty()) {
        for (auto& row : rows) {
            if (row.size() != header.size()) {
                std::ostringstream message;
                message << "Row width (" << row.size() << ") does not match header size (" << header.size()
                        << ")";
                throw std::runtime_error(message.str());
            }
        }
    }

    return Table(std::move(header), std::move(rows));
}

}  // namespace lubgen
#include <vector>
