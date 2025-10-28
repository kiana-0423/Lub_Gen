#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <utility>

#include "lubgen/csv_reader.h"
#include "lubgen/query_engine.h"

namespace {

struct CommandLineOptions {
    std::string filePath;
    std::optional<std::pair<std::string, std::string>> equalityFilter;
    std::size_t previewCount = 5;
};

void printUsage(const char* executable) {
    std::cerr << "Usage: " << executable << " <csv-file> [--where column=value] [--head n]\n";
}

std::optional<CommandLineOptions> parseArguments(int argc, char** argv) {
    if (argc < 2) {
        return std::nullopt;
    }

    CommandLineOptions options;
    options.filePath = argv[1];

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--where") {
            if (i + 1 >= argc) {
                std::cerr << "--where requires an argument in the form column=value\n";
                return std::nullopt;
            }
            std::string expression = argv[++i];
            auto separator = expression.find('=');
            if (separator == std::string::npos) {
                std::cerr << "--where argument must contain '=' (column=value)\n";
                return std::nullopt;
            }
            std::string column = expression.substr(0, separator);
            std::string value = expression.substr(separator + 1);
            options.equalityFilter = std::make_pair(std::move(column), std::move(value));
        } else if (arg == "--head") {
            if (i + 1 >= argc) {
                std::cerr << "--head requires a numeric argument\n";
                return std::nullopt;
            }
            try {
                options.previewCount = static_cast<std::size_t>(std::stoul(argv[++i]));
            } catch (const std::exception&) {
                std::cerr << "--head argument must be an unsigned integer\n";
                return std::nullopt;
            }
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            return std::nullopt;
        }
    }

    return options;
}

}  // namespace

int main(int argc, char** argv) {
    auto maybeOptions = parseArguments(argc, argv);
    if (!maybeOptions.has_value()) {
        printUsage(argv[0]);
        return 1;
    }

    try {
        lubgen::Table table = lubgen::CSVReader::read(maybeOptions->filePath);
        lubgen::QueryEngine engine(table);

        if (maybeOptions->equalityFilter.has_value()) {
            auto [column, value] = maybeOptions->equalityFilter.value();
            auto result = engine.whereEquals(column, value);
            std::cerr << "Matched rows: " << result.size() << '\n';
            if (result.size() == 0) {
                return 0;
            }
            result.writeCSV(std::cout);
        } else {
            std::cerr << "Loaded " << table.rowCount() << " rows across " << table.columnCount()
                      << " columns. Showing first " << maybeOptions->previewCount << " rows.\n";
            engine.head(maybeOptions->previewCount).writeCSV(std::cout);
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}

